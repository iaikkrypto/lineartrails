#include "hamsi_permutation.h"


HamsiPermutation::HamsiPermutation(unsigned int rounds) : Permutation(rounds) {
  for(unsigned int i = 0; i< 2*rounds +1; ++i){
      this->state_masks_[i].reset(new HamsiState);
    }
  for (unsigned int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(new HamsiSboxLayer);
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(new HamsiLinearLayer);
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
  }
  touchall();
}


HamsiPermutation::HamsiPermutation(const HamsiPermutation& other) : Permutation(other) {
}


HamsiPermutation& HamsiPermutation::operator=(const HamsiPermutation& rhs){
  for(unsigned int i = 0; i< 2*rounds_ +1; ++i){
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


HamsiPermutation* HamsiPermutation::clone() const{
  return new HamsiPermutation(*this);
}

void HamsiPermutation::PrintWithProbability(std::ostream& stream, int offset) {
  Permutation::PrintWithProbability(stream, 0);
}


void HamsiPermutation::touchall() {
  Permutation::touchall();
}


bool HamsiPermutation::setBit(BitMask cond, unsigned int bit){
  return Permutation::setBit(cond,bit,16,32);
}


