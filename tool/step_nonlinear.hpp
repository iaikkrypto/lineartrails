#include "mask.h"

template <unsigned bitsize>
LinearDistributionTable<bitsize>::LinearDistributionTable(std::function<BitVector(BitVector)> fun) {
  Initialize(fun);
}

template <unsigned bitsize>
void LinearDistributionTable<bitsize>::Initialize(const std::function<BitVector(BitVector)> fun) {
  unsigned int boxsize = (1 << (bitsize));

  //resize vector
  ldt.resize(boxsize, std::vector<signed>(boxsize));
  ldt_bool.resize(boxsize, std::vector<unsigned>(boxsize));

  //calc ldt
  for (unsigned int a = 0; a < boxsize; a++)
    for (unsigned int b = 0; b < boxsize; b++)
      ldt[a][b] = -(boxsize >> 1);

  for (unsigned int x = 0; x < boxsize; x++) {
    for (unsigned int a = 0; a < boxsize; ++a) {
      unsigned int facta = a & x;
      unsigned int asum = 0;
      for (unsigned int j = 0; j < bitsize; ++j)
        asum ^= ((facta >> j) & 1);
      for (unsigned int b = 0; b < boxsize; ++b) {
        unsigned int factb = b & fun(x);
        unsigned int bsum = 0;
        for (unsigned int j = 0; j < bitsize; ++j)
          bsum ^= ((factb >> j) & 1);
        ldt[a][b] += (asum == bsum);
      }
    }
  }

  for (unsigned int a = 0; a < boxsize; ++a)
    for (unsigned int b = 0; b < boxsize; ++b)
      if (ldt[a][b] != 0)
        ldt_bool[a][b] = ~0U;
}

template <unsigned bitsize>
LinearDistributionTable<bitsize>& LinearDistributionTable<bitsize>::operator=(const LinearDistributionTable<bitsize>& rhs){
  ldt_bool = rhs.ldt_bool;
  ldt = rhs.ldt;
  return *this;
}

template <unsigned bitsize>
std::ostream& operator<<(std::ostream& stream, const LinearDistributionTable<bitsize>& ldt) {
  // TODO
  return stream;
}

//-----------------------------------------------------------------------------

template <unsigned bitsize>
NonlinearStep<bitsize>::NonlinearStep(std::function<BitVector(BitVector)> fun) {
  Initialize(fun);
}

template <unsigned bitsize>
void NonlinearStep<bitsize>::Initialize(std::function<BitVector(BitVector)> fun) {
  is_active_ = false;
  is_guessable_ = true;
  ldt_.Initialize(fun);
}

template <unsigned bitsize>
bool NonlinearStep<bitsize>::Update(Mask& x, Mask& y) {
  std::vector<unsigned int> inmasks, outmasks;  // TODO check datatype!
  unsigned int inresult[2] = { 0, 0 };  // TODO check datatype!
  unsigned int outresult[2] = { 0, 0 };
  create_masks(inmasks, x);
  create_masks(outmasks, y);

  for (auto inmask : inmasks)
    for (auto outmask : outmasks) {
      inresult[0] |= (~inmask) & ldt_.ldt_bool[inmask][outmask];
      inresult[1] |= inmask & ldt_.ldt_bool[inmask][outmask];
      outresult[0] |= (~outmask) & ldt_.ldt_bool[inmask][outmask];
      outresult[1] |= outmask & ldt_.ldt_bool[inmask][outmask];
    }

  for (unsigned int i = 0; i < bitsize; ++i) {
    x.bitmasks[bitsize - i - 1] = ((inresult[1] & (1 << i))
        | ((inresult[0] & (1 << i)) << 1)) >> i;
    y.bitmasks[bitsize - i - 1] = ((outresult[1] & (1 << i))
        | ((outresult[0] & (1 << i)) << 1)) >> i;
  }

  x.reinit_caremask();
  y.reinit_caremask();

  if ((inresult[0] | inresult[1]) == 0 || (outresult[0] | outresult[1]) == 0)
    return false;

  if (x.caremask.canbe1 == 0
      || (x.caremask.canbe1 == (~0ULL >> (64 - bitsize)) && x.caremask.care == 0
          && y.caremask.canbe1 == (~0ULL >> (64 - bitsize))
          && y.caremask.care == 0))
    is_active_ = false;
  else
    is_active_ = true;

  is_guessable_ = false;
  for(unsigned int i = 0; i < bitsize; ++i)
    if(x.bitmasks[bitsize - i - 1] == BM_DUNNO || y.bitmasks[bitsize - i - 1] == BM_DUNNO){
      is_guessable_ = true;
      break;
    }


  return true;
}

template <unsigned bitsize>
NonlinearStep<bitsize>& NonlinearStep<bitsize>::operator=(const NonlinearStep<bitsize>& rhs){
  ldt_ = rhs.ldt_;
  is_active_ = rhs.is_active_;
  return *this;
}

template <unsigned bitsize>
void NonlinearStep<bitsize>::TakeBestBox(Mask& x, Mask& y) {
  std::vector<unsigned int> inmasks, outmasks;  // TODO check datatype!
  create_masks(inmasks, x);
  create_masks(outmasks, y);
    int branch_number = 0;
    unsigned int best_inmask = 0;
    unsigned int best_outmask = 0;

  for (auto inmask : inmasks)
    for (auto outmask : outmasks) {
       if(branch_number < std::abs(ldt_.ldt[inmask][outmask])){
         branch_number = std::abs(ldt_.ldt[inmask][outmask]);
         best_inmask = inmask;
         best_outmask = outmask;
       }
    }

  for (unsigned int i = 0; i < bitsize; ++i) {
    x.bitmasks[bitsize - i - 1] = (((best_inmask & (1 << i)) >> i) == 1 ? BM_1 : BM_0);
    y.bitmasks[bitsize - i - 1] = (((best_outmask & (1 << i)) >> i) == 1 ? BM_1 : BM_0);
  }

  if(best_inmask)
    is_active_ = true;
  else
    is_active_ = false;

  is_guessable_ = false;

  x.reinit_caremask();
  y.reinit_caremask();

}

template <unsigned bitsize>
void NonlinearStep<bitsize>::create_masks(std::vector<unsigned int> &masks,
                                          Mask& reference, unsigned int pos,
                                          unsigned int current_mask) {
  if (pos < bitsize) {
    switch (reference.bitmasks[pos]) {
      case BM_1:
        current_mask |= (1 << (bitsize - pos - 1));
        create_masks(masks, reference, ++pos, current_mask);
        break;
      case BM_0:
        create_masks(masks, reference, ++pos, current_mask);
        break;
      case BM_DUNNO:
        create_masks(masks, reference, ++pos, current_mask);
        current_mask |= (1 << (bitsize - (--pos) - 1));
        create_masks(masks, reference, ++pos, current_mask);
        break;
    }
  } else {
    masks.push_back(current_mask);
  }
}

template <unsigned bitsize>
std::ostream& operator<<(std::ostream& stream, const NonlinearStep<bitsize>& step) {
  // TODO
  return stream;
}
