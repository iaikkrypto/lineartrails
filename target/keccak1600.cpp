#include "keccak1600.h"

//#define HEXOUTPUT
//#define LATEX

Keccak1600State::Keccak1600State()
    : StateMask() {
}

Keccak1600State* Keccak1600State::clone() {
  Keccak1600State* obj = new Keccak1600State();
  for (size_t j = 0; j < words_.size(); ++j) {
    obj->words_[j] = words_[j];
    obj->changes_for_linear_layer_[j] = changes_for_linear_layer_[j];
    obj->changes_for_sbox_layer_[j] = changes_for_sbox_layer_[j];
  }
  return obj;
}

void Keccak1600State::print(std::ostream& stream) {
  stream << *this;
}

#ifdef HEXOUTPUT
#ifdef LATEX
std::ostream& operator<<(std::ostream& stream, const Keccak1600State& statemask) {

  for (size_t i = 0; i < statemask.words_.size(); ++i) {
    unsigned long long outword = 0;
    for (auto it = statemask.words_[i].bitmasks.rbegin(); it != statemask.words_[i].bitmasks.rend(); ++it) {
      outword <<= 1;
      if (*it % 4 == 1)
        outword |=1;
    }
    stream << " & \\texttt{" << std::hex << std::setfill('0') << std::setw(16) << outword << std::dec << "}";
    if(i%5 == 4)
     stream << " \\\\ "<< std::endl;
  }
  stream << " \\\\ "<< std::endl;
  return stream;
}
#else
std::ostream& operator<<(std::ostream& stream, const Keccak1600State& statemask) {

  for (size_t i = 0; i < statemask.words_.size(); ++i) {
    unsigned long long outword = 0;
    for (auto it = statemask.words_[i].bitmasks.rbegin(); it != statemask.words_[i].bitmasks.rend(); ++it) {
      outword <<= 1;
      if (*it % 4 == 1)
        outword |=1;
    }
    stream << std::hex << std::setfill('0') << std::setw(16) << outword << std::dec << "\t";
    if(i%5 == 4)
     stream << std::endl;
  }
  return stream;
}
#endif
#else
std::ostream& operator<<(std::ostream& stream, const Keccak1600State& statemask) {
#ifndef TERMINALCOLORS
  char symbol[4] {'#', '1', '0', '?'};
#else
  std::string symbol[4] { "\033[1;35m#\033[0m", "\033[1;31m1\033[0m", "0",
      "\033[1;33m?\033[0m" };
#endif
  for (Mask word : statemask.words_) {
    for (auto it = word.bitmasks.rbegin(); it != word.bitmasks.rend(); ++it) {
      stream << symbol[*it % 4];
    }
    stream << std::endl;
  }
  return stream;
}
#endif

//-----------------------------------------------------------------------------

std::array<BitVector, 25> Keccak1600Linear(std::array<BitVector, 25> in) {
   BitVector t[25];
   BitVector p[5];
   static constexpr int R[5][5] = {
       {  0, 36,  3, 41, 18 },
       {  1, 44, 10, 45,  2 },
       { 62,  6, 43, 15, 61 },
       { 28, 55, 25, 21, 56 },
       { 27, 20, 39,  8, 14 } };
   // parity of each lane
   p[0] = in[0] ^ in[5] ^ in[10] ^ in[15] ^ in[20];
   p[1] = in[1] ^ in[6] ^ in[11] ^ in[16] ^ in[21];
   p[2] = in[2] ^ in[7] ^ in[12] ^ in[17] ^ in[22];
   p[3] = in[3] ^ in[8] ^ in[13] ^ in[18] ^ in[23];
   p[4] = in[4] ^ in[9] ^ in[14] ^ in[19] ^ in[24];
   // theta w/o parity
   t[0] = in[0] ^ p[4] ^ ROTL(p[1], 1);
   t[1] = in[1] ^ p[0] ^ ROTL(p[2], 1);
   t[2] = in[2] ^ p[1] ^ ROTL(p[3], 1);
   t[3] = in[3] ^ p[2] ^ ROTL(p[4], 1);
   t[4] = in[4] ^ p[3] ^ ROTL(p[0], 1);
   t[5] = in[5] ^ p[4] ^ ROTL(p[1], 1);
   t[6] = in[6] ^ p[0] ^ ROTL(p[2], 1);
   t[7] = in[7] ^ p[1] ^ ROTL(p[3], 1);
   t[8] = in[8] ^ p[2] ^ ROTL(p[4], 1);
   t[9] = in[9] ^ p[3] ^ ROTL(p[0], 1);
   t[10] = in[10] ^ p[4] ^ ROTL(p[1], 1);
   t[11] = in[11] ^ p[0] ^ ROTL(p[2], 1);
   t[12] = in[12] ^ p[1] ^ ROTL(p[3], 1);
   t[13] = in[13] ^ p[2] ^ ROTL(p[4], 1);
   t[14] = in[14] ^ p[3] ^ ROTL(p[0], 1);
   t[15] = in[15] ^ p[4] ^ ROTL(p[1], 1);
   t[16] = in[16] ^ p[0] ^ ROTL(p[2], 1);
   t[17] = in[17] ^ p[1] ^ ROTL(p[3], 1);
   t[18] = in[18] ^ p[2] ^ ROTL(p[4], 1);
   t[19] = in[19] ^ p[3] ^ ROTL(p[0], 1);
   t[20] = in[20] ^ p[4] ^ ROTL(p[1], 1);
   t[21] = in[21] ^ p[0] ^ ROTL(p[2], 1);
   t[22] = in[22] ^ p[1] ^ ROTL(p[3], 1);
   t[23] = in[23] ^ p[2] ^ ROTL(p[4], 1);
   t[24] = in[24] ^ p[3] ^ ROTL(p[0], 1);
   // rho, pi
   for (int y = 0; y < 5; y++)
     for (int x = 0; x < 5; x++)
       in[5 * (((2 * x + 3 * y) % 5)) + y] = ROTR(t[x + 5 * y], (64 - R[x][y]) % 64);

   return in;
}

