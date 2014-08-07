#include "hamsi.h"

#define TERMINALCOLORS

HamsiState::HamsiState()
    : words { { Mask(32), Mask(32), Mask(32), Mask(32), Mask(32), Mask(32),
        Mask(32), Mask(32), Mask(32), Mask(32), Mask(32), Mask(32), Mask(32),
        Mask(32), Mask(32), Mask(32) } } {
}

HamsiState& HamsiState::operator=(const HamsiState& rhs) {
  for (size_t j = 0; j < words.size(); ++j)
    words[j] = rhs.words[j];
  return *this;
}


HamsiState*  HamsiState::clone(){
  HamsiState* obj =  new HamsiState();
  for(size_t j = 0; j< words.size(); ++j)
    obj->words[j] = words[j];
  return obj;
}

std::vector<UpdatePos> HamsiState::diff(const StateMask& other) {
  BitVector diffword;
  std::vector<UpdatePos> result;
  for (size_t i = 0; i < words.size(); ++i) {
    diffword = (words[i].caremask.canbe1 ^ other[i].caremask.canbe1) | (words[i].caremask.care ^ other[i].caremask.care);
    for (int b = 0; b < 32 && diffword; ++b) {
      if (diffword & 1)
        result.emplace_back(0, i, b, 0);
      diffword >>= 1;
    }
  }
  return result;
}

typename std::array<Mask, 16>::iterator HamsiState::begin() {
  return words.begin();
}

typename std::array<Mask, 16>::const_iterator HamsiState::begin() const {
  return words.begin();
}

typename std::array<Mask, 16>::iterator HamsiState::end() {
  return words.end();
}

typename std::array<Mask, 16>::const_iterator HamsiState::end() const {
  return words.end();
}

Mask& HamsiState::operator[](const int index) {
  return words[index];
}

const Mask& HamsiState::operator[](const int index) const {
  return words[index];
}

void HamsiState::SetState(BitMask value){
  for(size_t j = 0; j< words.size(); ++j){
    for(int i = 0; i< 32; ++i)
      words[j].bitmasks[i] = value;
    words[j].reinit_caremask();
  }
}

void HamsiState::SetBit(BitMask value, int word_pos, int bit_pos){
  words.at(word_pos).set_bit(value, bit_pos);
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
  for (size_t j = 0; j< statemask.words.size(); ++j){
    for (auto it = statemask.words[j].bitmasks.rbegin(); it != statemask.words[j].bitmasks.rend(); ++it){
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


std::unique_ptr<LRU_Cache<WordMaskArray<32, 4>, LinearStepUpdateInfo<32, 4>>> HamsiLinearLayer::cache_[1];

HamsiLinearLayer& HamsiLinearLayer::operator=(const HamsiLinearLayer& rhs){
  layers = rhs.layers;
  return *this;
}

HamsiLinearLayer::HamsiLinearLayer() {
  Init();
}

int HamsiLinearLayer::GetNumLayer() {
  return layers.size();
}

HamsiLinearLayer* HamsiLinearLayer::clone(){
  //TODO: write copy constructor
  HamsiLinearLayer* obj = new HamsiLinearLayer(in,out);
  obj->layers = this->layers;
  return obj;
}

HamsiLinearLayer::HamsiLinearLayer(StateMask *in, StateMask *out) : LinearLayer(in, out) {
  Init();
}

void HamsiLinearLayer::Init(){
  layers[0].Initialize(HamsiLinear);
  layers[1].Initialize(HamsiLinear);
  layers[2].Initialize(HamsiLinear);
  layers[3].Initialize(HamsiLinear);
  if (this->cache_[0].get() == nullptr)
      this->cache_[0].reset(
          new LRU_Cache<WordMaskArray<32, 4>, LinearStepUpdateInfo<32,4>>(0x1000));
}

bool HamsiLinearLayer::Update(UpdatePos pos) {
  if (pos.word == 0 || pos.word == 5 || pos.word == 10 || pos.word == 15)
    return layers[0].Update( { &((*in)[0]), &((*in)[5]), &((*in)[10]),
                                &((*in)[15]) },
                            { &((*out)[0]), &((*out)[5]), &((*out)[10]),
                                &((*out)[15]) },
                            cache_[0].get());
  if (pos.word == 1 || pos.word == 6 || pos.word == 11 || pos.word == 12)
      return layers[1].Update( { &((*in)[1]), &((*in)[6]), &((*in)[11]),
                                  &((*in)[12]) },
                              { &((*out)[1]), &((*out)[6]), &((*out)[11]),
                                  &((*out)[12]) },
                              cache_[0].get());
  if (pos.word == 2 || pos.word == 7 || pos.word == 8 || pos.word == 13)
      return layers[2].Update( { &((*in)[2]), &((*in)[7]), &((*in)[8]),
                                  &((*in)[13]) },
                              { &((*out)[2]), &((*out)[7]), &((*out)[8]),
                                  &((*out)[13]) },
                              cache_[0].get());

  if (pos.word == 3 || pos.word == 4 || pos.word == 9 || pos.word == 14)
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
  InitSboxes();
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(0x1000));
}

HamsiSboxLayer::HamsiSboxLayer(StateMask *in, StateMask *out)
    : SboxLayer<4, 128>(in, out) {
  InitSboxes();
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(0x1000));
}

void HamsiSboxLayer::InitSboxes(){
  std::shared_ptr<LinearDistributionTable<4>> ldt(new LinearDistributionTable<4>(HamsiSbox));
    for (int i = 0; i < 128; i++)
      sboxes[i].Initialize(ldt);
}

HamsiSboxLayer* HamsiSboxLayer::clone(){
  //TODO: write copy constructor
  HamsiSboxLayer* obj = new HamsiSboxLayer(in,out);
  obj->sboxes = this->sboxes;
  return obj;
}


bool HamsiSboxLayer::Update(UpdatePos pos) {
//  assert(pos.bit < 32);
//  assert(pos.word < 16);
//
//  if (pos.word == 0 || pos.word == 5 || pos.word == 10 || pos.word == 15)
//    pos.word = 0;
//  else {
//    if (pos.word == 1 || pos.word == 6 || pos.word == 11 || pos.word == 12)
//      pos.word = 1;
//    else {
//      if (pos.word == 2 || pos.word == 7 || pos.word == 8 || pos.word == 13)
//        pos.word = 2;
//      else {
//        if (pos.word == 3 || pos.word == 4 || pos.word == 9 || pos.word == 14)
//          pos.word = 3;
//      }
//    }
//  }

  assert(pos.bit < 128);

  bool ret_val;
  Mask copyin(GetVerticalMask(pos.bit, *in));
  Mask copyout(GetVerticalMask(pos.bit, *out));
 ret_val = sboxes[pos.bit].Update(copyin, copyout, cache_.get());
  SetVerticalMask(pos.bit, *in, copyin);
  SetVerticalMask(pos.bit, *out, copyout);
  return ret_val;
}

Mask HamsiSboxLayer::GetVerticalMask(int b, const StateMask& s) const {
  return Mask(
      { s[b/32].bitmasks[b%32], s[b/32 + 4].bitmasks[b%32], s[b/32 + 8].bitmasks[b%32], s[b/32 +12].bitmasks[b%32]});
}

void HamsiSboxLayer::SetVerticalMask(int b, StateMask& s, const Mask& mask) {
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

