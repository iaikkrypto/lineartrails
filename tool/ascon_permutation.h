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
  virtual bool checkchar();
  virtual bool guessbestsbox(SboxPos pos);
  virtual bool guessbestsbox(SboxPos pos, int num_alternatives);
  virtual bool update();
  void touchall();
  virtual void SboxStatus(std::vector<SboxPos>& active, std::vector<SboxPos>& inactive);
  virtual void SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive);
  AsconPermutation<rounds>* clone() const;

  void PrintWithProbability();
  ProbabilityPair GetProbability();

  friend std::ostream& operator<<(std::ostream& stream, const AsconPermutation<rounds>& permutation) {
    int i = 0;
    for (const auto& state : permutation.state_masks_)
      stream << "State Mask " << ++i << std::endl << state << std::endl;
    return stream;
  }

  std::array<AsconState, 2 * rounds + 1> state_masks_;
  bool toupdate_linear;
  bool toupdate_nonlinear;
};


#include "ascon_permutation.hpp"
#endif
