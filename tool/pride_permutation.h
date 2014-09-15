#ifndef PRIDEPERMUTATION_H_
#define PRIDEPERMUTATION_H_

#include <array>

#include "pride.h"
#include "mask.h"
#include "permutation.h"

struct PridePermutation : public Permutation {
  PridePermutation& operator=(const PridePermutation& rhs);
  PridePermutation(unsigned int rounds);
  PridePermutation(const PridePermutation& other);
  void touchall();
  PridePermutation* clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);
};


#endif
