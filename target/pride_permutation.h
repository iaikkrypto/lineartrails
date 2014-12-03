#ifndef PRIDEPERMUTATION_H_
#define PRIDEPERMUTATION_H_

#include <memory>
#include <array>

#include "pride.h"
#include "mask.h"
#include "permutation.h"

struct PridePermutation : public Permutation {
  PridePermutation(unsigned int rounds);
  PridePermutation(const PridePermutation& other);
  void touchall();
  PermPtr clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);
};


#endif
