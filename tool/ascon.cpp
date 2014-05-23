#include "ascon.h"


AsconState::AsconState() : words{{Mask(64), Mask(64), Mask(64), Mask(64), Mask(64)}} {
}

AsconState& AsconState::operator=(const AsconState& rhs)
{
  for(int j = 0; j< 5; ++j)
     words[j] = rhs.words[j];
  return *this;
}

std::vector<UpdatePos> AsconState::diff(const StateMask& other) {
  BitVector diffword;
  std::vector<UpdatePos> result;
  for (int i = 0; i < 5; ++i) {
    diffword = (words[i].caremask.canbe1 ^ other[i].caremask.canbe1) | (words[i].caremask.care ^ other[i].caremask.care);
    for (int b = 0; b < 63 && diffword; ++b) {
      if (diffword & 1)
        result.emplace_back(0, i, b, 0);
      diffword >>= 1;
    }
  }
  return result;
}

typename std::array<Mask, 5>::iterator AsconState::begin() {
  return words.begin();
}

typename std::array<Mask, 5>::const_iterator AsconState::begin() const {
  return words.begin();
}

typename std::array<Mask, 5>::iterator AsconState::end() {
  return words.end();
}

typename std::array<Mask, 5>::const_iterator AsconState::end() const {
  return words.end();
}

Mask& AsconState::operator[](const int index) {
  return words[index];
}

const Mask& AsconState::operator[](const int index) const {
  return words[index];
}

void AsconState::SetState(BitMask value){
  for(int j = 0; j< 5; ++j){
    for(int i = 0; i< 64; ++i)
      words[j].bitmasks[i] = value;
    words[j].reinit_caremask();
  }
}


std::ostream& operator<<(std::ostream& stream, const AsconState& statemask) {
  char symbol[4] {'#', '1', '0', '?'};
  for (Mask word : statemask.words){
    for (BitMask m : word.bitmasks){
      stream << symbol[m % 4];
    }
    stream << std::endl;
  }
  return stream;
}

//-----------------------------------------------------------------------------
AsconLinearLayer& AsconLinearLayer::operator=(const AsconLinearLayer& rhs){
  sigmas = rhs.sigmas;
  return *this;
}
AsconLinearLayer::AsconLinearLayer(StateMask *in, StateMask *out) : Layer(in, out) {
  sigmas[0].Initialize(AsconSigma<0>);
  sigmas[1].Initialize(AsconSigma<1>);
  sigmas[2].Initialize(AsconSigma<2>);
  sigmas[3].Initialize(AsconSigma<3>);
  sigmas[4].Initialize(AsconSigma<4>);
}

bool AsconLinearLayer::Update(UpdatePos pos) {
  return sigmas[pos.word].Update((*in)[pos.word], (*out)[pos.word]);
}

//-----------------------------------------------------------------------------


BitVector AsconSbox(BitVector in) {
  // with x0 as MSB
  static const BitVector sbox[32] = {
       4, 11, 31, 20, 26, 21,  9,  2, 27,  5,  8, 18, 29,  3,  6, 28,
      30, 19,  7, 14,  0, 13, 17, 24, 16, 12,  1, 25, 22, 10, 15, 23,
  };
  return sbox[in % 32];
}

AsconSboxLayer& AsconSboxLayer::operator=(const AsconSboxLayer& rhs){
  sboxes = rhs.sboxes;
  return *this;
}

AsconSboxLayer::AsconSboxLayer(StateMask *in, StateMask *out) : Layer(in, out) {
  std::shared_ptr<LinearDistributionTable<5>> ldt(new LinearDistributionTable<5>(AsconSbox));
  for (int i = 0; i < 64; i++)
    sboxes[i].Initialize(ldt);
}

bool AsconSboxLayer::Update(UpdatePos pos) {
  Mask copyin(GetVerticalMask(pos.bit, *in));
  Mask copyout(GetVerticalMask(pos.bit, *out));
  if (!sboxes[pos.bit].Update(copyin, copyout))
    return false;
  SetVerticalMask(pos.bit, *in, copyin);
  SetVerticalMask(pos.bit, *out, copyout);
  return true;
}

bool AsconSboxLayer::SboxActive(int pos){
  return sboxes[pos].is_active_;
}

bool AsconSboxLayer::SboxGuessable(int pos){
  return sboxes[pos].is_guessable_;
}

