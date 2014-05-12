#include "prost256.h"

//#define HEXOUTPUT
//#define LATEX

Prost256State::Prost256State()
    : StateMask() {
}

Prost256State* Prost256State::clone() {
  Prost256State* obj = new Prost256State();
  for (size_t j = 0; j < words_.size(); ++j) {
    obj->words_[j] = words_[j];
    obj->changes_for_linear_layer_[j] = changes_for_linear_layer_[j];
    obj->changes_for_sbox_layer_[j] = changes_for_sbox_layer_[j];
  }
  return obj;
}

void Prost256State::print(std::ostream& stream) {
  stream << *this;
}

#ifdef HEXOUTPUT
#ifdef LATEX
std::ostream& operator<<(std::ostream& stream, const Prost256State& statemask) {

  for (size_t i = 0; i < statemask.words_.size(); ++i) {
    unsigned long long outword = 0;
    for (auto it = statemask.words_[i].bitmasks.rbegin(); it != statemask.words_[i].bitmasks.rend(); ++it) {
      outword <<= 1;
      if (*it % 4 == 1)
        outword |=1;
    }
    stream << " & \\texttt{" << std::hex << std::setfill('0') << std::setw(8) << outword << std::dec << "}";
    if(i%4 == 3)
     stream << " \\\\ "<< std::endl;
  }
//  stream << " \\\\ "<< std::endl;
  return stream;
}
#else
std::ostream& operator<<(std::ostream& stream, const Prost256State& statemask) {

  for (size_t i = 0; i < statemask.words_.size(); ++i) {
    unsigned long long outword = 0;
    for (auto it = statemask.words_[i].bitmasks.rbegin(); it != statemask.words_[i].bitmasks.rend(); ++it) {
      outword <<= 1;
      if (*it % 4 == 1)
        outword |=1;
    }
    stream << std::hex << std::setfill('0') << std::setw(8) << outword << std::dec << "\t";
    if(i%4 == 3)
     stream << std::endl;
  }
  return stream;
}
#endif
#else
std::ostream& operator<<(std::ostream& stream, const Prost256State& statemask) {
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

BitVector Prost256Sbox(BitVector in) {
  static const BitVector sbox[16] = {
      0,4,8,0xf,1,5,0xe,9,2,7,0xa,0xc,0xb,0xd,6,3};
  return sbox[in % 16] & 0xf;
}

std::unique_ptr<LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>> Prost256SboxLayer::cache_;
std::shared_ptr<LinearDistributionTable<4>> Prost256SboxLayer::ldt_;

Prost256SboxLayer& Prost256SboxLayer::operator=(const Prost256SboxLayer& rhs) {
  sboxes = rhs.sboxes;
  return *this;
}

Prost256SboxLayer::Prost256SboxLayer() {
  if(ldt_ == nullptr)
    ldt_.reset(new LinearDistributionTable<4>(Prost256Sbox));
  InitSboxes(ldt_);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(
            cache_size_));
}

Prost256SboxLayer::Prost256SboxLayer(StateMaskBase *in, StateMaskBase *out)
    : SboxLayer(in, out) {
  if(ldt_ == nullptr)
    ldt_.reset(new LinearDistributionTable<4>(Prost256Sbox));
  InitSboxes(ldt_);
  if (this->cache_.get() == nullptr)
    this->cache_.reset(
        new LRU_Cache<unsigned long long, NonlinearStepUpdateInfo>(
            cache_size_));
}

Prost256SboxLayer* Prost256SboxLayer::clone() {
  //TODO: write copy constructor
  Prost256SboxLayer* obj = new Prost256SboxLayer(in, out);
  obj->sboxes = this->sboxes;
  return obj;
}

bool Prost256SboxLayer::updateStep(unsigned int step_pos) {
  assert(step_pos < sboxes.size());
  bool ret_val;
  Mask copyin(GetVerticalMask(step_pos, *in));
  Mask copyout(GetVerticalMask(step_pos, *out));
  ret_val = sboxes[step_pos].Update(copyin, copyout, cache_.get());
  SetVerticalMask(step_pos, *in, copyin);
  SetVerticalMask(step_pos, *out, copyout);
  return ret_val;
}

Mask Prost256SboxLayer::GetVerticalMask(unsigned int b,
                                     const StateMaskBase& s) const {
  return Mask(
      { s[((b/32)*4)+3].bitmasks[b%32], s[((b/32)*4)+2].bitmasks[b%32], s[((b/32)*4)+1].bitmasks[b%32],
          s[((b/32)*4)+0].bitmasks[b%32] });
}

void Prost256SboxLayer::SetVerticalMask(unsigned int b, StateMaskBase& s,
                                     const Mask& mask) {

//  TODO: Test faster update
//  s.getWordLinear(((b/32)*4)+0) |= ((mask.changes_>>(3))&1) << b%32;
//  s.getWordLinear(((b/32)*4)+1) |= ((mask.changes_>>(2))&1) << b%32;
//  s.getWordLinear(((b/32)*4)+2) |= ((mask.changes_>>(1))&1) << b%32;
//  s.getWordLinear(((b/32)*4)+3) |= ((mask.changes_>>(0))&1) << b%32;

  s[((b/32)*4)+0].bitmasks[b%32] = mask.bitmasks[3];
  s[((b/32)*4)+1].bitmasks[b%32] = mask.bitmasks[2];
  s[((b/32)*4)+2].bitmasks[b%32] = mask.bitmasks[1];
  s[((b/32)*4)+3].bitmasks[b%32] = mask.bitmasks[0];
  BitVector m = ~(1ULL << (b%32));
  s[((b/32)*4)+0].caremask.canbe1 = (s[((b/32)*4)+0].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 3) & 1) << (b%32));
  s[((b/32)*4)+1].caremask.canbe1 = (s[((b/32)*4)+1].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 2) & 1) << (b%32));
  s[((b/32)*4)+2].caremask.canbe1 = (s[((b/32)*4)+2].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 1) & 1) << (b%32));
  s[((b/32)*4)+3].caremask.canbe1 = (s[((b/32)*4)+3].caremask.canbe1 & m) | (((mask.caremask.canbe1 >> 0) & 1) << (b%32));

  s[((b/32)*4)+0].caremask.care = (s[((b/32)*4)+0].caremask.care & m) | (((mask.caremask.care >> 3) & 1) << (b%32));
  s[((b/32)*4)+1].caremask.care = (s[((b/32)*4)+1].caremask.care & m) | (((mask.caremask.care >> 2) & 1) << (b%32));
  s[((b/32)*4)+2].caremask.care = (s[((b/32)*4)+2].caremask.care & m) | (((mask.caremask.care >> 1) & 1) << (b%32));
  s[((b/32)*4)+3].caremask.care = (s[((b/32)*4)+3].caremask.care & m) | (((mask.caremask.care >> 0) & 1) << (b%32));
}

