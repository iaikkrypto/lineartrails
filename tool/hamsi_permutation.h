#ifndef HAMSIPERMUTATION_H_
#define HAMSIPERMUTATION_H_

#include <vector>

#include "hamsi.h"
#include "mask.h"
#include "permutation.h"

struct HamsiPermutation : public Permutation {
  HamsiPermutation& operator=(const HamsiPermutation& rhs);
  HamsiPermutation(unsigned int rounds);
  HamsiPermutation(const HamsiPermutation& other);
  void touchall();
  HamsiPermutation* clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);
  virtual bool setBit(BitMask cond, unsigned int bit);
};

struct HamsiCompression : public Permutation {
  HamsiCompression& operator=(const HamsiCompression& rhs);
  HamsiCompression(unsigned int rounds);
  HamsiCompression(const HamsiCompression& other);
  void touchall();
  HamsiCompression* clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);
  virtual bool setBit(BitMask cond, unsigned int bit);
  void set(Permutation* perm);
};

#endif
