/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
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
  bitsize_ = rhs.bitsize_;
  changes_ = rhs.changes_;
  return *this;
}

Mask::Mask() : caremask(64), bitsize_(64), changes_(~0ULL) {
}

Mask::Mask(unsigned bitsize) : caremask(bitsize), bitsize_(bitsize), changes_(~0ULL) {
    init_bitmasks();
}

Mask::Mask(const Mask& other) : bitmasks(other.bitmasks), caremask(other.caremask), bitsize_(other.bitsize_), changes_(~0ULL) {
}

Mask::Mask(std::initializer_list<char> other) : bitmasks(other), caremask(other.size()), bitsize_(other.size()), changes_(~0ULL) {
  init_caremask();
}

Mask::Mask(WordMask& other) : bitmasks(other), caremask(other.size()), bitsize_(other.size()), changes_(~0ULL) {
  init_caremask();
}


void Mask::set_bit(BitMask bit, const int index){
  assert(bit <= 3 && bit >=0 && index >= 0 && index < 64);
  bitmasks[index] = bit;
  changes_ |= 1 << index;
  BitVector hole = ~0ULL - 1;
  hole = (hole << index) | (hole >> (64-index));
  caremask.canbe1 &= hole;
  caremask.care &= hole;
  caremask.canbe1 |= ((BitVector)(bit & 1)) << index;
  caremask.care   |= (((BitVector)(bit != BM_DUNNO)) << index);

}

void Mask::reinit_caremask(){
  BitVector canbe1 = 0;
  BitVector care  = 0;
  for (unsigned i = 0; i < bitmasks.size(); ++i) {
    canbe1 |= ((((BitVector)(bitmasks[i] != BM_0)) << i));
    care   |= (((BitVector)(bitmasks[i] != BM_DUNNO)) << i);
  }
  changes_ = caremask.canbe1 ^ canbe1;
  changes_ |= caremask.care ^ care;
  caremask.canbe1 = canbe1;
  caremask.care = care;
}

void Mask::init_caremask() {
  caremask.Reset();
  for (unsigned i = 0; i < bitmasks.size(); ++i) {
    caremask.canbe1 &= (~0ULL ^ (((BitVector)(bitmasks[i] == BM_0)) << i));
    caremask.care   |= (((BitVector)(bitmasks[i] != BM_DUNNO)) << i);
  }
}

void Mask::init_bitmasks() {
  bitmasks.resize(bitsize_);
  for(auto& bitmask : bitmasks )
    bitmask = BM_DUNNO;
}

void Mask::reinit_bitmasks() {
  BitVector canbe1 = caremask.canbe1;
  BitVector care   = caremask.care;

  int i = 0;

  changes_ = 0;

  while (canbe1 | care) {
    unsigned char bitval = ((canbe1 & 1) | ((!(canbe1 & care & 1)) << 1));
    changes_ |= (( (BitVector) (bitmasks[i] != bitval))&1) << i;
    bitmasks[i++] = bitval;
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
