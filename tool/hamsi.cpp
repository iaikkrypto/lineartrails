#include "hamsi.h"

#define TERMINALCOLORS

HamsiState::HamsiState()
    : StateMask() {
}



HamsiState*  HamsiState::clone(){
  HamsiState* obj =  new HamsiState();
  for(size_t j = 0; j< words_.size(); ++j)
    obj->words_[j] = words_[j];
  return obj;
}

void HamsiState::print(std::ostream& stream){
  stream << *this;
}


std::ostream& operator<<(std::ostream& stream, const HamsiState& statemask) {
#ifndef TERMINALCOLORS
  char symbol[4] {'#', '1', '0', '?'};
#else
  std::string symbol[4] {"\033[1;35m#\033[0m", "\033[1;31m1\033[0m", "0", "\033[1;33m?\033[0m"};
#endif
  for (size_t j = 0; j< statemask.words_.size(); ++j){
    for (auto it = statemask.words_[j].bitmasks.rbegin(); it != statemask.words_[j].bitmasks.rend(); ++it){
      stream << symbol[*it % 4];
    }
    stream << "\t";
    if((j%4) == 3)
    stream << std::endl;
  }
  return stream;
}

//-----------------------------------------------------------------------------

HamsiStateFeedForward::HamsiStateFeedForward(HamsiState* other_state)
    : HamsiState() {
  other_state_ = other_state;
}


HamsiStateFeedForward*  HamsiStateFeedForward::clone(){
  HamsiStateFeedForward* obj =  new HamsiStateFeedForward(other_state_);
  for(size_t j = 0; j< words_.size(); ++j)
    obj->words_[j] = words_[j];
  return obj;
}

HamsiStateFeedForward*  HamsiStateFeedForward::clone(HamsiState* other_state){
  HamsiStateFeedForward* obj =  new HamsiStateFeedForward(other_state);
  for(size_t j = 0; j< words_.size(); ++j)
    (*obj)[j] = (*this)[j];
  return obj;
}

Mask& HamsiStateFeedForward::operator[](const int index) {
  switch (index) {
    case 0:
      return words_[0];
    case 1:
      return words_[1];
    case 2:
      return (*other_state_)[0];
    case 3:
      return (*other_state_)[1];
    case 4:
      return (*other_state_)[2];
    case 5:
      return (*other_state_)[3];
    case 6:
      return words_[6];
    case 7:
      return words_[7];
    case 8:
      return words_[8];
    case 9:
      return words_[9];
    case 10:
      return (*other_state_)[8];
    case 11:
      return (*other_state_)[9];
    case 12:
      return (*other_state_)[10];
    case 13:
      return (*other_state_)[11];
    case 14:
      return words_[14];
    case 15:
      return words_[15];
  }

  return words_[index];
}

const Mask& HamsiStateFeedForward::operator[](const int index) const {
  switch (index) {
    case 0:
      return words_[0];
    case 1:
      return words_[1];
    case 2:
      return (*other_state_)[0];
    case 3:
      return (*other_state_)[1];
    case 4:
      return (*other_state_)[2];
    case 5:
      return (*other_state_)[3];
    case 6:
      return words_[6];
    case 7:
      return words_[7];
    case 8:
      return words_[8];
    case 9:
      return words_[9];
    case 10:
      return (*other_state_)[8];
    case 11:
      return (*other_state_)[9];
    case 12:
      return (*other_state_)[10];
    case 13:
      return (*other_state_)[11];
    case 14:
      return words_[14];
    case 15:
      return words_[15];
  }
  return words_[index];
}

void HamsiStateFeedForward::print(std::ostream& stream){
  stream << *this;
}


