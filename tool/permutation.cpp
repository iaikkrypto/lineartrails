#include "permutation.h"

Permutation::Permutation(unsigned int rounds)
    : toupdate_linear(true),
      toupdate_nonlinear(true),
      rounds_(rounds) {
  state_masks_.resize(2 * rounds_ + 1);
  sbox_layers_.resize(rounds_);
  linear_layers_.resize(rounds_);
}

Permutation::Permutation(const Permutation& other)
    : Permutation(other.rounds_) {
  for (unsigned int i = 0; i < 2 * rounds_ + 1; ++i) {
    this->state_masks_[i].reset(other.state_masks_[i]->clone());
  }

  for (unsigned int i = 0; i < rounds_; ++i) {
    this->sbox_layers_[i].reset(other.sbox_layers_[i]->clone());
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2 * i].get(),
                                    this->state_masks_[2 * i + 1].get());
    this->linear_layers_[i].reset(other.linear_layers_[i]->clone());
    this->linear_layers_[i]->SetMasks(this->state_masks_[2 * i + 1].get(),
                                      this->state_masks_[2 * i + 2].get());
  }
  this->toupdate_linear = other.toupdate_linear;
  this->toupdate_nonlinear = other.toupdate_nonlinear;
}

void Permutation::SboxStatus(std::vector<SboxPos>& active,
                             std::vector<SboxPos>& inactive) {
  active.clear();
  inactive.clear();

  for (unsigned int layer = 0; layer < rounds_; ++layer)
    for (unsigned int pos = 0; pos < this->sbox_layers_[layer]->GetNumSteps();
        ++pos)
      if (this->sbox_layers_[layer]->SboxGuessable(pos)) {
        if (this->sbox_layers_[layer]->SboxActive(pos))
          active.emplace_back(layer, pos);
        else
          inactive.emplace_back(layer, pos);
      }
}

void Permutation::SboxStatus(std::vector<std::vector<SboxPos>>& active,
                             std::vector<std::vector<SboxPos>>& inactive) {
  active.clear();
  inactive.clear();
  active.resize(rounds_);
  inactive.resize(rounds_);

  for (size_t layer = 0; layer < this->sbox_layers_.size(); ++layer)
    for (unsigned int pos = 0; pos < this->sbox_layers_[layer]->GetNumSteps();
        ++pos)
      if (this->sbox_layers_[layer]->SboxGuessable(pos)) {
        if (this->sbox_layers_[layer]->SboxActive(pos))
          active[layer].emplace_back(layer, pos);
        else
          inactive[layer].emplace_back(layer, pos);
      }
}

bool Permutation::isActive(SboxPos pos) {

  return this->sbox_layers_[pos.layer_]->SboxActive(pos.pos_);
}

bool Permutation::guessbestsbox(SboxPos pos,
                                std::function<int(int, int, int)> rating) {

  this->sbox_layers_[pos.layer_]->GuessBox(pos.pos_,
                                           rating);

  this->toupdate_linear = true;
  return update();
}

