#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>


// ==== Masks ====
#define BM_DUNNO  3
#define BM_0      2
#define BM_1      1
#define BM_CONTRA 0

typedef char BitMask;                  // mask for 1 bit (10=0, 01=1, 11=?)
typedef std::vector<BitMask> WordMask;
typedef uint64_t BitVector;            // n-bit vector

struct WordMaskCare {
  WordMaskCare(unsigned bitsize) : canbe1(~0ULL >> (64 - bitsize)), care(0) { }
  WordMaskCare(const WordMaskCare& other) : canbe1(other.canbe1), care(other.care) { }

  void Reset() {
    canbe1 |= care;
    canbe1 |= (canbe1 >> 1);
    canbe1 |= (canbe1 >> 2);
    canbe1 |= (canbe1 >> 4);
    canbe1 |= (canbe1 >> 8);
    canbe1 |= (canbe1 >> 16);
    canbe1 |= (canbe1 >> 32);
    care = 0;
  }

  BitVector canbe1;                    // 1=canbe1, 0=mustbe1 // LSB = WordMask[0]!!!
  BitVector care;                      // 1=0/1, 0=?)
};

struct Mask {
  Mask(unsigned bitsize) : caremask(bitsize) {
    init_bitmasks();
  }

  Mask(const Mask& other) : bitmasks(other.bitmasks), caremask(other.caremask) {
  }

  Mask(std::initializer_list<char> other) : bitmasks(other), caremask(other.size()) {
    init_caremask();
  }

  Mask(WordMask& other) : bitmasks(other), caremask(other.size()) {
    init_caremask();
  }

  Mask(WordMaskCare& other) : caremask(other) {
    init_bitmasks();
  }

  void init_caremask() {
    caremask.Reset();
    for (unsigned i = 0; i < bitmasks.size(); ++i) {
      caremask.canbe1 &= (~0 ^ (((BitVector)(bitmasks[i] == BM_0)) << i));
      caremask.care   |= (((BitVector)(bitmasks[i] != BM_DUNNO)) << i);
    }
  }

  void init_bitmasks() {
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

  friend std::ostream& operator<<(std::ostream& stream, const Mask& mask) {
    char symbol[4] {'#', '1', '0', '?'};
    for (BitMask m : mask.bitmasks)
      stream << symbol[m % 4];
    return stream;
  }

  WordMask bitmasks; // maybe don't store this & use just for initialization
  WordMaskCare caremask;
};

// ==== Linear Propagation ====
struct Row {
  Row(BitVector x, BitVector y, bool rhs, unsigned bitsize) : x(x), y(y), rhs(rhs), bitsize(bitsize) { }

  Row GetPivotRow() {
    if (x) {
      BitVector xp = x;
      xp |= xp >> 1;
      xp |= xp >> 2;
      xp |= xp >> 4;
      xp |= xp >> 8;
      xp |= xp >> 16;
      xp |= xp >> 32;
      return Row(xp-(xp>>1), 0, 0, bitsize);
    } else if (y) {
      BitVector yp = y;
      yp |= yp >> 1;
      yp |= yp >> 2;
      yp |= yp >> 4;
      yp |= yp >> 8;
      yp |= yp >> 16;
      yp |= yp >> 32;
      return Row(0, yp-(yp>>1), 0, bitsize);
    } else {
      return Row(0, 0, rhs, bitsize);
    }
  }

  bool IsContradiction() {
    return !x && !y && rhs;
  }

  bool IsEmpty() {
    return !x && !y && !rhs;
  } 

  bool IsXSingleton() {
    return (x & (x-1)) == 0 && !y;
  }

  bool IsYSingleton() {
    return (y & (y-1)) == 0 && !x;
  }

  bool CommonVariableWith(const Row& other) {
    return (x & other.x) || (y & other.y);
  }
  
  friend Row operator^(const Row& left, const Row& right) {
    return Row(left.x ^ right.x, left.y ^ right.y, left.rhs ^ right.rhs, std::max(left.bitsize, right.bitsize));
  }

  Row& operator^=(const Row& right) {
    x ^= right.x;
    y ^= right.y;
    rhs ^= right.rhs;
    bitsize = std::max(bitsize, right.bitsize);
    return *this;
  }
   
  friend Row operator&(const Row& left, const Row& right) {
    return Row(left.x & right.x, left.y & right.y, left.rhs & right.rhs, std::max(left.bitsize, right.bitsize));
  }

  Row& operator&=(const Row& right) {
    x &= right.x;
    y &= right.y;
    rhs &= right.rhs;
    bitsize = std::max(bitsize, right.bitsize);
    return *this;
  }
 
  friend Row operator|(const Row& left, const Row& right) {
    return Row(left.x | right.x, left.y | right.y, left.rhs | right.rhs, std::max(left.bitsize, right.bitsize));
  }

  Row& operator|=(const Row& right) {
    x |= right.x;
    y |= right.y;
    rhs |= right.rhs;
    bitsize = std::max(bitsize, right.bitsize);
    return *this;
  }

  friend bool operator==(const Row& left, const Row& right) {
    return left.x == right.x && left.y == right.y && left.rhs == right.rhs;
  }

