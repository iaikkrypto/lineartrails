#include "mask.h" 

  WordMaskCare& WordMaskCare::operator=(const WordMaskCare& rhs)
  {
    canbe1 = rhs.canbe1;
    care = rhs.care;
    return *this;
  }

WordMaskCare::WordMaskCare(unsigned bitsize) : canbe1(~0ULL >> (64 - bitsize)), care(0) {
}

WordMaskCare::WordMaskCare(const WordMaskCare& other) : canbe1(other.canbe1), care(other.care) {
}

WordMaskCare::WordMaskCare(BitVector canbe1, BitVector care) : canbe1(canbe1), care(care) {
}

void WordMaskCare::Reset() {
  canbe1 |= care;
  canbe1 |= (canbe1 >> 1);
  canbe1 |= (canbe1 >> 2);
  canbe1 |= (canbe1 >> 4);
  canbe1 |= (canbe1 >> 8);
  canbe1 |= (canbe1 >> 16);
  canbe1 |= (canbe1 >> 32);
  care = 0;
}

//-----------------------------------------------------------------------------
Mask& Mask::operator=(const Mask& rhs)
{
  bitmasks = rhs.bitmasks;
  caremask = rhs.caremask;
  return *this;
}

Mask::Mask() : caremask(64), bitsize_(64) {
}

Mask::Mask(unsigned bitsize) : caremask(bitsize), bitsize_(bitsize) {
    init_bitmasks();
}

Mask::Mask(const Mask& other) : bitmasks(other.bitmasks), caremask(other.caremask), bitsize_(other.bitsize_) {
}

Mask::Mask(std::initializer_list<char> other) : bitmasks(other), caremask(other.size()), bitsize_(other.size()) {
  init_caremask();
}

Mask::Mask(WordMask& other) : bitmasks(other), caremask(other.size()), bitsize_(other.size()) {
  init_caremask();
}

//Mask::Mask(WordMaskCare& other) : caremask(other) {
//  init_bitmasks();
//}

void Mask::set_bit(BitMask bit, const int index){
  assert(bit <= 3 && bit >=0 && index >= 0 && index < 64);
  bitmasks[index] = bit;
  BitVector hole = ~0ULL - 1;
  hole = (hole << index) | (hole >> (64-index));
  caremask.canbe1 &= hole;
  caremask.care &= hole;
  caremask.canbe1 |= ((BitVector)(bit & 1)) << index;
  caremask.care   |= (((BitVector)(bit != BM_DUNNO)) << index);

}

void Mask::reinit_caremask(){
  caremask.canbe1 = 0;
  caremask.care  = 0;
  for (unsigned i = 0; i < bitmasks.size(); ++i) {
    caremask.canbe1 |= ((((BitVector)(bitmasks[i] != BM_0)) << i));
    caremask.care   |= (((BitVector)(bitmasks[i] != BM_DUNNO)) << i);
  }
}

void Mask::init_caremask() {
  caremask.Reset();
  for (unsigned i = 0; i < bitmasks.size(); ++i) {
    caremask.canbe1 &= (~0ULL ^ (((BitVector)(bitmasks[i] == BM_0)) << i));
    caremask.care   |= (((BitVector)(bitmasks[i] != BM_DUNNO)) << i);
  }
//  std::cout << std::hex << "canbe1: " << caremask.canbe1 << std::endl;
//  std::cout << "care: " << caremask.care << std::dec << std::endl;
}

void Mask::init_bitmasks() {
  bitmasks.resize(bitsize_);
  for(auto& bitmask : bitmasks )
    bitmask = BM_DUNNO;
}

void Mask::reinit_bitmasks() {
  BitVector canbe1 = caremask.canbe1;
  BitVector care   = caremask.care;
  bitmasks.clear();
  if ((canbe1 | care) >> 32)
    bitmasks.reserve(64);
  else if ((canbe1 | care) >> 56)
    bitmasks.reserve(32);
  else
    bitmasks.reserve(8);

  while (canbe1 | care) {
    bitmasks.push_back((canbe1 & 1) | ((!(canbe1 & care & 1)) << 1));
    canbe1 >>= 1;
    care   >>= 1;
  }

}

void Mask::reset(int bitsize) {
  bitsize_ = bitsize;
  caremask.canbe1 = ~0ULL >> (64 - bitsize);
  caremask.care = 0;
    init_bitmasks();
}

std::ostream& operator<<(std::ostream& stream, const Mask& mask) {
  char symbol[4] {'#', '1', '0', '?'};
  for (BitMask m : mask.bitmasks)
    stream << symbol[m % 4];
  return stream;
}