std::ostream& operator<<(std::ostream& stream, const HamsiStateFeedForward& statemask) {
#ifndef TERMINALCOLORS
  char symbol[4] {'#', '1', '0', '?'};
#else
  std::string symbol[4] {"\033[1;35m#\033[0m", "\033[1;31m1\033[0m", "0", "\033[1;33m?\033[0m"};
#endif
  for (size_t j = 0; j< statemask.words_.size(); ++j){
    for (auto it = statemask[j].bitmasks.rbegin(); it != statemask[j].bitmasks.rend(); ++it){
      stream << symbol[*it % 4];
    }
    stream << "\t";
    if((j%4) == 3)
    stream << std::endl;
  }
  return stream;
}

//-----------------------------------------------------------------------------

std::array<BitVector, 4> HamsiLinear(std::array<BitVector, 4> in) {

in[0] = ROTL32(in[0],13);
in[2] = ROTL32(in[2],3);
in[1] ^= in[0] ^ in[2];
in[3] ^= in[2] ^ (in[0] << 3);
in[1] = ROTL32(in[1],1);
in[3] = ROTL32(in[3],7);
in[0] ^= in[1] ^ in[3];
in[2] ^= in[3] ^ (in[1] << 7);
in[0] = ROTL32(in[0],5);
in[2] = ROTL32(in[2],22);

for(int i = 0; i < 4; ++i)
  in[i] &= (~0ULL >> (32));

  return in;
}

std::unique_ptr<
    LRU_Cache<
        WordMaskArray<HamsiLinearLayer::word_size_,
            HamsiLinearLayer::words_per_step_>,
        LinearStepUpdateInfo<HamsiLinearLayer::word_size_,
            HamsiLinearLayer::words_per_step_>>> HamsiLinearLayer::cache_[1];

HamsiLinearLayer& HamsiLinearLayer::operator=(const HamsiLinearLayer& rhs){
  layers = rhs.layers;
  return *this;
}

HamsiLinearLayer::HamsiLinearLayer() {
  Init();
}

unsigned int HamsiLinearLayer::GetNumSteps() {
  return layers.size();
}

HamsiLinearLayer* HamsiLinearLayer::clone(){
  //TODO: write copy constructor
  HamsiLinearLayer* obj = new HamsiLinearLayer(in,out);
  obj->layers = this->layers;
  return obj;
}

HamsiLinearLayer::HamsiLinearLayer(StateMaskBase *in, StateMaskBase *out) : LinearLayer(in, out) {
  Init();
}

void HamsiLinearLayer::Init(){
  layers[0].Initialize(HamsiLinear);
  layers[1].Initialize(HamsiLinear);
  layers[2].Initialize(HamsiLinear);
  layers[3].Initialize(HamsiLinear);
  if (this->cache_[0].get() == nullptr)
      this->cache_[0].reset(
          new LRU_Cache<WordMaskArray<word_size_, words_per_step_>, LinearStepUpdateInfo<word_size_,words_per_step_>>(cache_size_));
}

bool HamsiLinearLayer::Update(unsigned int step_pos) {
  if (step_pos == 0 || step_pos == 5 || step_pos == 10 || step_pos == 15)
    return layers[0].Update( { &((*in)[0]), &((*in)[5]), &((*in)[10]),
                                &((*in)[15]) },
                            { &((*out)[0]), &((*out)[5]), &((*out)[10]),
                                &((*out)[15]) },
                            cache_[0].get());
  if (step_pos == 1 || step_pos == 6 || step_pos == 11 || step_pos == 12)
      return layers[1].Update( { &((*in)[1]), &((*in)[6]), &((*in)[11]),
                                  &((*in)[12]) },
                              { &((*out)[1]), &((*out)[6]), &((*out)[11]),
                                  &((*out)[12]) },
                              cache_[0].get());
  if (step_pos == 2 || step_pos == 7 || step_pos == 8 || step_pos == 13)
      return layers[2].Update( { &((*in)[2]), &((*in)[7]), &((*in)[8]),
                                  &((*in)[13]) },
                              { &((*out)[2]), &((*out)[7]), &((*out)[8]),
                                  &((*out)[13]) },
                              cache_[0].get());

  if (step_pos == 3 || step_pos == 4 || step_pos== 9 || step_pos == 14)
      return layers[3].Update( { &((*in)[3]), &((*in)[4]), &((*in)[9]),
                                  &((*in)[14]) },
                              { &((*out)[3]), &((*out)[4]), &((*out)[9]),
                                  &((*out)[14]) },
                              cache_[0].get());


  assert(!"something went wrong");

      return false;
}

