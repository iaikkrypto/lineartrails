/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
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
std::vector<unsigned int> NonlinearStep<bitsize>::inmasks_;
template <unsigned bitsize>
std::vector<unsigned int>NonlinearStep<bitsize>::outmasks_;
template <unsigned bitsize>
std::multimap<int, std::pair<unsigned int, unsigned int>, std::greater<int>> NonlinearStep<bitsize>::valid_masks_;

template <unsigned bitsize>
NonlinearStep<bitsize>::NonlinearStep(std::function<BitVector(BitVector)> fun) {
  Initialize(fun);
}

template <unsigned bitsize>
void NonlinearStep<bitsize>::Initialize(std::function<BitVector(BitVector)> fun) {
  is_active_ = false;
  is_guessable_ = true;
  has_to_be_active_ = false;
  ldt_.reset(new LinearDistributionTable<bitsize>(fun));
  inmasks_.reserve(bitsize);
  outmasks_.reserve(bitsize);
}

template <unsigned bitsize>
void NonlinearStep<bitsize>::Initialize(std::shared_ptr<LinearDistributionTable<bitsize>> ldt) {
  is_active_ = false;
  is_guessable_ = true;
  has_to_be_active_ = false;
  ldt_ = ldt;
  inmasks_.reserve(bitsize*bitsize);
  outmasks_.reserve(bitsize*bitsize);
}

template <unsigned bitsize>
bool NonlinearStep<bitsize>::Update(Mask& x, Mask& y) {
  std::vector<unsigned int> inmasks, outmasks;
  unsigned int inresult[2] = { 0, 0 };
  unsigned int outresult[2] = { 0, 0 };
  create_masks(inmasks, x);
  create_masks(outmasks, y);
  for (const auto& inmask : inmasks)
    for (const auto& outmask : outmasks) {
      inresult[0] |= (~inmask) & ldt_->ldt_bool[inmask][outmask];
      inresult[1] |= inmask & ldt_->ldt_bool[inmask][outmask];
      outresult[0] |= (~outmask) & ldt_->ldt_bool[inmask][outmask];
      outresult[1] |= outmask & ldt_->ldt_bool[inmask][outmask];
    }

  for (unsigned int i = 0; i < bitsize; ++i) {
    x.bitmasks[i] = ((inresult[1] & (1 << i))
        | ((inresult[0] & (1 << i)) << 1)) >> i;
    y.bitmasks[i] = ((outresult[1] & (1 << i))
        | ((outresult[0] & (1 << i)) << 1)) >> i;
  }

  x.reinit_caremask();
  y.reinit_caremask();

  if ((inresult[0] | inresult[1]) == 0 || (outresult[0] | outresult[1]) == 0) {
    for (unsigned int i = 0; i < bitsize; ++i) {
      x.bitmasks[i] = BM_CONTRA;
      y.bitmasks[i] = BM_CONTRA;
    }
    return false;
  }

  if ( (((~x.caremask.canbe1) | (~x.caremask.care)) & (~0ULL >> (64 - bitsize)))  == (~0ULL >> (64 - bitsize))
      && (((~y.caremask.canbe1) | (~y.caremask.care)) & (~0ULL >> (64 - bitsize)))  == (~0ULL >> (64 - bitsize)))
    is_active_ = false;
  else
    is_active_ = true;

  is_guessable_ = false;
  for(unsigned int i = 0; i < bitsize; ++i)
    if(x.bitmasks[i] == BM_DUNNO || y.bitmasks[i] == BM_DUNNO){
      is_guessable_ = true;
      break;
    }

  //FIXME: hack
  if(has_to_be_active_ == true && is_active_ == false && is_guessable_ == false)
    return false;

  return true;
}

