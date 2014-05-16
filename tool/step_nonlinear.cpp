#include "step_nonlinear.h"

LinearDistributionTable::LinearDistributionTable() {
}

LinearDistributionTable::LinearDistributionTable(std::function<BitVector(BitVector)> fun, unsigned bitsize) {
  unsigned int boxsize = (1 << (bitsize));

  //resize vector
  ldt.resize(boxsize, std::vector<signed>(boxsize));
  ldt_bool.resize(boxsize, std::vector<unsigned int>(boxsize));


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

std::ostream& operator<<(std::ostream& stream, const LinearDistributionTable& ldt) {
  // TODO
  return stream;
}

//-----------------------------------------------------------------------------

NonlinearStep::NonlinearStep() : bitsize_(0) {
}

NonlinearStep::NonlinearStep(std::function<BitVector(BitVector)> fun, unsigned bitsize) : ldt_(fun, bitsize), bitsize_(bitsize) {
}


bool NonlinearStep::Update(Mask& x, Mask& y) {
std::vector<unsigned int> inmasks, outmasks;
unsigned int inresult[2] = {0,0};
unsigned int outresult[2] = {0,0};
create_masks(inmasks,x);
create_masks(outmasks,y);

for (auto inmask : inmasks)
  for (auto outmask : outmasks) {
      inresult[0] |= (~inmask) & ldt_.ldt_bool[inmask][outmask];
      inresult[1] |= inmask & ldt_.ldt_bool[inmask][outmask];
      outresult[0] |= (~outmask) & ldt_.ldt_bool[inmask][outmask];
      outresult[1] |= outmask & ldt_.ldt_bool[inmask][outmask];
  }

  for (unsigned int i = 0; i < bitsize_; ++i) {
    x.bitmasks[bitsize_ - i - 1] = ((inresult[1] & (1<<i)) | ((inresult[0] & (1<<i))<<1)) >> i;
    y.bitmasks[bitsize_ - i - 1] = ((outresult[1] & (1<<i)) | ((outresult[0] & (1<<i))<<1)) >> i;
  }

  x.init_caremask();
  y.init_caremask();

  return true;
}

void NonlinearStep::create_masks(std::vector<unsigned int> &masks,
                                 Mask& reference, unsigned int pos,
                                 unsigned int current_mask) {
  if (pos < bitsize_) {
    switch (reference.bitmasks[pos]) {
      case BM_1:
        current_mask |= (1 << (bitsize_ - pos - 1));
        create_masks(masks, reference, ++pos, current_mask);
        break;
      case BM_0:
        create_masks(masks, reference, ++pos, current_mask);
        break;
      case BM_DUNNO:
        create_masks(masks, reference, ++pos, current_mask);
        current_mask |= (1 << (bitsize_ - (--pos) - 1));
        create_masks(masks, reference, ++pos, current_mask);
        break;
    }
  } else {
    masks.push_back(current_mask);
  }
}


std::ostream& operator<<(std::ostream& stream, const NonlinearStep& step) {
  // TODO
  return stream;
}
