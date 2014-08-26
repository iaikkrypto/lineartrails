#ifndef PRIDEPERMUTATION_H_
#define PRIDEPERMUTATION_H_

#include <array>

#include "pride.h"
#include "mask.h"
#include "permutation.h"

template <unsigned rounds>
struct PridePermutation : public Permutation<rounds> {
  PridePermutation<rounds>& operator=(const PridePermutation<rounds>& rhs);
  PridePermutation();
  PridePermutation(const PridePermutation& other);
  void touchall();
  PridePermutation<rounds>* clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);
  virtual bool setBit(BitMask cond, unsigned int bit);

  friend std::ostream& operator<<(std::ostream& stream,
                                  const PridePermutation<rounds>& permutation) {
    permutation->print(stream);
    return stream;
  }
};


#include "pride_permutation.hpp"
#endif