void AsconSboxLayer::GuessBox(UpdatePos pos) {
  Mask copyin(GetVerticalMask(pos.bit, *in));
  Mask copyout(GetVerticalMask(pos.bit, *out));

  sboxes[pos.bit].TakeBestBox(copyin, copyout);

  SetVerticalMask(pos.bit, *in, copyin);
  SetVerticalMask(pos.bit, *out, copyout);

}

Mask AsconSboxLayer::GetVerticalMask(int b, const StateMask& s) const {
  return Mask({s[4].bitmasks[b], s[3].bitmasks[b], s[2].bitmasks[b], s[1].bitmasks[b], s[0].bitmasks[b]});
}

void AsconSboxLayer::SetVerticalMask(int b, StateMask& s, const Mask& mask) {
  s[0].bitmasks[b] = mask.bitmasks[4];
  s[1].bitmasks[b] = mask.bitmasks[3];
  s[2].bitmasks[b] = mask.bitmasks[2];
  s[3].bitmasks[b] = mask.bitmasks[1];
  s[4].bitmasks[b] = mask.bitmasks[0];
  BitVector m = ~(1ULL << b);
  s[0].caremask.canbe1 = (s[0].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 4) & 1) << b);
  s[1].caremask.canbe1 = (s[1].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 3) & 1) << b);
  s[2].caremask.canbe1 = (s[2].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 2) & 1) << b);
  s[3].caremask.canbe1 = (s[3].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 1) & 1) << b);
  s[4].caremask.canbe1 = (s[4].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 0) & 1) << b);

  s[0].caremask.care = (s[0].caremask.care & m) | (((mask.caremask.care >> 4) & 1) << b);
  s[1].caremask.care = (s[1].caremask.care & m) | (((mask.caremask.care >> 3) & 1) << b);
  s[2].caremask.care = (s[2].caremask.care & m) | (((mask.caremask.care >> 2) & 1) << b);
  s[3].caremask.care = (s[3].caremask.care & m) | (((mask.caremask.care >> 1) & 1) << b);
  s[4].caremask.care = (s[4].caremask.care & m) | (((mask.caremask.care >> 0) & 1) << b);
}

//-----------------------------------------------------------------------------

AsconPermutation::AsconPermutation(int number_steps) {
  state_masks_.resize(2 * number_steps + 1);
  for (int i = 0; i < number_steps; ++i) {
    sbox_layers_.emplace_back(&(state_masks_[2*i]), &(state_masks_[2*i + 1]));
    linear_layers_.emplace_back(&(state_masks_[2*i + 1]), &(state_masks_[2*i + 2]));
  }
  touchall();
}

AsconPermutation& AsconPermutation::operator=(AsconPermutation& rhs){
 state_masks_ = rhs.state_masks_;
 toupdate_linear = rhs.toupdate_linear;
 toupdate_nonlinear = rhs.toupdate_nonlinear;
 sbox_layers_ = rhs.sbox_layers_;
 linear_layers_ = rhs.linear_layers_;
 return *this;
}

bool AsconPermutation::checkchar() {
  bool correct;
  std::cout << "Characteristic before propagation" << std::endl << *this;
  correct = update();
  std::cout << "Characteristic after propagation" << std::endl << *this;
  return correct;
}

bool AsconPermutation::guessbestsbox(SboxPos pos) {
  AsconState tempin, tempout;

    tempin = *((AsconState*) sbox_layers_[pos.layer_].in);
    tempout = *((AsconState*) sbox_layers_[pos.layer_].out);

    sbox_layers_[pos.layer_].GuessBox(UpdatePos(0, 0, pos.pos_, 0));

    if (tempin.diff(*((AsconState*) sbox_layers_[pos.layer_].in)).size() != 0
        || tempout.diff(*((AsconState*) sbox_layers_[pos.layer_].out)).size() != 0)
      toupdate_linear = true;
    return update();
}