template<unsigned bitsize>
bool NonlinearStep<bitsize>::Update(
    Mask& x, Mask& y,
    Cache<unsigned long long, NonlinearStepUpdateInfo>* box_cache) {
  NonlinearStepUpdateInfo stepdata;
  x.reinit_caremask();
  y.reinit_caremask();
  unsigned long long key = getKey(x, y);

  if (box_cache->find(key, stepdata)) {
    is_active_ = stepdata.is_active_;
    is_guessable_ = stepdata.is_guessable_;
    x.bitmasks = stepdata.inmask_;
    y.bitmasks = stepdata.outmask_;
    x.reinit_caremask();
    y.reinit_caremask();
    //FIXME: hack
    if (has_to_be_active_ == true && is_active_ == false
        && is_guessable_ == false)
      return false;
    return true;
  }

  if (Update(x, y)) {
    stepdata.is_active_ = is_active_;
    stepdata.is_guessable_ = is_guessable_;
    stepdata.inmask_ = x.bitmasks;
    stepdata.outmask_ = y.bitmasks;
    x.reinit_caremask();
    y.reinit_caremask();
    box_cache->insert(key, stepdata);
    return true;
  }

  return false;
}

template<unsigned bitsize>
unsigned long long NonlinearStep<bitsize>::getKey(Mask& in, Mask& out) {
  return ((in.caremask.canbe1) << (3 * bitsize))
      | ((in.caremask.care) << (2 * bitsize))
      | ((out.caremask.canbe1) << bitsize)
      | ((out.caremask.care));
}

template <unsigned bitsize>
double NonlinearStep<bitsize>::GetProbability(Mask& x, Mask& y) {
  std::vector<unsigned int> inmasks, outmasks;
  create_masks(inmasks, x);
  create_masks(outmasks, y);

  //TODO: Calculate some probability if undefined bits are present
  if(inmasks.size() > 1)
    return -10000000;
 if(ldt_->ldt[inmasks[0]][outmasks[0]] == 0)
    return -1;
  return  (double) (std::log2((double)std::abs(
      ldt_->ldt[inmasks[0]][outmasks[0]])) - bitsize);

}

template <unsigned bitsize>
NonlinearStep<bitsize>& NonlinearStep<bitsize>::operator=(const NonlinearStep<bitsize>& rhs){
  ldt_ = rhs.ldt_;
  is_active_ = rhs.is_active_;
  has_to_be_active_ = rhs.has_to_be_active_;
  is_guessable_ = rhs.is_guessable_;
  return *this;
}

template <unsigned bitsize>
void NonlinearStep<bitsize>::TakeBestBox(Mask& x, Mask& y, std::function<int(int, int, int)> rating) {
  inmasks_.clear();
  outmasks_.clear();
  create_masks(inmasks_, x);
  create_masks(outmasks_, y);

    int best_rate = 0;
    unsigned int best_inmask = (unsigned int) has_to_be_active_;
    unsigned int best_outmask = 0;

  for (const auto& inmask : inmasks_)
    for (const auto& outmask : outmasks_) {
      if(ldt_->ldt[inmask][outmask] != 0)
       if(best_rate < rating(ldt_->ldt[inmask][outmask], __builtin_popcount (inmask), __builtin_popcount (outmask))){
         best_rate = rating(ldt_->ldt[inmask][outmask], __builtin_popcount (inmask), __builtin_popcount (outmask));
         best_inmask = inmask;
         best_outmask = outmask;
       }
    }

  for (unsigned int i = 0; i < bitsize; ++i) {
    x.bitmasks[i] = (((best_inmask  >> i)&1) == 1 ? BM_1 : BM_0);
    y.bitmasks[i] = (((best_outmask  >> i)&1) == 1 ? BM_1 : BM_0);
  }

  if(best_inmask)
    is_active_ = true;
  else
    is_active_ = false;

  is_guessable_ = false;

  x.reinit_caremask();
  y.reinit_caremask();

}

