#include "pride.h"

#define TERMINALCOLORS

PrideState::PrideState()
    : StateMask() {
}

PrideState*  PrideState::clone(){
  PrideState* obj =  new PrideState();
  for(size_t j = 0; j< words_.size(); ++j){
    obj->words_[j] = words_[j];
    obj->changes_for_linear_layer_[j] = changes_for_linear_layer_[j];
    obj->changes_for_sbox_layer_[j] = changes_for_sbox_layer_[j];
  }
  return obj;
}


void PrideState::print(std::ostream& stream){
  stream << *this;
}


std::ostream& operator<<(std::ostream& stream, const PrideState& statemask) {
#ifndef TERMINALCOLORS
  char symbol[4] {'#', '1', '0', '?'};
#else
  std::string symbol[4] {"\033[1;35m#\033[0m", "\033[1;31m1\033[0m", "0", "\033[1;33m?\033[0m"};
#endif
  for (size_t j = 0; j< statemask.words_.size(); ++j){
    for (auto it = statemask.words_[j].bitmasks.rbegin(); it != statemask.words_[j].bitmasks.rend(); ++it){
      stream << symbol[*it % 4];
    }
    stream << " ";
  }
  stream << std::endl;
  return stream;
}


//-----------------------------------------------------------------------------
std::array<BitVector, 2> PrideLinear0(std::array<BitVector, 2> in) {

  BitVector t0, t1;
  t0 = in[0];
  t1 = in[1];
  in[0] = (in[0] >> 4) | (in[0] << 4);
  in[1] = (in[1] >> 4) | (in[1] << 4);
  in[0] ^= in[1];
  t0 ^= in[0];
  in[1] = t0;
  in[0] ^= t1;

for(int i = 0; i < 2; ++i)
  in[i] &= (~0ULL >> (64-8));

  return in;
}

std::array<BitVector, 2> PrideLinear1(std::array<BitVector, 2> in) {

  BitVector t0, t1, t2, t3;
  in[1] = (in[1] >> 4) | (in[1] << 4);
  t0 = in[0];
  t1 = in[1];
  t2 = in[0];
  t3 = in[1];
  t2 = (t2 << 1) | (t0 >> 7);
  t0 <<= 1;
  t3 = (t3 >> 1) | (t1 << 7);
  t1 >>= 1;
  in[0] ^= t3;
  t0 = in[0];
  in[0] ^= t2;
  in[1] ^= t0;

for(int i = 0; i < 2; ++i)
  in[i] &= (~0ULL >> (64-8));

  return in;
}

std::array<BitVector, 2> PrideLinear2(std::array<BitVector, 2> in) {

  BitVector t0, t1, t2, t3;
  in[0] = (in[0] >> 4) | (in[0] << 4);
  t0 = in[0];
  t1 = in[1];
  t2 = in[0];
  t3 = in[1];
  t2 = (t2 << 1) | (t0 >> 7);
  t0 <<= 1;
  t3 = (t3 >> 1) | (t1 << 7);
  t1 >>= 1;
  in[0] ^= t3;
  t0 = in[0];
  in[0] ^= t2;
  in[1] ^= t0;

for(int i = 0; i < 2; ++i)
  in[i] &= (~0ULL >> (64-8));

  return in;
}

std::array<BitVector, 2> PrideLinear3(std::array<BitVector, 2> in) {

  BitVector t0, t1;
  t0 = in[0];
  t1 = in[1];
  in[0] = (in[0] >> 4) | (in[0] << 4);
  in[1] = (in[1] >> 4) | (in[1] << 4);
  in[0] ^= in[1];
  t1 ^= in[0];
  in[1] = t1;
  in[0] ^= t0;

for(int i = 0; i < 2; ++i)
  in[i] &= (~0ULL >> (64-8));

  return in;
}

std::unique_ptr<
    LRU_Cache<
        WordMaskArray<PrideLinearLayer::word_size_,
            PrideLinearLayer::words_per_step_>,
        LinearStepUpdateInfo<PrideLinearLayer::word_size_,
            PrideLinearLayer::words_per_step_>>> PrideLinearLayer::cache_[PrideLinearLayer::linear_steps_];

PrideLinearLayer& PrideLinearLayer::operator=(const PrideLinearLayer& rhs){
  layers = rhs.layers;
  return *this;
}

PrideLinearLayer::PrideLinearLayer() {
  Init();
}

unsigned int PrideLinearLayer::GetNumSteps() {
  return layers.size();
}

PrideLinearLayer* PrideLinearLayer::clone(){
  //TODO: write copy constructor
  PrideLinearLayer* obj = new PrideLinearLayer(in,out);
  obj->layers = this->layers;
  return obj;
}

PrideLinearLayer::PrideLinearLayer(StateMaskBase *in, StateMaskBase *out) : LinearLayer(in, out) {
  Init();
}

