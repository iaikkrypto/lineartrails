#include "icepole.h"

//#define HEXOUTPUT
//#define LATEX

IcepoleState::IcepoleState()
    : StateMask() {
}

IcepoleState* IcepoleState::clone() {
  IcepoleState* obj = new IcepoleState();
  for (size_t j = 0; j < words_.size(); ++j) {
    obj->words_[j] = words_[j];
    obj->changes_for_linear_layer_[j] = changes_for_linear_layer_[j];
    obj->changes_for_sbox_layer_[j] = changes_for_sbox_layer_[j];
  }
  return obj;
}

void IcepoleState::print(std::ostream& stream) {
  stream << *this;
}

#ifdef HEXOUTPUT
#ifdef LATEX
std::ostream& operator<<(std::ostream& stream, const IcepoleState& statemask) {

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
//  stream << " \\\\ "<< std::endl;
  return stream;
}
#else
std::ostream& operator<<(std::ostream& stream, const IcepoleState& statemask) {

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
std::ostream& operator<<(std::ostream& stream, const IcepoleState& statemask) {
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

std::array<BitVector, 20> IcepoleLinear(std::array<BitVector, 20> in) {
BitVector tmp[20];

//the code used in this function is taken and modified from the reference version of ICEPOLE available at
//http://bench.cr.yp.to/ebash.html

tmp[0*5+0] = in[0*5+4] ^ in[1*5+0] ^ in[2*5+0] ^ in[3*5+0];
tmp[0*5+1] = in[0*5+0] ^ in[1*5+1] ^ in[2*5+1] ^ in[3*5+1];
tmp[0*5+2] = in[0*5+4] ^ in[0*5+1] ^ in[1*5+2] ^ in[2*5+2] ^ in[3*5+2];
tmp[0*5+3] = in[0*5+2] ^ in[1*5+3] ^ in[2*5+3] ^ in[3*5+3];
tmp[0*5+4] = in[0*5+3] ^ in[1*5+4] ^ in[2*5+4] ^ in[3*5+4];

tmp[1*5+0] = in[0*5+0] ^ in[1*5+0] ^ in[2*5+1] ^ in[3*5+4];
tmp[1*5+1] = in[0*5+1] ^ in[1*5+1] ^ in[2*5+2] ^ in[2*5+0] ^ in[3*5+0];
tmp[1*5+2] = in[0*5+2] ^ in[1*5+2] ^ in[2*5+3] ^ in[3*5+4] ^ in[3*5+1];
tmp[1*5+3] = in[0*5+3] ^ in[1*5+3] ^ in[2*5+4] ^ in[3*5+2];
tmp[1*5+4] = in[0*5+4] ^ in[1*5+4] ^ in[2*5+0] ^ in[3*5+3];

tmp[2*5+0] = in[0*5+0] ^ in[1*5+4] ^ in[2*5+0] ^ in[3*5+1];
tmp[2*5+1] = in[0*5+1] ^ in[1*5+0] ^ in[2*5+1] ^ in[3*5+2] ^ in[3*5+0];
tmp[2*5+2] = in[0*5+2] ^ in[1*5+4] ^ in[1*5+1] ^ in[2*5+2] ^ in[3*5+3];
tmp[2*5+3] = in[0*5+3] ^ in[1*5+2] ^ in[2*5+3] ^ in[3*5+4];
tmp[2*5+4] = in[0*5+4] ^ in[1*5+3] ^ in[2*5+4] ^ in[3*5+0];

tmp[3*5+0] = in[0*5+0] ^ in[1*5+1] ^ in[2*5+4] ^ in[3*5+0];
tmp[3*5+1] = in[0*5+1] ^ in[1*5+2] ^ in[1*5+0] ^ in[2*5+0] ^ in[3*5+1];
tmp[3*5+2] = in[0*5+2] ^ in[1*5+3] ^ in[2*5+4] ^ in[2*5+1] ^ in[3*5+2];
tmp[3*5+3] = in[0*5+3] ^ in[1*5+4] ^ in[2*5+2] ^ in[3*5+3];
tmp[3*5+4] = in[0*5+4] ^ in[1*5+0] ^ in[2*5+3] ^ in[3*5+4];

tmp[0*5+1] = ROTL(tmp[0*5+1], 36);
tmp[0*5+2] = ROTL(tmp[0*5+2],  3);
tmp[0*5+3] = ROTL(tmp[0*5+3], 41);
tmp[0*5+4] = ROTL(tmp[0*5+4], 18);

tmp[1*5+0] = ROTL(tmp[1*5+0],  1);
tmp[1*5+1] = ROTL(tmp[1*5+1], 44);
tmp[1*5+2] = ROTL(tmp[1*5+2], 10);
tmp[1*5+3] = ROTL(tmp[1*5+3], 45);
tmp[1*5+4] = ROTL(tmp[1*5+4],  2);

tmp[2*5+0] = ROTL(tmp[2*5+0], 62);
tmp[2*5+1] = ROTL(tmp[2*5+1],  6);
tmp[2*5+2] = ROTL(tmp[2*5+2], 43);
tmp[2*5+3] = ROTL(tmp[2*5+3], 15);
tmp[2*5+4] = ROTL(tmp[2*5+4], 61);

tmp[3*5+0] = ROTL(tmp[3*5+0], 28);
tmp[3*5+1] = ROTL(tmp[3*5+1], 55);
tmp[3*5+2] = ROTL(tmp[3*5+2], 25);
tmp[3*5+3] = ROTL(tmp[3*5+3], 21);
tmp[3*5+4] = ROTL(tmp[3*5+4], 56);

unsigned int x,y;
for(x = 0; x < 4; ++x) {
    for (y = 0; y < 5; ++y) {
        unsigned int xx = (x + y) % 4;
        unsigned int yy = (xx + y + 1) % 5;
        in[xx*5+yy] = tmp[x*5+y];
    }
}

   return in;
}

IcepoleLinearLayer& IcepoleLinearLayer::operator=(const IcepoleLinearLayer& rhs) {
  icepole_linear_ = rhs.icepole_linear_;
  return *this;
}

IcepoleLinearLayer::IcepoleLinearLayer() {
  Init();
}

unsigned int IcepoleLinearLayer::GetNumSteps() {
  return linear_steps_;
}

IcepoleLinearLayer* IcepoleLinearLayer::clone() {
  //TODO: write copy constructor
  IcepoleLinearLayer* obj = new IcepoleLinearLayer(in, out);
  obj->icepole_linear_ = this->icepole_linear_;
  return obj;
}

IcepoleLinearLayer::IcepoleLinearLayer(StateMaskBase *in, StateMaskBase *out)
    : LinearLayer(in, out) {
  Init();
}

void IcepoleLinearLayer::Init() {
  icepole_linear_[0].Initialize(IcepoleLinear);

}

bool IcepoleLinearLayer::updateStep(unsigned int step_pos) {
assert(step_pos <= linear_steps_);
bool ret_val;
ret_val = icepole_linear_[step_pos].Update( {
        &((*in)[0]), &((*in)[1]), &((*in)[2]), &((*in)[3]), &((*in)[4]),
        &((*in)[5]), &((*in)[6]), &((*in)[7]), &((*in)[8]), &((*in)[9]),
        &((*in)[10]), &((*in)[11]), &((*in)[12]), &((*in)[13]), &((*in)[14]),
        &((*in)[15]), &((*in)[16]), &((*in)[17]), &((*in)[18]), &((*in)[19])},
       {&((*out)[0]), &((*out)[1]), &((*out)[2]), &((*out)[3]), &((*out)[4]),
        &((*out)[5]), &((*out)[6]), &((*out)[7]), &((*out)[8]), &((*out)[9]),
        &((*out)[10]), &((*out)[11]), &((*out)[12]), &((*out)[13]), &((*out)[14]),
        &((*out)[15]), &((*out)[16]), &((*out)[17]), &((*out)[18]), &((*out)[19])});

for(int i = 0; i < 20; ++i){
  in->getWordSbox(i) |= (*in)[i].changes_;
  out->getWordSbox(i) |= (*out)[i].changes_;
}
return ret_val;
}

void IcepoleLinearLayer::copyValues(LinearLayer* other){
  IcepoleLinearLayer* ptr = dynamic_cast<IcepoleLinearLayer*> (other);
  icepole_linear_ = ptr->icepole_linear_;
}

//-----------------------------------------------------------------------------

BitVector IcepoleSbox(BitVector in) {
  static const BitVector sbox[32] = {
      31, 5, 10, 11, 20, 17, 22, 23, 9, 12, 3, 2, 13, 8, 15, 14, 18, 21,
      24, 27, 6, 1, 4, 7, 26, 29, 16, 19, 30, 25, 28, 0};
  return sbox[in % 32] & 0x1f;
}

std::unique_ptr<LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>> IcepoleSboxLayer::cache_;
std::shared_ptr<LinearDistributionTable<5>> IcepoleSboxLayer::ldt_;

IcepoleSboxLayer& IcepoleSboxLayer::operator=(const IcepoleSboxLayer& rhs) {
  sboxes = rhs.sboxes;
  return *this;
}

IcepoleSboxLayer::IcepoleSboxLayer() {
  if(ldt_ == nullptr)
    ldt_.reset(new LinearDistributionTable<5>(IcepoleSbox));
  InitSboxes(ldt_);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(
            cache_size_));
}

IcepoleSboxLayer::IcepoleSboxLayer(StateMaskBase *in, StateMaskBase *out)
    : SboxLayer(in, out) {
  if(ldt_ == nullptr)
    ldt_.reset(new LinearDistributionTable<5>(IcepoleSbox));
  InitSboxes(ldt_);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(
            cache_size_));
}

