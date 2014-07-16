template <unsigned rounds>
Permutation<rounds>::Permutation(const Permutation& other) {
  for(int i = 0; i< 2*rounds +1; ++i){
    this->state_masks_[i].reset(other.state_masks_[i]->clone());
  }

  for (int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(other.sbox_layers_[i]->clone());
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(other.linear_layers_[i]->clone());
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
  }
  this->toupdate_linear = other.toupdate_linear;
  this->toupdate_nonlinear = other.toupdate_nonlinear;
}


template <unsigned rounds>
void Permutation<rounds>::SboxStatus(std::vector<SboxPos>& active,
                                  std::vector<SboxPos>& inactive) {
  active.clear();
  inactive.clear();

  for (size_t layer = 0; layer < rounds; ++layer)
    for (int pos = 0; pos < this->sbox_layers_[layer]->GetNumLayer(); ++pos)
      if (this->sbox_layers_[layer]->SboxGuessable(pos)) {
        if (this->sbox_layers_[layer]->SboxActive(pos))
          active.emplace_back(layer, pos);
        else
          inactive.emplace_back(layer, pos);
      }
}
template <unsigned rounds>
void Permutation<rounds>::SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive){
  active.clear();
  inactive.clear();
  active.resize(rounds);
  inactive.resize(rounds);

  for (size_t layer = 0; layer < this->sbox_layers_.size(); ++layer)
    for (int pos = 0; pos < this->sbox_layers_[layer]->GetNumLayer(); ++pos)
      if (this->sbox_layers_[layer]->SboxGuessable(pos)) {
        if (this->sbox_layers_[layer]->SboxActive(pos))
          active[layer].emplace_back(layer, pos);
        else
          inactive[layer].emplace_back(layer, pos);
      }
}

template <unsigned rounds>
bool Permutation<rounds>::isActive(SboxPos pos){

return this->sbox_layers_[pos.layer_]->SboxActive(pos.pos_);
}


template <unsigned rounds>
bool Permutation<rounds>::guessbestsbox(SboxPos pos, std::function<int(int, int, int)> rating) {

  this->sbox_layers_[pos.layer_]->GuessBox(UpdatePos(0, 0, pos.pos_, 0), rating);

  this->toupdate_linear = true;
    return update();
}

template<unsigned rounds>
bool Permutation<rounds>::guessbestsbox(SboxPos pos, std::function<int(int, int, int)> rating,
                                             int num_alternatives) {
  bool update_works = false;
  std::unique_ptr<Permutation<rounds>> temp;
  temp.reset(this->clone());

  for (int i = 0; i < num_alternatives; ++i) {
    int total_alternatives = this->sbox_layers_[pos.layer_]->GuessBox(
        UpdatePos(0, 0, pos.pos_, 0), rating, i);
    num_alternatives =
        total_alternatives < num_alternatives ?
            total_alternatives : num_alternatives;
    this->toupdate_linear = true;
    update_works = update();
    if (update_works)
      return update_works;
    this->set(temp.get());
  }
  return false;
}

template<unsigned rounds>
bool Permutation<rounds>::guessbestsboxrandom(SboxPos pos, std::function<int(int, int, int)> rating,
                                             int num_alternatives) {
  bool update_works = false;
  std::unique_ptr<Permutation<rounds>> temp;
  temp.reset(this->clone());

  for (int i = 0; i < num_alternatives; ++i) {
    int total_alternatives = 0xffff;
    if(i)
    total_alternatives = this->sbox_layers_[pos.layer_]->GuessBox(
        UpdatePos(0, 0, pos.pos_, 0), rating, i);
    else
      this->sbox_layers_[pos.layer_]->GuessBoxRandom(
              UpdatePos(0, 0, pos.pos_, 0), rating);
    num_alternatives =
        total_alternatives < num_alternatives ?
            total_alternatives : num_alternatives;
    this->toupdate_linear = true;
    update_works = update();
    if (update_works)
      return update_works;
    this->set(temp.get());
  }
  return false;
}

template<unsigned rounds>
void Permutation<rounds>::set(Permutation<rounds>* perm){
  for(int i = 0; i< 2*rounds +1; ++i){
    this->state_masks_[i].reset(perm->state_masks_[i]->clone());
  }

  for (int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(perm->sbox_layers_[i]->clone());
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(perm->linear_layers_[i]->clone());
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
  }
  this->toupdate_linear = perm->toupdate_linear;
  this->toupdate_nonlinear = perm->toupdate_nonlinear;
}

