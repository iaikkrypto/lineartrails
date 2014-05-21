#include "ascon.h"


AsconState::AsconState() : words{{Mask(64), Mask(64), Mask(64), Mask(64), Mask(64)}} {
}

AsconState& AsconState::operator=(AsconState rhs)
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

AsconSboxLayer::AsconSboxLayer(StateMask *in, StateMask *out) : Layer(in, out) {
  for (int i = 0; i < 64; i++)
    sboxes[i].Initialize(AsconSbox);
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
  s[3].caremask.canbe1 = (s[3].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 2) & 1) << b);
  s[4].caremask.canbe1 = (s[4].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 0) & 1) << b);

  s[0].caremask.care = (s[0].caremask.care & m) | (((mask.caremask.care >> 4) & 1) << b);
  s[1].caremask.care = (s[1].caremask.care & m) | (((mask.caremask.care >> 3) & 1) << b);
  s[2].caremask.care = (s[2].caremask.care & m) | (((mask.caremask.care >> 2) & 1) << b);
  s[3].caremask.care = (s[3].caremask.care & m) | (((mask.caremask.care >> 2) & 1) << b);
  s[4].caremask.care = (s[4].caremask.care & m) | (((mask.caremask.care >> 0) & 1) << b);
}

//-----------------------------------------------------------------------------

AsconPermutation::AsconPermutation(int number_steps) {
  state_masks_.resize(2 * number_steps + 1);
  for (int i = 0; i < number_steps; ++i) {
    layers_.emplace_back( new
        AsconSboxLayer(&(state_masks_[i]), &(state_masks_[i + 1])));
    layers_.emplace_back( new
        AsconLinearLayer(&(state_masks_[i + 1]), &(state_masks_[i + 2])));
  }
  touchall();
}

AsconPermutation& AsconPermutation::operator=(AsconPermutation& rhs){
 state_masks_ = rhs.state_masks_;
 toupdate_linear = rhs.toupdate_linear;
 toupdate_nonlinear = rhs.toupdate_nonlinear;
layers_.clear();
 for (size_t i = 0; i < (state_masks_.size()>>1); ++i) {
   layers_.emplace_back( new
       AsconSboxLayer(&(state_masks_[i]), &(state_masks_[i + 1])));
   layers_.emplace_back( new
       AsconLinearLayer(&(state_masks_[i + 1]), &(state_masks_[i + 2])));
 }
 return *this;
}

bool AsconPermutation::checkchar() {
  bool correct;
  std::cout << "Characteristic before propagation" << std::endl << *this;
  correct = update();
  std::cout << "Characteristic after propagation" << std::endl << *this;
  return correct;
}
bool AsconPermutation::randomsboxguess() {


  std::default_random_engine generator;
  std::uniform_int_distribution<int> layer(0,layers_.size()/2);
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

    tempin = *((AsconState*) layers_[picklayer]->in);
    tempout = *((AsconState*) layers_[picklayer]->out);

    layers_[picklayer]->GuessBox(UpdatePos(0, 0, guessable_pos[sbox(generator)], 0));

    if (tempin.diff(*((AsconState*) layers_[picklayer]->in)).size() != 0
        || tempout.diff(*((AsconState*) layers_[picklayer]->out)).size() != 0)
      toupdate_linear = true;

    touchall();
    bool ret = update();
    std::cout << *this << std::endl;

    return ret;
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
      for (size_t layer = 0; layer < layers_.size(); layer += 2) {
        tempin = *((AsconState*) layers_[layer]->in);
        tempout = *((AsconState*) layers_[layer]->out);
        for (int i = 0; i < 64; ++i)
          correct &= layers_[layer]->Update(UpdatePos(0, 0, i, 1));
        if (tempin.diff(*((AsconState*) layers_[layer]->in)).size() != 0
            || tempout.diff(*((AsconState*) layers_[layer]->out)).size() != 0)
          toupdate_linear = true;
      }
    }
    if (toupdate_linear == true) {
      toupdate_linear = false;
      for (size_t layer = 1; layer < layers_.size(); layer += 2) {
        tempin = *((AsconState*) layers_[layer]->in);
        tempout = *((AsconState*) layers_[layer]->out);
        for (int i = 0; i < 5; ++i)
          correct &= layers_[layer]->Update(UpdatePos(0, i, 0, 1));
        if (tempin.diff(*((AsconState*) layers_[layer]->in)).size() != 0
            || tempout.diff(*((AsconState*) layers_[layer]->out)).size() != 0)
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

