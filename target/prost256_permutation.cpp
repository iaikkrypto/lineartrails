#include "prost256_permutation.h"


Prost256Permutation::Prost256Permutation(unsigned int rounds) : Permutation(rounds) {
  for(unsigned int i = 0; i< 2*rounds +1; ++i) {
    this->state_masks_[i].reset(new Prost256State);
    this->saved_state_masks_[i].reset(new Prost256State);
  }
  for (unsigned int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(new Prost256SboxLayer);
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    if((i & 1) == 1)
      this->linear_layers_[i].reset(new Prost256LinearLayer<1>);
    else
      this->linear_layers_[i].reset(new Prost256LinearLayer<0>);
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
    this->saved_sbox_layers_[i].reset(new Prost256SboxLayer);
    this->saved_sbox_layers_[i]->SetMasks(this->saved_state_masks_[2*i].get(), this->saved_state_masks_[2*i + 1].get());
    if ((i & 1) == 1)
      this->saved_linear_layers_[i].reset(new Prost256LinearLayer<1>);
    else
      this->saved_linear_layers_[i].reset(new Prost256LinearLayer<0>);
    this->saved_linear_layers_[i]->SetMasks(this->saved_state_masks_[2*i + 1].get(), this->saved_state_masks_[2*i + 2].get());
  }
  touchall();
}


Prost256Permutation::Prost256Permutation(const Prost256Permutation& other) : Permutation(other) {
  for(unsigned int i = 0; i< 2*rounds_ +1; ++i) {
    this->saved_state_masks_[i].reset(new Prost256State);
  }
  for (unsigned int i = 0; i < rounds_; ++i) {
    this->saved_sbox_layers_[i].reset(new Prost256SboxLayer);
    this->saved_sbox_layers_[i]->SetMasks(this->saved_state_masks_[2*i].get(), this->saved_state_masks_[2*i + 1].get());
    if ((i & 1) == 1)
      this->saved_linear_layers_[i].reset(new Prost256LinearLayer<1>);
    else
      this->saved_linear_layers_[i].reset(new Prost256LinearLayer<0>);
    this->saved_linear_layers_[i]->SetMasks(this->saved_state_masks_[2*i + 1].get(), this->saved_state_masks_[2*i + 2].get());
  }
}




//Prost256Permutation& Prost256Permutation::operator=(const Prost256Permutation& rhs){
//  for(unsigned int i = 0; i< 2*rounds_ +1; ++i){
//    this->state_masks_[i].reset(rhs.state_masks_[i]->clone());
//  }
//  this->toupdate_linear = rhs.toupdate_linear;
// this->toupdate_nonlinear = rhs.toupdate_nonlinear;
// for (unsigned int i = 0; i < rounds_; ++i) {
//   this->linear_layers_[i].reset(new Prost256LinearLayer);
//   this->linear_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
//   this->sbox_layers_[i].reset(new Prost256SboxLayer);
//   this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
// }
// return *this;
//}



Permutation::PermPtr Prost256Permutation::clone() const {
  return PermPtr(new Prost256Permutation(*this));
}


void Prost256Permutation::PrintWithProbability(std::ostream& stream,
                                               unsigned int offset) {
  Permutation::PrintWithProbability(stream, 0);
}


void Prost256Permutation::touchall() {
  Permutation::touchall();
}