  friend std::ostream& operator<<(std::ostream& stream, const Row& row) {
    // prints in expected order (but not in memory order)
    //for (int xshift = (int)row.bitsize - 1; xshift >= 0; --xshift)
    for (unsigned xshift = 0; xshift < row.bitsize; ++xshift)
      stream << ((row.x >> xshift) & 1) << " ";
    stream << " ";
    //for (int yshift = (int)row.bitsize - 1; yshift >= 0; --yshift)
    for (unsigned yshift = 0; yshift < row.bitsize; ++yshift)
      stream << ((row.y >> yshift) & 1) << " ";
    stream << " " << row.rhs;
    return stream;
  }

  BitVector x;
  BitVector y;
  bool rhs; 
  unsigned bitsize;
};

struct LinSys {
  LinSys(std::function<BitVector(BitVector)> fun, unsigned bitsize=64) : bitsize(bitsize) {
    rows.reserve(bitsize);
    for (unsigned i = 0; i < bitsize; ++i)
      rows.emplace_back(1 << i, fun(1 << i), 0, bitsize); // lower triangle version
  }

  friend std::ostream& operator<<(std::ostream& stream, const LinSys& sys) {
    for (const Row& row : sys.rows)
      stream << row << std::endl;
    return stream;
  }

  bool AddMasks(Mask& x, Mask& y) {
    BitVector care = x.caremask.care;
    BitVector pat = 1;
    for (unsigned xshift = 0; xshift < bitsize; ++xshift, pat <<= 1) {
      if (!(care & ((~0U) << xshift)))
        break;
      if (care & pat)
        if (!AddRow(Row(pat, 0, (pat & x.caremask.canbe1) != 0, bitsize)))
          return false;
    }
    care = y.caremask.care;
    pat = 1;
    for (unsigned yshift = 0; yshift < bitsize; ++yshift, pat <<= 1) {
      if (!(care & ((~0U) << yshift)))
        break;
      if (care & pat)
        if (!AddRow(Row(0, pat, (pat & y.caremask.canbe1) != 0, bitsize)))
          return false;
    }
    return true;
  }

  bool AddRow(const Row& row) {
    // assumes that only one variable is set!!
    for (Row& other : rows) { // maybe optimize via pivots
      if (other.CommonVariableWith(row)) {
        other ^= row;
        if (other.IsContradiction()) {
          return false;
        } else if (other.IsEmpty()) {
          other = rows.back();
          rows.pop_back();
          return true;
        } else {
          Row pivotrow = other.GetPivotRow();
          for (Row& third : rows)
            if (&third != &other && third.CommonVariableWith(pivotrow))
              third ^= other;
        }
      }
    }
    return true;
  }

  bool ExtractMasks(Mask& x, Mask& y) {
    // deletes information from system!!
    for (Row& row : rows) {
      if (row.IsXSingleton()) {
        if (x.caremask.care & row.x) {
          if (((x.caremask.canbe1 & row.x) != 0) != row.rhs)
            return false;
        } else {
          x.caremask.care |= row.x;
          x.caremask.canbe1 &= (~0ULL ^ (row.x * (BitVector)(1-row.rhs)));
        }
        row = rows.back();
        rows.pop_back();
      } else if (row.IsYSingleton()) {
        if (y.caremask.care & row.y) {
          if (((y.caremask.canbe1 & row.y) != 0) != row.rhs)
            return false;
        } else {
          y.caremask.care |= row.y;
          y.caremask.canbe1 &= (~0ULL ^ (row.y * (BitVector)(1-row.rhs)));
        }
        row = rows.back();
        rows.pop_back();
      }
    }
    x.init_bitmasks();
    y.init_bitmasks();
    return true;
  }

  unsigned bitsize;
  std::vector<Row> rows;
};

// ==== Nonlinear Propagation ====
// TODO

// ==== Target Functions ====
BitVector testfun(BitVector in) {
  // y0 = x0 + x1
  // y1 = x1
  return ((in ^ (in >> 1)) & 1) | (in & 2);
}

// ==== Main / Search ====
int main() {
  auto fun = testfun;
  unsigned bitsize = 2;

  std::vector<std::pair<Mask, Mask>> inout {
    {{BM_DUNNO, BM_DUNNO},   {BM_1,     BM_0    }}, // should be 11/10
    {{BM_0,     BM_1    },   {BM_DUNNO, BM_DUNNO}}, // should be 01/01
    {{BM_1,     BM_0    },   {BM_DUNNO, BM_DUNNO}}, // should be 10/11
    {{BM_1,     BM_DUNNO},   {BM_DUNNO, BM_0    }}, // should be 11/10
    {{BM_1,     BM_DUNNO},   {BM_0,     BM_1    }}, // should be contradiction
    {{BM_1,     BM_DUNNO},   {BM_DUNNO, BM_DUNNO}}, // should be 1?/1?
  };

  for (auto challenge : inout) {
    LinSys sys(fun, bitsize);
    bool sat = sys.AddMasks(challenge.first, challenge.second);
    //Mask in(bitsize), out(bitsize);
    Mask in(challenge.first), out(challenge.second);
    sat &= sys.ExtractMasks(in, out);
    std::cout << challenge.first << "/" << challenge.second << " > ";
    if (sat)
      std::cout << in << "/" << out << std::endl;
    else {
      for (unsigned i = 0; i < bitsize; ++i)
        std::cout << "#";
      std::cout << "/";
      for (unsigned i = 0; i < bitsize; ++i)
        std::cout << "#";
      std::cout << std::endl;
    }
  }

  return 0;
}
