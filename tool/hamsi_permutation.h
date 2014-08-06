#ifndef HAMSIPERMUTATION_H_
#define HAMSIPERMUTATION_H_

#include <array>

#include "hamsi.h"
#include "mask.h"
#include "permutation.h"

template <unsigned rounds>
struct HamsiPermutation : public Permutation<rounds> {
  HamsiPermutation<rounds>& operator=(const HamsiPermutation<rounds>& rhs);
  HamsiPermutation();
  HamsiPermutation(const HamsiPermutation& other);
  void touchall();
  HamsiPermutation<rounds>* clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);
  virtual bool setBit(BitMask cond, unsigned int bit);

  friend std::ostream& operator<<(std::ostream& stream,
                                  const HamsiPermutation<rounds>& permutation) {
    permutation->print(stream);
    return stream;
  }
};


#include "hamsi_permutation.hpp"
#endif
