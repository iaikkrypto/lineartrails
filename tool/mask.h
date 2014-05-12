#ifndef MASK_H_
#define MASK_H_

#include <vector>
#include <iostream>

#define BM_DUNNO  3
#define BM_0      2
#define BM_1      1
#define BM_CONTRA 0

typedef char BitMask;                  // mask for 1 bit (10=0, 01=1, 11=?)
typedef std::vector<BitMask> WordMask;
typedef uint64_t BitVector;            // n-bit vector

struct WordMaskCare {
  WordMaskCare(unsigned bitsize);
  WordMaskCare(const WordMaskCare& other);
  void Reset();

  BitVector canbe1;                    // 1=canbe1, 0=mustbe1 // LSB = WordMask[0]!!!
  BitVector care;                      // 1=0/1, 0=?)
};

struct Mask {
  Mask(unsigned bitsize);
  Mask(const Mask& other);
  Mask(std::initializer_list<char> other);
  Mask(WordMask& other);
  Mask(WordMaskCare& other);
  void init_caremask();
  void init_bitmasks();

  friend std::ostream& operator<<(std::ostream& stream, const Mask& mask);

  WordMask bitmasks; // maybe don't store this & use just for initialization
  WordMaskCare caremask;
};

#endif // MASK_H_