bool AsconPermutation::randomsboxguess() {
  std::default_random_engine generator;
  std::uniform_int_distribution<int> layer(0,sbox_layers_.size());
  int picklayer = layer(generator) << 1;
  BitVector guessable = 0;
  std::vector<uint8_t> guessable_pos;

  for(int j = 0; j< 2; ++j){
    guessable = 0;
      for(int i = 0; i < 5; ++i)
        guessable |= ~state_masks_[picklayer+j].words[i].caremask.care;

  for(uint8_t i = 0; i < 64; ++i)
    if(((guessable >> i) & 1) == 1 )
      guessable_pos.push_back(i);

  }
  if(guessable_pos.size() == 0)
    return true;

  std::uniform_int_distribution<int> sbox(0,guessable_pos.size()-1);
  AsconState tempin, tempout;

    tempin = *((AsconState*) sbox_layers_[picklayer].in);
    tempout = *((AsconState*) sbox_layers_[picklayer].out);

    sbox_layers_[picklayer].GuessBox(UpdatePos(0, 0, guessable_pos[sbox(generator)], 0));

    if (tempin.diff(*((AsconState*) sbox_layers_[picklayer].in)).size() != 0
        || tempout.diff(*((AsconState*) sbox_layers_[picklayer].out)).size() != 0)
      toupdate_linear = true;

    return update();
}

bool AsconPermutation::anythingtoguess(){
  for (AsconState state : state_masks_){
    for(int i = 0; i < 5; ++i)
      if(state.words[i].caremask.care != (~0ULL))
        return true;
  }
  return false;
}

bool AsconPermutation::update() {
  //TODO: Better update
  bool correct = true;
  AsconState tempin, tempout;
  while (toupdate_linear == true || toupdate_nonlinear == true) {
    if (toupdate_nonlinear == true) {
      toupdate_nonlinear = false;
      for (size_t layer = 0; layer < sbox_layers_.size(); ++layer) {
        tempin = *((AsconState*) sbox_layers_[layer].in);
        tempout = *((AsconState*) sbox_layers_[layer].out);
        for (int i = 0; i < 64; ++i)
          correct &= sbox_layers_[layer].Update(UpdatePos(0, 0, i, 1));
        if (tempin.diff(*((AsconState*) sbox_layers_[layer].in)).size() != 0
            || tempout.diff(*((AsconState*) sbox_layers_[layer].out)).size() != 0)
          toupdate_linear = true;
      }
    }
    if (toupdate_linear == true) {
      toupdate_linear = false;
      for (size_t layer = 0; layer < linear_layers_.size(); ++layer) {
        tempin = *((AsconState*) linear_layers_[layer].in);
        tempout = *((AsconState*) linear_layers_[layer].out);
        for (int i = 0; i < 5; ++i)
          correct &= linear_layers_[layer].Update(UpdatePos(0, i, 0, 1));
        if (tempin.diff(*((AsconState*) linear_layers_[layer].in)).size() != 0
            || tempout.diff(*((AsconState*) linear_layers_[layer].out)).size() != 0)
          toupdate_nonlinear = true;
      }
    }
  }
  return true;
}

void AsconPermutation::touchall() {
//for(int i = 0; i< state_masks_.size(); ++i){
//  for(int j = 0; j<64; ++j)
//    queue_nonlinear_.add_item(UpdatePos(i, 0, j, 0));
//  for(int j = 0; j<5; ++j)
//      queue_linear_.add_item(UpdatePos(i, j, 0, 0));
//
//}
  toupdate_linear = true;
  toupdate_nonlinear = true;
}

std::ostream& operator<<(std::ostream& stream,
                         const AsconPermutation& permutation) {
  int i = 0;
  for (AsconState state : permutation.state_masks_)
    stream << "State Mask " << ++i << std::endl << state << std::endl;
  return stream;
}

void AsconPermutation::SboxStatus(std::vector<SboxPos>& active,
                                  std::vector<SboxPos>& inactive) {
  active.clear();
  inactive.clear();

  for (size_t layer = 0; layer < sbox_layers_.size(); ++layer)
    for (int pos = 0; pos < 64; ++pos)
      if (sbox_layers_[layer].SboxGuessable(pos)) {
        if (sbox_layers_[layer].SboxActive(pos))
          active.emplace_back(layer, pos);
        else
          inactive.emplace_back(layer, pos);
      }
}

void AsconPermutation::SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive){
  active.clear();
  inactive.clear();
  active.resize(sbox_layers_.size());
  inactive.resize(sbox_layers_.size());

  for (size_t layer = 0; layer < sbox_layers_.size(); ++layer)
    for (int pos = 0; pos < 64; ++pos)
      if (sbox_layers_[layer].SboxGuessable(pos)) {
        if (sbox_layers_[layer].SboxActive(pos))
          active[layer].emplace_back(layer, pos);
        else
          inactive[layer].emplace_back(layer, pos);
      }
}