void PrideLinearLayer::Init(){
  layers[0].Initialize(PrideLinear0);
  layers[1].Initialize(PrideLinear1);
  layers[2].Initialize(PrideLinear2);
  layers[3].Initialize(PrideLinear3);
  for(unsigned int i = 0; i< linear_steps_; ++i)
  if (this->cache_[i].get() == nullptr)
      this->cache_[i].reset(
          new LRU_Cache<WordMaskArray<word_size_, words_per_step_>, LinearStepUpdateInfo<word_size_,words_per_step_>>(cache_size_));
}

//TODO: map update to bits
bool PrideLinearLayer::updateStep(unsigned int step_pos) {
    return layers[step_pos].Update( { &((*in)[2*step_pos]), &((*in)[2*step_pos+1]) },
                            { &((*out)[2*step_pos]), &((*out)[2*step_pos+1]) },
                            cache_[step_pos].get());



//  return layers[pos.word].Update( { &((*in)[2*pos.word]), &((*in)[2*pos.word+1]) },
//                          { &((*out)[2*pos.word]), &((*out)[2*pos.word+1]) });




  assert(!"something went wrong");

      return false;
}

//-----------------------------------------------------------------------------


BitVector PrideSbox(BitVector in) {
  BitVector in0 = (in>>0)&1;
  BitVector in1 = (in>>1)&1;
  BitVector in2 = (in>>2)&1;
  BitVector in3 = (in>>3)&1;

  BitVector t0, t1;
  t0 = in0;
  t1 = in1;
  in0 &= in1;
  in0 ^= in2;
  in1 &= in2;
  in1 ^= in3;
  in2 = in0;
  in3 = in1;
  in2 &= in3;
  in2 ^= t0;
  in3 &= in2;
  in3 ^= t1;

  return (in0 & 1) | ((in1&1)<<1)| ((in2&1)<<2)| ((in3&1)<<3);
}

std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> PrideSboxLayer::cache_;

PrideSboxLayer& PrideSboxLayer::operator=(const PrideSboxLayer& rhs){
  sboxes = rhs.sboxes;
  return *this;
}

PrideSboxLayer::PrideSboxLayer() {
  InitSboxes(PrideSbox);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(cache_size_));
}

PrideSboxLayer::PrideSboxLayer(StateMaskBase *in, StateMaskBase *out)
    : SboxLayer(in, out) {
  InitSboxes(PrideSbox);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(cache_size_));
}

PrideSboxLayer* PrideSboxLayer::clone(){
  //TODO: write copy constructor
  PrideSboxLayer* obj = new PrideSboxLayer(in,out);
  obj->sboxes = this->sboxes;
  return obj;
}


bool PrideSboxLayer::updateStep(unsigned int step_pos) {

  assert(step_pos < sboxes.size());

  bool ret_val;
  Mask copyin(GetVerticalMask(step_pos, *in));
  Mask copyout(GetVerticalMask(step_pos, *out));
  ret_val = sboxes[step_pos].Update(copyin, copyout, cache_.get());
  SetVerticalMask(step_pos, *in, copyin);
  SetVerticalMask(step_pos, *out, copyout);
  return ret_val;
}

Mask PrideSboxLayer::GetVerticalMask(unsigned int b, const StateMaskBase& s) const {
  if(b < 8)
  return Mask(
      { s[0].bitmasks[b%8], s[2].bitmasks[b%8], s[4].bitmasks[b%8], s[6].bitmasks[b%8]});
  return Mask(
        { s[1].bitmasks[b%8], s[3].bitmasks[b%8], s[5].bitmasks[b%8], s[7].bitmasks[b%8]});
}

void PrideSboxLayer::SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask) {
  int offset = 0;
  if(b > 7)
    offset++;
  s[6+offset].bitmasks[b%8] = mask.bitmasks[3];
  s[4+offset].bitmasks[b%8] = mask.bitmasks[2];
  s[2+offset].bitmasks[b%8] = mask.bitmasks[1];
  s[0+offset].bitmasks[b%8] = mask.bitmasks[0];
  BitVector m = ~(1ULL << b%8);
  s[6+offset].caremask.canbe1 = (s[6+offset].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 3) & 1) << b%8);
  s[4+offset].caremask.canbe1 = (s[4+offset].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 2) & 1) << b%8);
  s[2+offset].caremask.canbe1 = (s[2+offset].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 1) & 1) << b%8);
  s[0+offset].caremask.canbe1 = (s[0+offset].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 0) & 1) << b%8);

  s[6+offset].caremask.care = (s[6+offset].caremask.care & m) | (((mask.caremask.care >> 3) & 1) << b%8);
  s[4+offset].caremask.care = (s[4+offset].caremask.care & m) | (((mask.caremask.care >> 2) & 1) << b%8);
  s[2+offset].caremask.care = (s[2+offset].caremask.care & m) | (((mask.caremask.care >> 1) & 1) << b%8);
  s[0+offset].caremask.care = (s[0+offset].caremask.care & m) | (((mask.caremask.care >> 0) & 1) << b%8);
}