template <unsigned rounds>
bool Permutation<rounds>::update() {
  //TODO: Better update
  bool correct = true;
  std::unique_ptr<StateMask> tempin, tempout;
  while (this->toupdate_linear == true || this->toupdate_nonlinear == true) {
    if (this->toupdate_nonlinear == true) {
      this->toupdate_nonlinear = false;
      for (size_t layer = 0; layer < rounds; ++layer) {
        tempin.reset(this->sbox_layers_[layer]->in->clone());
        tempout.reset(this->sbox_layers_[layer]->out->clone());
        for (int i = 0; i < this->sbox_layers_[layer]->GetNumLayer(); ++i)
          correct &= this->sbox_layers_[layer]->Update(UpdatePos(i, i, i, 1));
        if (tempin->diff(*( this->sbox_layers_[layer]->in)).size() != 0
            || tempout->diff(*( this->sbox_layers_[layer]->out)).size() != 0)
          this->toupdate_linear = true;
      }
    }
    if (this->toupdate_linear == true) {
      this->toupdate_linear = false;
      for (size_t layer = 0; layer < rounds; ++layer) {
        tempin.reset(this->linear_layers_[layer]->in->clone());
        tempout.reset(this->linear_layers_[layer]->out->clone());
        for (int i = 0; i < this->linear_layers_[layer]->GetNumLayer(); ++i)
          correct &= this->linear_layers_[layer]->Update(UpdatePos(i, i, i, 1));
        if (tempin->diff(*( this->linear_layers_[layer]->in)).size() != 0
            || tempout->diff(*( this->linear_layers_[layer]->out)).size() != 0)
          this->toupdate_nonlinear = true;
      }
    }
  }
  return correct;
}

template<unsigned rounds>
ProbabilityPair Permutation<rounds>::GetProbability() {
  ProbabilityPair prob { 1, 0.0 };
  ProbabilityPair temp_prob;

  for (auto& layer : this->sbox_layers_) {
    temp_prob = layer->GetProbability();
    prob.sign *= temp_prob.sign;
    prob.bias += temp_prob.bias;
  }

  prob.bias += rounds - 1;

  return prob;
}

template<unsigned rounds>
unsigned int Permutation<rounds>::GetActiveSboxes() {
  unsigned int active_sboxes_layer = 0;

  for (auto& layer : this->sbox_layers_) {
    for (int j = 0; j < layer->GetNumLayer(); ++j)
            active_sboxes_layer += layer->SboxActive(j);
  }

  return active_sboxes_layer;
}

template <unsigned rounds>
bool Permutation<rounds>::checkchar(std::ostream& stream) {
  bool correct;
  stream << "Characteristic before propagation" << std::endl;
  this->print(stream);
  correct = this->update();
  stream << "Characteristic after propagation" << std::endl;
  this->print(stream);
  return correct;
}

template <unsigned rounds>
void Permutation<rounds>::print(std::ostream& stream) {
  int i = 0;
  for (const auto& state : state_masks_) {
    stream << "State Mask " << ++i << std::endl;
    state->print(stream);
    stream << std::endl;
  }
}

template<unsigned rounds>
void Permutation<rounds>::PrintWithProbability(std::ostream& stream, int offset) {
  ProbabilityPair prob { 1, 0.0 };
  ProbabilityPair temp_prob;
  int active_sboxes = 0;
  for (int i = 0; i <= 2 * rounds; ++i) {
    stream << "State Mask " << i + 1;
    if (i % 2 == offset && i < 2 * rounds) {
      temp_prob = this->sbox_layers_[i / 2]->GetProbability();
      int active_sboxes_layer = 0;
      for (int j = 0; j < this->sbox_layers_[i / 2]->GetNumLayer(); ++j)
        active_sboxes_layer += (int) this->sbox_layers_[i / 2]->SboxActive(j);
      active_sboxes += active_sboxes_layer;
      prob.sign *= temp_prob.sign;
      prob.bias += temp_prob.bias;
      stream << " sign: " << (int) temp_prob.sign << " bias: "
                << temp_prob.bias << " active sboxes: " << active_sboxes_layer;
    }
    stream << std::endl;
    this->state_masks_[i]->print(stream);
    stream << std::endl;
  }
  prob.bias += rounds - 1;
  stream << "Total: sign: " << (int) prob.sign << " bias: " << prob.bias
      << " active sboxes: " << active_sboxes << std::endl << std::endl;

  stream << "----------------------------------------------------------------" << std::endl;
}

template <unsigned rounds>
void Permutation<rounds>::touchall() {
  this->toupdate_linear = true;
  this->toupdate_nonlinear = true;
}

template <unsigned rounds>
bool Permutation<rounds>::setBit(BitMask cond, unsigned int bit, unsigned char num_words, unsigned char num_bits){
unsigned int state = bit / (num_words*num_bits);
if (state > 2*rounds +1)
  return false;

bit %= (num_words*num_bits);

unsigned int word = bit / num_bits;

bit %= (num_bits);

bit = num_bits - bit - 1;

state_masks_[state]->SetBit(cond, word, bit);

  return true;
}


