#include <memory>
#include "ascon_permutation.h"


AsconPermutation::AsconPermutation(unsigned int rounds) : Permutation(rounds) {
  for(unsigned int i = 0; i< 2*rounds +1; ++i) {
    this->state_masks_[i].reset(new AsconState);
  }
  for (unsigned int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(new AsconSboxLayer);
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(new AsconLinearLayer);
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
  }
  touchall();
}


AsconPermutation::AsconPermutation(const AsconPermutation& other) : Permutation(other) {
}


AsconPermutation& AsconPermutation::operator=(const AsconPermutation& rhs) {
  for(unsigned int i = 0; i< 2*rounds_ +1; ++i) {
    this->state_masks_[i].reset(rhs.state_masks_[i]->clone());
  }
  this->toupdate_linear = rhs.toupdate_linear;
  this->toupdate_nonlinear = rhs.toupdate_nonlinear;
  for (unsigned int i = 0; i < rounds_; ++i) {
    this->sbox_layers_[i].reset(rhs.sbox_layers_[i]->clone());
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(rhs.linear_layers_[i]->clone());
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
  }
  return *this;
}


Permutation::PermPtr AsconPermutation::clone() const {
  return PermPtr(new AsconPermutation(*this));
}


void AsconPermutation::PrintWithProbability(std::ostream& stream, int offset) {
  Permutation::PrintWithProbability(stream, 0);
}


void AsconPermutation::touchall() {
  Permutation::touchall();
}