//-----------------------------------------------------------------------------


BitVector HamsiSbox(BitVector in) {
  // with x0 as MSB
  static const BitVector sbox[16] = { 8, 6, 7, 9, 3, 0xc, 0xa, 0xf, 0xd, 1, 0xe,
      4, 0, 0xb, 5, 2 };
  return sbox[in % 16];
}

std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> HamsiSboxLayer::cache_;

HamsiSboxLayer& HamsiSboxLayer::operator=(const HamsiSboxLayer& rhs){
  sboxes = rhs.sboxes;
  return *this;
}

HamsiSboxLayer::HamsiSboxLayer() {
  InitSboxes(HamsiSbox);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(cache_size_));
}

HamsiSboxLayer::HamsiSboxLayer(StateMaskBase *in, StateMaskBase *out)
    : SboxLayer(in, out) {
  InitSboxes(HamsiSbox);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(cache_size_));
}


HamsiSboxLayer* HamsiSboxLayer::clone(){
  //TODO: write copy constructor
  HamsiSboxLayer* obj = new HamsiSboxLayer(in,out);
  obj->sboxes = this->sboxes;
  return obj;
}


bool HamsiSboxLayer::Update(unsigned int step_pos) {
  assert(step_pos < sboxes.size());

  bool ret_val;
  Mask copyin(GetVerticalMask(step_pos, *in));
  Mask copyout(GetVerticalMask(step_pos, *out));
  ret_val = sboxes[step_pos].Update(copyin, copyout, cache_.get());
  SetVerticalMask(step_pos, *in, copyin);
  SetVerticalMask(step_pos, *out, copyout);
  return ret_val;
}

Mask HamsiSboxLayer::GetVerticalMask(unsigned int b, const StateMaskBase& s) const {
  return Mask(
      { s[b/32].bitmasks[b%32], s[b/32 + 4].bitmasks[b%32], s[b/32 + 8].bitmasks[b%32], s[b/32 +12].bitmasks[b%32]});
}

void HamsiSboxLayer::SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask) {
  s[b/32+12].bitmasks[b%32] = mask.bitmasks[3];
  s[b/32+8].bitmasks[b%32] = mask.bitmasks[2];
  s[b/32+4].bitmasks[b%32] = mask.bitmasks[1];
  s[b/32+0].bitmasks[b%32] = mask.bitmasks[0];
  BitVector m = ~(1ULL << b%32);
  s[b/32+12].caremask.canbe1 = (s[b/32+12].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 3) & 1) << b%32);
  s[b/32+8].caremask.canbe1 = (s[b/32+8].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 2) & 1) << b%32);
  s[b/32+4].caremask.canbe1 = (s[b/32+4].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 1) & 1) << b%32);
  s[b/32+0].caremask.canbe1 = (s[b/32+0].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 0) & 1) << b%32);

  s[b/32+12].caremask.care = (s[b/32+12].caremask.care & m) | (((mask.caremask.care >> 3) & 1) << b%32);
  s[b/32+8].caremask.care = (s[b/32+8].caremask.care & m) | (((mask.caremask.care >> 2) & 1) << b%32);
  s[b/32+4].caremask.care = (s[b/32+4].caremask.care & m) | (((mask.caremask.care >> 1) & 1) << b%32);
  s[b/32+0].caremask.care = (s[b/32+0].caremask.care & m) | (((mask.caremask.care >> 0) & 1) << b%32);
}