template<unsigned bitsize>
int NonlinearStep<bitsize>::TakeBestBox(
    Mask& x, Mask& y, std::function<int(int, int, int)> rating, int pos) {
  inmasks_.clear();
  outmasks_.clear();
  create_masks(inmasks_, x);
  create_masks(outmasks_, y);
  valid_masks_.clear();

  //FIXME: not nice, just to be able to set boxes active
  if (has_to_be_active_ == true){
    for (auto it = inmasks_.begin(); it != inmasks_.end(); ++it)
      if (*it == 0) {
        inmasks_.erase(it);
        //FIXME: even worse
        if(pos > 0)
          --pos;
        break;
      }
  }

  for (const auto& inmask : inmasks_)
    for (const auto& outmask : outmasks_) {
      if (ldt_->ldt[inmask][outmask] != 0) {
        valid_masks_.insert(
            std::pair<int, std::pair<unsigned int, unsigned int>>(
                rating(ldt_->ldt[inmask][outmask], __builtin_popcount(inmask),
                       __builtin_popcount(outmask)),
                std::pair<unsigned int, unsigned int>(inmask, outmask)));
      }
    }

  assert(pos < (int)valid_masks_.size());

  for (unsigned int i = 0; i < bitsize; ++i) {
    x.bitmasks[i] = (
        ((std::next(valid_masks_.begin(), pos)->second.first >> i) & 1) == 1 ?
            BM_1 : BM_0);
    y.bitmasks[i] = (
        ((std::next(valid_masks_.begin(), pos)->second.second >> i) & 1) == 1 ?
            BM_1 : BM_0);
  }

  if (std::next(valid_masks_.begin(), pos)->second.first)
    is_active_ = true;
  else
    is_active_ = false;

  is_guessable_ = false;

  x.reinit_caremask();
  y.reinit_caremask();

  return valid_masks_.size();
}

template<unsigned bitsize>
void NonlinearStep<bitsize>::TakeBestBoxRandom(
    Mask& x, Mask& y, std::function<int(int, int, int)> rating) {
  inmasks_.clear();
  outmasks_.clear();
  create_masks(inmasks_, x);
  create_masks(outmasks_, y);
  valid_masks_.clear();

  //FIXME: not nice, just to be able to set boxes active
  if (has_to_be_active_ == true)
    for (auto it = inmasks_.begin(); it != inmasks_.end(); ++it)
      if (*it == 0) {
        inmasks_.erase(it);
        break;
      }

  for (const auto& inmask : inmasks_)
    for (const auto& outmask : outmasks_) {
      if (ldt_->ldt[inmask][outmask] != 0) {
        valid_masks_.insert(
            std::pair<int, std::pair<unsigned int, unsigned int>>(
                rating(ldt_->ldt[inmask][outmask], __builtin_popcount(inmask),
                       __builtin_popcount(outmask)),
                std::pair<unsigned int, unsigned int>(inmask, outmask)));
      }
    }

  assert(valid_masks_.rbegin() != valid_masks_.rend());
  auto iterators = valid_masks_.equal_range(valid_masks_.begin()->first);
  std::mt19937 generator(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> guessbox(0, std::distance(iterators.first, iterators.second) - 1);
  int box = guessbox(generator);

  for (unsigned int i = 0; i < bitsize; ++i) {
    x.bitmasks[i] = (
        ((std::next(valid_masks_.begin(), box)->second.first >> i) & 1) == 1 ?
            BM_1 : BM_0);
    y.bitmasks[i] = (
        ((std::next(valid_masks_.begin(), box)->second.second >> i) & 1) == 1 ?
            BM_1 : BM_0);
  }

  if (std::next(valid_masks_.begin(), box)->second.first)
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
  if (pos < (int)bitsize) {
    switch (reference.bitmasks[pos]) {
      case BM_1:
        current_mask |= (1 << pos);
        create_masks(masks, reference, ++pos, current_mask);
        break;
      case BM_0:
        create_masks(masks, reference, ++pos, current_mask);
        break;
      case BM_DUNNO:
        create_masks(masks, reference, ++pos, current_mask);
        current_mask |= (1 << ((--pos)));
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