Keccak1600LinearLayer& Keccak1600LinearLayer::operator=(const Keccak1600LinearLayer& rhs) {
  keccak_linear_ = rhs.keccak_linear_;
  return *this;
}

Keccak1600LinearLayer::Keccak1600LinearLayer() {
  Init();
}

unsigned int Keccak1600LinearLayer::GetNumSteps() {
  return linear_steps_;
}

Keccak1600LinearLayer* Keccak1600LinearLayer::clone() {
  //TODO: write copy constructor
  Keccak1600LinearLayer* obj = new Keccak1600LinearLayer(in, out);
  obj->keccak_linear_ = this->keccak_linear_;
  return obj;
}

Keccak1600LinearLayer::Keccak1600LinearLayer(StateMaskBase *in, StateMaskBase *out)
    : LinearLayer(in, out) {
  Init();
}

void Keccak1600LinearLayer::Init() {
  keccak_linear_[0].Initialize(Keccak1600Linear);

}

bool Keccak1600LinearLayer::updateStep(unsigned int step_pos) {
assert(step_pos <= linear_steps_);
bool ret_val;
ret_val = keccak_linear_[step_pos].Update( {
        &((*in)[0]), &((*in)[1]), &((*in)[2]), &((*in)[3]), &((*in)[4]),
        &((*in)[5]), &((*in)[6]), &((*in)[7]), &((*in)[8]), &((*in)[9]),
        &((*in)[10]), &((*in)[11]), &((*in)[12]), &((*in)[13]), &((*in)[14]),
        &((*in)[15]), &((*in)[16]), &((*in)[17]), &((*in)[18]), &((*in)[19]),
        &((*in)[20]), &((*in)[21]), &((*in)[22]), &((*in)[23]), &((*in)[24])},
       {&((*out)[0]), &((*out)[1]), &((*out)[2]), &((*out)[3]), &((*out)[4]),
        &((*out)[5]), &((*out)[6]), &((*out)[7]), &((*out)[8]), &((*out)[9]),
        &((*out)[10]), &((*out)[11]), &((*out)[12]), &((*out)[13]), &((*out)[14]),
        &((*out)[15]), &((*out)[16]), &((*out)[17]), &((*out)[18]), &((*out)[19]),
        &((*out)[20]), &((*out)[21]), &((*out)[22]), &((*out)[23]), &((*out)[24])});

//  TODO: Test faster update
//for(int i = 0; i < 25; ++i){
//  in->getWordSbox(i) |= (*in)[i].changes_;
//  out->getWordSbox(i) |= (*out)[i].changes_;
//}
return ret_val;
}

void Keccak1600LinearLayer::copyValues(LinearLayer* other){
  Keccak1600LinearLayer* ptr = dynamic_cast<Keccak1600LinearLayer*> (other);
  keccak_linear_ = ptr->keccak_linear_;
}

//-----------------------------------------------------------------------------

BitVector Keccak1600Sbox(BitVector in) {
  static const BitVector sbox[32] = {
      0, 5, 10, 11, 20, 17, 22, 23, 9, 12, 3, 2, 13, 8, 15, 14, 18, 21,
      24, 27, 6, 1, 4, 7, 26, 29, 16, 19, 30, 25, 28, 31};
  return sbox[in % 32] & 0x1f;
}

std::unique_ptr<LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>> Keccak1600SboxLayer::cache_;
std::shared_ptr<LinearDistributionTable<5>> Keccak1600SboxLayer::ldt_;

