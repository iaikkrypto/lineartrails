#ifndef ASCONPERMUTATION_H_
#define ASCONPERMUTATION_H_

#include <array>

#include "ascon.h"
#include "mask.h"

template <unsigned rounds>
struct AsconPermutation : public Permutation {
  AsconPermutation<rounds>& operator=(const AsconPermutation<rounds>& rhs);
  AsconPermutation();
  AsconPermutation(const AsconPermutation& other);
  virtual bool checkchar();
  bool guessbestsbox(SboxPos pos);
  virtual bool update();
  void touchall();
  void SboxStatus(std::vector<SboxPos>& active, std::vector<SboxPos>& inactive);
  void SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive);
  AsconPermutation<rounds>* clone() const;

  void PrintWithProbability();
  ProbabilityPair GetProbability();

  friend std::ostream& operator<<(std::ostream& stream, const AsconPermutation<rounds>& permutation) {
    int i = 0;
    for (const auto& state : permutation.state_masks_)
      stream << "State Mask " << ++i << std::endl << state << std::endl;
    return stream;
  }

  std::array<AsconSboxLayer,rounds> sbox_layers_;
  std::array<AsconLinearLayer, rounds> linear_layers_;
  std::array<AsconState, 2 * rounds + 1> state_masks_;
  bool toupdate_linear;
  bool toupdate_nonlinear;
};


#include "ascon_permutation.hpp"
#endif
