#ifndef ASCONPERMUTATION_H_
#define ASCONPERMUTATION_H_

#include <array>

#include "ascon.h"
#include "mask.h"
#include "permutation.h"

template <unsigned rounds>
struct AsconPermutation : public Permutation<rounds> {
  AsconPermutation<rounds>& operator=(const AsconPermutation<rounds>& rhs);
  AsconPermutation();
  AsconPermutation(const AsconPermutation& other);
  void touchall();
  AsconPermutation<rounds>* clone() const;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);


  friend std::ostream& operator<<(std::ostream& stream,
                                  const AsconPermutation<rounds>& permutation) {
    permutation->print(stream);
    return stream;
  }
};


#include "ascon_permutation.hpp"
#endif
