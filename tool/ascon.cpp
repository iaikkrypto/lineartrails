#include "ascon.h"

#define TERMINALCOLORS

AsconState::AsconState() : words{{Mask(64), Mask(64), Mask(64), Mask(64), Mask(64)}} {
}

AsconState& AsconState::operator=(const AsconState& rhs)
{
  for(int j = 0; j< 5; ++j)
     words[j] = rhs.words[j];
  return *this;
}


AsconState*  AsconState::clone(){
  AsconState* obj =  new AsconState();
  for(int j = 0; j< 5; ++j)
    obj->words[j] = words[j];
  return obj;
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

void AsconState::SetBit(BitMask value, int word_pos, int bit_pos){
  words.at(word_pos).set_bit(value, bit_pos);
}

void AsconState::print(){
  std::cout << *this;
}


std::ostream& operator<<(std::ostream& stream, const AsconState& statemask) {
#ifndef TERMINALCOLORS
  char symbol[4] {'#', '1', '0', '?'};
#else
  std::string symbol[4] {"\033[1;35m#\033[0m", "\033[1;31m1\033[0m", "0", "\033[1;33m?\033[0m"};
#endif
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

AsconLinearLayer::AsconLinearLayer() {
  Init();
}

AsconLinearLayer* AsconLinearLayer::clone(){
  //TODO: write copy constructor
  AsconLinearLayer* obj = new AsconLinearLayer(in,out);
  obj->sigmas = this->sigmas;
  return obj;
}

AsconLinearLayer::AsconLinearLayer(StateMask *in, StateMask *out) : LinearLayer(in, out) {
  Init();
}

void AsconLinearLayer::Init(){
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

AsconSboxLayer::AsconSboxLayer(){
  InitSboxes();
}

AsconSboxLayer::AsconSboxLayer(StateMask *in, StateMask *out) : SboxLayer<5,64>(in, out) {
 InitSboxes();
}

void AsconSboxLayer::InitSboxes(){
  std::shared_ptr<LinearDistributionTable<5>> ldt(new LinearDistributionTable<5>(AsconSbox));
    for (int i = 0; i < 64; i++)
      sboxes[i].Initialize(ldt);
}

AsconSboxLayer* AsconSboxLayer::clone(){
  //TODO: write copy constructor
  AsconSboxLayer* obj = new AsconSboxLayer(in,out);
  obj->sboxes = this->sboxes;
  return obj;
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

