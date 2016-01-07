#ifndef ICEPOLEPERMUTATION_H_
#define ICEPOLEPERMUTATION_H_

#include <memory>
#include <vector>

#include "icepole.h"
#include "mask.h"
#include "permutation.h"


struct IcepolePermutation : public Permutation {
  IcepolePermutation& operator=(const IcepolePermutation& rhs);
  IcepolePermutation(unsigned int rounds);
  IcepolePermutation(const IcepolePermutation& other);
  virtual void set(Permutation* perm);
  bool update();
  void touchall();
  PermPtr clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout,
                                    unsigned int offset = 0) override;

};


#endif