bool Permutation::guessbestsbox(SboxPos pos,
                                std::function<int(int, int, int)> rating,
                                int num_alternatives) {
  bool update_works = false;
  std::unique_ptr<Permutation> temp;
  temp.reset(this->clone());

  for (int i = 0; i < num_alternatives; ++i) {
    int total_alternatives = this->sbox_layers_[pos.layer_]->GuessBox(
        pos.pos_, rating, i);
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

bool Permutation::guessbestsboxrandom(SboxPos pos,
                                      std::function<int(int, int, int)> rating,
                                      int num_alternatives) {
  bool update_works = false;
  std::unique_ptr<Permutation> temp;
  temp.reset(this->clone());

  for (int i = 0; i < num_alternatives; ++i) {
    int total_alternatives = 0xffff;
    if (i)
      total_alternatives = this->sbox_layers_[pos.layer_]->GuessBox(pos.pos_, rating, i);
    else
      this->sbox_layers_[pos.layer_]->GuessBoxRandom(pos.pos_, rating);
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

void Permutation::set(Permutation* perm) {
  for (unsigned int i = 0; i < 2 * rounds_ + 1; ++i) {
    this->state_masks_[i].reset(perm->state_masks_[i]->clone());
  }

  for (unsigned int i = 0; i < rounds_; ++i) {
    this->sbox_layers_[i].reset(perm->sbox_layers_[i]->clone());
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2 * i].get(),
                                    this->state_masks_[2 * i + 1].get());
    this->linear_layers_[i].reset(perm->linear_layers_[i]->clone());
    this->linear_layers_[i]->SetMasks(this->state_masks_[2 * i + 1].get(),
                                      this->state_masks_[2 * i + 2].get());
  }
  this->toupdate_linear = perm->toupdate_linear;
  this->toupdate_nonlinear = perm->toupdate_nonlinear;
}

bool Permutation::update() {
  //TODO: Better update
  std::unique_ptr<StateMaskBase> tempin, tempout;
  bool update_before, update_after;
  while (this->toupdate_linear == true || this->toupdate_nonlinear == true) {
    if (this->toupdate_nonlinear == true) {
      this->toupdate_nonlinear = false;
      for (unsigned int layer = 0; layer < rounds_; ++layer) {
        tempin.reset(this->sbox_layers_[layer]->in->clone());
        tempout.reset(this->sbox_layers_[layer]->out->clone());
        if (this->sbox_layers_[layer]->Update() == false)
          return false;
        update_before = this->sbox_layers_[layer]->in->diffLinear(*(tempin));
        update_after = this->sbox_layers_[layer]->out->diffLinear(*(tempout));
        if(((update_before == true) && (layer != 0)) ||  update_after)
          this->toupdate_linear = true;
      }
    }
    if (this->toupdate_linear == true) {
      this->toupdate_linear = false;
      for (unsigned int layer = 0; layer < rounds_; ++layer) {
        tempin.reset(this->linear_layers_[layer]->in->clone());
        tempout.reset(this->linear_layers_[layer]->out->clone());
        if (this->linear_layers_[layer]->Update() == false)
          return false;
        update_before = this->linear_layers_[layer]->in->diffSbox(*(tempin));
        update_after = this->linear_layers_[layer]->out->diffSbox(*(tempout));
        if(((update_after == true) && (layer != rounds_ - 1)) ||  update_before)
          this->toupdate_nonlinear = true;
    }
  }
  }
  return true;
}

double Permutation::GetProbability() {
  double prob = 0.0;
  double temp_prob;

  for (auto& layer : this->sbox_layers_) {
    temp_prob = layer->GetProbability();
    prob += temp_prob;
  }

  prob += rounds_ - 1;

  return prob;
}

unsigned int Permutation::GetActiveSboxes() {
  unsigned int active_sboxes_layer = 0;

  for (auto& layer : this->sbox_layers_) {
    for (unsigned int j = 0; j < layer->GetNumSteps(); ++j)
      active_sboxes_layer += layer->SboxActive(j);
  }

  return active_sboxes_layer;
}

bool Permutation::checkchar(std::ostream& stream) {
  bool correct;
  stream << "Characteristic before propagation" << std::endl;
  this->print(stream);
  correct = this->update();
  stream << "Characteristic after propagation" << std::endl;
  this->print(stream);
  return correct;
}

void Permutation::print(std::ostream& stream) {
  int i = 0;
  for (const auto& state : state_masks_) {
    stream << "State Mask " << ++i << std::endl;
    state->print(stream);
    stream << std::endl;
  }
}

void Permutation::PrintWithProbability(std::ostream& stream,
                                       unsigned int offset) {
  double prob = 0.0;
  double temp_prob;
  int active_sboxes = 0;
  for (unsigned int i = 0; i <= 2 * rounds_; ++i) {
    stream << "State Mask " << i + 1;
    if (i % 2 == offset && i < 2 * rounds_) {
      temp_prob = this->sbox_layers_[i / 2]->GetProbability();
      int active_sboxes_layer = 0;
      for (unsigned int j = 0; j < this->sbox_layers_[i / 2]->GetNumSteps();
          ++j)
        active_sboxes_layer += (int) this->sbox_layers_[i / 2]->SboxActive(j);
      active_sboxes += active_sboxes_layer;
      prob += temp_prob;
      stream << " bias: " << temp_prob << " active sboxes: "
             << active_sboxes_layer;
    }
    stream << std::endl;
    this->state_masks_[i]->print(stream);
    stream << std::endl;
  }
  prob += rounds_ - 1;
  stream << "Total: bias: " << prob << " active sboxes: " << active_sboxes
         << std::endl << std::endl;

  stream << "----------------------------------------------------------------"
         << std::endl;
}

void Permutation::touchall() {
  this->toupdate_linear = true;
  this->toupdate_nonlinear = true;
}

bool Permutation::setBit(BitMask cond, unsigned int bit,
                         unsigned char num_words, unsigned char num_bits) {
  unsigned int state = bit / (num_words * num_bits);
  if (state >= 2 * rounds_ + 1)
    return false;

  bit %= (num_words * num_bits);

  unsigned int word = bit / num_bits;

  bit %= (num_bits);

  bit = num_bits - bit - 1;

  state_masks_[state]->SetBit(cond, word, bit);

  return true;
}

bool Permutation::setBit(BitMask cond, unsigned int bit) {
  return setBit(cond, bit, state_masks_.begin()->get()->getnumwords(),
                state_masks_.begin()->get()->getnumbits());
}

bool Permutation::setBox(bool active, unsigned int box_num) {
  //FIXME: Do not assume every layer has the same amount of sboxes
  unsigned int box_per_layer = sbox_layers_.begin()->get()->GetNumSteps();
  unsigned int sbox_layer = box_num / box_per_layer;
  if (sbox_layer >= sbox_layers_.size())
    return false;

  sbox_layers_[sbox_layer]->SetSboxActive(
      box_per_layer - (box_num % box_per_layer) - 1, active);

  return true;
}

