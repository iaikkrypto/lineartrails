#include "mask.h" 

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

Mask::Mask(unsigned bitsize) : caremask(bitsize) {
  init_bitmasks();
}

Mask::Mask(const Mask& other) : bitmasks(other.bitmasks), caremask(other.caremask) {
}

Mask::Mask(std::initializer_list<char> other) : bitmasks(other), caremask(other.size()) {
  init_caremask();
}

Mask::Mask(WordMask& other) : bitmasks(other), caremask(other.size()) {
  init_caremask();
}

Mask::Mask(WordMaskCare& other) : caremask(other) {
  init_bitmasks();
}

void Mask::init_caremask() {
  caremask.Reset();
  for (unsigned i = 0; i < bitmasks.size(); ++i) {
    caremask.canbe1 &= (~0 ^ (((BitVector)(bitmasks[i] == BM_0)) << i));
    caremask.care   |= (((BitVector)(bitmasks[i] != BM_DUNNO)) << i);
  }
}

void Mask::init_bitmasks() {
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

std::ostream& operator<<(std::ostream& stream, const Mask& mask) {
  char symbol[4] {'#', '1', '0', '?'};
  for (BitMask m : mask.bitmasks)
    stream << symbol[m % 4];
  return stream;
}
