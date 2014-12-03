#include "pride_permutation.h"

PridePermutation::PridePermutation(unsigned int rounds) : Permutation(rounds) {
  for(unsigned int i = 0; i< 2*rounds +1; ++i){
      this->state_masks_[i].reset(new PrideState);
      this->saved_state_masks_[i].reset(new PrideState);
    }
  for (unsigned int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(new PrideSboxLayer);
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(new PrideLinearLayer);
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
    this->saved_sbox_layers_[i].reset(new PrideSboxLayer);
    this->saved_sbox_layers_[i]->SetMasks(this->saved_state_masks_[2*i].get(), this->saved_state_masks_[2*i + 1].get());
    this->saved_linear_layers_[i].reset(new PrideLinearLayer);
    this->saved_linear_layers_[i]->SetMasks(this->saved_state_masks_[2*i + 1].get(), this->saved_state_masks_[2*i + 2].get());
  }
  touchall();
}


PridePermutation::PridePermutation(const PridePermutation& other) : Permutation(other) {
  for(unsigned int i = 0; i< 2*rounds_ +1; ++i){
      this->saved_state_masks_[i].reset(new PrideState);
    }
  for (unsigned int i = 0; i < rounds_; ++i) {
    this->saved_sbox_layers_[i].reset(new PrideSboxLayer);
    this->saved_sbox_layers_[i]->SetMasks(this->saved_state_masks_[2*i].get(), this->saved_state_masks_[2*i + 1].get());
    this->saved_linear_layers_[i].reset(new PrideLinearLayer);
    this->saved_linear_layers_[i]->SetMasks(this->saved_state_masks_[2*i + 1].get(), this->saved_state_masks_[2*i + 2].get());
  }
}


//PridePermutation& PridePermutation::operator=(const PridePermutation& rhs){
//  for(unsigned int i = 0; i< 2*rounds_ +1; ++i){
//    this->state_masks_[i].reset(rhs.state_masks_[i]->clone());
//  }
//  this->toupdate_linear = rhs.toupdate_linear;
// this->toupdate_nonlinear = rhs.toupdate_nonlinear;
// for (unsigned int i = 0; i < rounds_; ++i) {
//   this->sbox_layers_[i].reset(rhs.sbox_layers_[i]->clone());
//   this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
//   this->linear_layers_[i].reset(rhs.linear_layers_[i]->clone());
//   this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
// }
// return *this;
//}


Permutation::PermPtr PridePermutation::clone() const {
  return PermPtr(new PridePermutation(*this));
}


void PridePermutation::PrintWithProbability(std::ostream& stream, int offset) {
  Permutation::PrintWithProbability(stream, 0);
}


void PridePermutation::touchall() {
  Permutation::touchall();
}

