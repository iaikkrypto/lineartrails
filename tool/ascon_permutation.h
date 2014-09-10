#ifndef ASCONPERMUTATION_H_
#define ASCONPERMUTATION_H_

#include <array>

#include "ascon.h"
#include "mask.h"
#include "permutation.h"


struct AsconPermutation : public Permutation {
  AsconPermutation& operator=(const AsconPermutation& rhs);
  AsconPermutation(unsigned int rounds);
  AsconPermutation(const AsconPermutation& other);
  void touchall();
  AsconPermutation* clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);
  virtual bool setBit(BitMask cond, unsigned int bit);

};


#endif