Keccak1600SboxLayer& Keccak1600SboxLayer::operator=(const Keccak1600SboxLayer& rhs) {
  sboxes = rhs.sboxes;
  return *this;
}

Keccak1600SboxLayer::Keccak1600SboxLayer() {
  if(ldt_ == nullptr)
    ldt_.reset(new LinearDistributionTable<5>(Keccak1600Sbox));
  InitSboxes(ldt_);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(
            cache_size_));
}

Keccak1600SboxLayer::Keccak1600SboxLayer(StateMaskBase *in, StateMaskBase *out)
    : SboxLayer(in, out) {
  if(ldt_ == nullptr)
    ldt_.reset(new LinearDistributionTable<5>(Keccak1600Sbox));
  InitSboxes(ldt_);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(
            cache_size_));
}

Keccak1600SboxLayer* Keccak1600SboxLayer::clone() {
  //TODO: write copy constructor
  Keccak1600SboxLayer* obj = new Keccak1600SboxLayer(in, out);
  obj->sboxes = this->sboxes;
  return obj;
}

bool Keccak1600SboxLayer::updateStep(unsigned int step_pos) {
  assert(step_pos < sboxes.size());
  bool ret_val;
  Mask copyin(GetVerticalMask(step_pos, *in));
  Mask copyout(GetVerticalMask(step_pos, *out));
  ret_val = sboxes[step_pos].Update(copyin, copyout, cache_.get());
  SetVerticalMask(step_pos, *in, copyin);
  SetVerticalMask(step_pos, *out, copyout);
  return ret_val;
}

Mask Keccak1600SboxLayer::GetVerticalMask(unsigned int b,
                                     const StateMaskBase& s) const {
  return Mask(
      { s[((b/64)*5)+4].bitmasks[b%64], s[((b/64)*5)+3].bitmasks[b%64], s[((b/64)*5)+2].bitmasks[b%64], s[((b/64)*5)+1].bitmasks[b%64],
          s[((b/64)*5)+0].bitmasks[b%64] });
}

void Keccak1600SboxLayer::SetVerticalMask(unsigned int b, StateMaskBase& s,
                                     const Mask& mask) {

//  TODO: Test faster update
//  s.getWordLinear(((b/64)*5)+0) |= ((mask.changes_>>(4))&1) << b%64;
//  s.getWordLinear(((b/64)*5)+1) |= ((mask.changes_>>(3))&1) << b%64;
//  s.getWordLinear(((b/64)*5)+2) |= ((mask.changes_>>(2))&1) << b%64;
//  s.getWordLinear(((b/64)*5)+3) |= ((mask.changes_>>(1))&1) << b%64;
//  s.getWordLinear(((b/64)*5)+4) |= ((mask.changes_>>(0))&1) << b%64;

  s[((b/64)*5)+0].bitmasks[b%64] = mask.bitmasks[4];
  s[((b/64)*5)+1].bitmasks[b%64] = mask.bitmasks[3];
  s[((b/64)*5)+2].bitmasks[b%64] = mask.bitmasks[2];
  s[((b/64)*5)+3].bitmasks[b%64] = mask.bitmasks[1];
  s[((b/64)*5)+4].bitmasks[b%64] = mask.bitmasks[0];
  BitVector m = ~(1ULL << b);
  s[((b/64)*5)+0].caremask.canbe1 = (s[((b/64)*5)+0].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 4) & 1) << (b%64));
  s[((b/64)*5)+1].caremask.canbe1 = (s[((b/64)*5)+1].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 3) & 1) << (b%64));
  s[((b/64)*5)+2].caremask.canbe1 = (s[((b/64)*5)+2].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 2) & 1) << (b%64));
  s[((b/64)*5)+3].caremask.canbe1 = (s[((b/64)*5)+3].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 1) & 1) << (b%64));
  s[((b/64)*5)+4].caremask.canbe1 = (s[((b/64)*5)+4].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 0) & 1) << (b%64));

  s[((b/64)*5)+0].caremask.care = (s[((b/64)*5)+0].caremask.care & m) | (((mask.caremask.care >> 4) & 1) << (b%64));
  s[((b/64)*5)+1].caremask.care = (s[((b/64)*5)+1].caremask.care & m) | (((mask.caremask.care >> 3) & 1) << (b%64));
  s[((b/64)*5)+2].caremask.care = (s[((b/64)*5)+2].caremask.care & m) | (((mask.caremask.care >> 2) & 1) << (b%64));
  s[((b/64)*5)+3].caremask.care = (s[((b/64)*5)+3].caremask.care & m) | (((mask.caremask.care >> 1) & 1) << (b%64));
  s[((b/64)*5)+4].caremask.care = (s[((b/64)*5)+4].caremask.care & m) | (((mask.caremask.care >> 0) & 1) << (b%64));
}

