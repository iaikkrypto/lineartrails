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
AsconPermutation<rounds>::AsconPermutation(const AsconPermutation& other) : Permutation<rounds>(other) {
}

template <unsigned rounds>
AsconPermutation<rounds>& AsconPermutation<rounds>::operator=(const AsconPermutation<rounds>& rhs){
  for(int i = 0; i< 2*rounds +1; ++i){
    this->state_masks_[i].reset(rhs.state_masks_[i]->clone());
  }
  this->toupdate_linear = rhs.toupdate_linear;
 this->toupdate_nonlinear = rhs.toupdate_nonlinear;
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

template <unsigned rounds>
bool AsconPermutation<rounds>::setBit(BitMask cond, unsigned int bit){
  return Permutation<rounds>::setBit(cond,bit,5,64);
}


