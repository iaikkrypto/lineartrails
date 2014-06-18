//-----------------------------------------------------------------------------

template <unsigned rounds>
AsconPermutation<rounds>::AsconPermutation() {
  for(int i = 0; i< 2*rounds +1; ++i){
      this->state_masks_[i].reset(new AsconState);
    }
  for (int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(new AsconSboxLayer);
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(new AsconLinearLayer);
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
  }
  touchall();
}

template <unsigned rounds>
AsconPermutation<rounds>::AsconPermutation(const AsconPermutation& other) {
  for(int i = 0; i< 2*rounds +1; ++i){
    this->state_masks_[i].reset(other.state_masks_[i]->clone());
  }

  for (int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(other.sbox_layers_[i]->clone());
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(other.linear_layers_[i]->clone());
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
  }
//  state_masks_ = other.state_masks_;
  this->toupdate_linear = other.toupdate_linear;
  this->toupdate_nonlinear = other.toupdate_nonlinear;
//  this->sbox_layers_ = other.sbox_layers_;
//  this->linear_layers_ = other.linear_layers_;

}

template <unsigned rounds>
AsconPermutation<rounds>& AsconPermutation<rounds>::operator=(const AsconPermutation<rounds>& rhs){
  for(int i = 0; i< 2*rounds +1; ++i){
    this->state_masks_[i].reset(rhs.state_masks_[i]->clone());
  }
// state_masks_ = rhs.state_maskss_;
  this->toupdate_linear = rhs.toupdate_linear;
 this->toupdate_nonlinear = rhs.toupdate_nonlinear;
// this->sbox_layers_ = rhs.sbox_layers_;
// this->linear_layers_ = rhs.linear_layers_;
 for (int i = 0; i < rounds; ++i) {
   this->sbox_layers_[i].reset(rhs.sbox_layers_[i]->clone());
   this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
   this->linear_layers_[i].reset(rhs.linear_layers_[i]->clone());
   this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
 }
 return *this;
}

template <unsigned rounds>
AsconPermutation<rounds>* AsconPermutation<rounds>::clone() const{
  return new AsconPermutation(*this);
}

template<unsigned rounds>
void AsconPermutation<rounds>::PrintWithProbability(std::ostream& stream, int offset) {
  Permutation<rounds>::PrintWithProbability(stream, 0);
}

template <unsigned rounds>
void AsconPermutation<rounds>::touchall() {
  Permutation<rounds>::touchall();
}


