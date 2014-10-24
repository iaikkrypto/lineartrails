#ifndef KECCAK1600PERMUTATION_H_
#define KECCAK1600PERMUTATION_H_

#include <vector>

#include "keccak1600.h"
#include "mask.h"
#include "permutation.h"


struct Keccak1600Permutation : public Permutation {
  Keccak1600Permutation& operator=(const Keccak1600Permutation& rhs);
  Keccak1600Permutation(unsigned int rounds);
  Keccak1600Permutation(const Keccak1600Permutation& other);
  virtual void set(Permutation* perm);
  bool update();
  void touchall();
  Keccak1600Permutation* clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);

};


#endif
