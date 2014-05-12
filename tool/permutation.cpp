/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
#include "permutation.h"

//#define LATEX
//#define LATEXX 5
//#define LATEXY 1

Permutation::Permutation(unsigned int rounds)
    : toupdate_linear(true),
      toupdate_nonlinear(true),
      rounds_(rounds),
      saved_toupdate_linear(true),
      saved_toupdate_nonlinear(true){
  state_masks_.resize(2 * rounds_ + 1);
  sbox_layers_.resize(rounds_);
  linear_layers_.resize(rounds_);
  saved_state_masks_.resize(2 * rounds_ + 1);
  saved_sbox_layers_.resize(rounds_);
  saved_linear_layers_.resize(rounds_);
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

void Permutation::save() {
  for (unsigned int i = 0; i < 2 * rounds_ + 1; ++i) {
    saved_state_masks_[i]->copyValues(state_masks_[i].get());
  }

  for (unsigned int i = 0; i < rounds_; ++i) {
    saved_sbox_layers_[i]->copyValues(sbox_layers_[i].get());
    saved_linear_layers_[i]->copyValues(linear_layers_[i].get());
  }

  saved_toupdate_linear = toupdate_linear;
  saved_toupdate_nonlinear = toupdate_nonlinear;
}

void Permutation::restore() {
  for (unsigned int i = 0; i < 2 * rounds_ + 1; ++i) {
    state_masks_[i]->copyValues(saved_state_masks_[i].get());
  }

  for (unsigned int i = 0; i < rounds_; ++i) {
    sbox_layers_[i]->copyValues(saved_sbox_layers_[i].get());
    linear_layers_[i]->copyValues(saved_linear_layers_[i].get());
  }
  toupdate_linear = saved_toupdate_linear;
  toupdate_nonlinear = saved_toupdate_nonlinear;
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
  PermPtr temp = this->clone();

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

  save();
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
    restore();
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
  //TODO: Faster update still under test
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
//        update_before = this->sbox_layers_[layer]->in->changesforLinear();
//        update_after = this->sbox_layers_[layer]->out->changesforLinear();
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
//        update_before = this->linear_layers_[layer]->in->changesforSbox();
//        update_after = this->linear_layers_[layer]->out->changesforSbox();
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
    stream << "Round " << i/2;
    if(i%2 == 1)
      stream << ".5";
    stream  << std::endl;
    i++;
    state->print(stream);
    stream << std::endl;
  }
}

#ifdef LATEX
void Permutation::PrintWithProbability(std::ostream& stream,
                                       unsigned int offset) {
  int j = 0;
  int x = LATEXX;
  int y = LATEXY;

  double prob = 0.0;
  double temp_prob;
  int active_sboxes = 0;
  for (unsigned int i = 0; i <= 2 * rounds_; ++i) {
    if (i % 2 == offset && i < 2 * rounds_) {
      temp_prob = this->sbox_layers_[i / 2]->GetProbability();
      int active_sboxes_layer = 0;
      for (unsigned int j = 0; j < this->sbox_layers_[i / 2]->GetNumSteps();
          ++j)
        active_sboxes_layer += (int) this->sbox_layers_[i / 2]->SboxActive(j);
      active_sboxes += active_sboxes_layer;
      prob += temp_prob;
    }
  }
  prob += rounds_ - 1;


  stream << "\\begin{subtable}{\\textwidth}" << std::endl;
  stream << "\\caption{$r=" << state_masks_.size()/2 <<"$: ";
  stream << active_sboxes << " active S-boxes, bias $2^{";
  stream << prob << "}$.}" << std::endl << "\\label{tab:}" << std::endl;
  stream << "\\centering" << std::endl;
  stream << "{\\scriptsize" << std::endl;
  stream << "\\begin{tabular}{@{\\hskip .75em}c@{\\hskip .75em}";
  for(int i = 0;  i< x; ++i)
    stream << " c";
  stream << "}" << std::endl;

  stream << " \\toprule " << std::endl << "Round & \\multicolumn{" << x << "}{c}{State} \\\\ \\midrule" << std::endl;
  for (size_t i = 0; i < state_masks_.size(); ++i) {
    if(i%2==0){
    stream << "  \\multirow{" << y << "}{*}{" << j++ <<"}";
    state_masks_[i]->print(stream);
    if(i<state_masks_.size()-1)
    stream << " \\statesep"<< std::endl;
    else
    stream << " \\\\ \\bottomrule"<< std::endl;
    }
  }
  stream << "\\end{tabular}"<< std::endl;
      stream << "}"<< std::endl;
      stream << "\\end{subtable}"<< std::endl;

}
#else
void Permutation::PrintWithProbability(std::ostream& stream,
                                       unsigned int offset) {
  double prob = 0.0;
  double temp_prob;
  int active_sboxes = 0;
  for (unsigned int i = 0; i <= 2 * rounds_; ++i) {
    stream << "Round " << i/2;
    if(i%2 == 1)
      stream << ".5";
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
#endif

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
  //TODO: Do not assume every layer has the same amount of sboxes
  unsigned int box_per_layer = sbox_layers_.begin()->get()->GetNumSteps();
  unsigned int sbox_layer = box_num / box_per_layer;
  if (sbox_layer >= sbox_layers_.size())
    return false;

  sbox_layers_[sbox_layer]->SetSboxActive(
      box_per_layer - (box_num % box_per_layer) - 1, active);

  return true;
}