IcepoleSboxLayer* IcepoleSboxLayer::clone() {
  //TODO: write copy constructor
  IcepoleSboxLayer* obj = new IcepoleSboxLayer(in, out);
  obj->sboxes = this->sboxes;
  return obj;
}

bool IcepoleSboxLayer::updateStep(unsigned int step_pos) {
  assert(step_pos < sboxes.size());
  bool ret_val;
  Mask copyin(GetVerticalMask(step_pos, *in));
  Mask copyout(GetVerticalMask(step_pos, *out));
  ret_val = sboxes[step_pos].Update(copyin, copyout, cache_.get());
  SetVerticalMask(step_pos, *in, copyin);
  SetVerticalMask(step_pos, *out, copyout);
  return ret_val;
}

Mask IcepoleSboxLayer::GetVerticalMask(unsigned int b,
                                     const StateMaskBase& s) const {
  return Mask(
      { s[((b/64)*5)+4].bitmasks[b%64], s[((b/64)*5)+3].bitmasks[b%64], s[((b/64)*5)+2].bitmasks[b%64], s[((b/64)*5)+1].bitmasks[b%64],
          s[((b/64)*5)+0].bitmasks[b%64] });
}

void IcepoleSboxLayer::SetVerticalMask(unsigned int b, StateMaskBase& s,
                                     const Mask& mask) {

  s.getWordLinear(((b/64)*5)+0) |= ((mask.changes_>>(4))&1) << b%64;
  s.getWordLinear(((b/64)*5)+1) |= ((mask.changes_>>(3))&1) << b%64;
  s.getWordLinear(((b/64)*5)+2) |= ((mask.changes_>>(2))&1) << b%64;
  s.getWordLinear(((b/64)*5)+3) |= ((mask.changes_>>(1))&1) << b%64;
  s.getWordLinear(((b/64)*5)+4) |= ((mask.changes_>>(0))&1) << b%64;

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

