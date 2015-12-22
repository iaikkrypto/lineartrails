#ifndef PROST256PERMUTATION_H_
#define PROST256PERMUTATION_H_

#include <memory>
#include <vector>

#include "prost256.h"
#include "mask.h"
#include "permutation.h"


struct Prost256Permutation : public Permutation {
//  Prost256Permutation& operator=(const Prost256Permutation& rhs);
  Prost256Permutation(unsigned int rounds);
  Prost256Permutation(const Prost256Permutation& other);
  void touchall();
  PermPtr clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout,
                                    unsigned int offset = 0) override;

};


#endif
