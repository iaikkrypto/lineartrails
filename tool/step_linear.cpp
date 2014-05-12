#include <algorithm>

#include "step_linear.h"

Row::Row(BitVector x, BitVector y, bool rhs, unsigned bitsize) : x(x), y(y), rhs(rhs), bitsize(bitsize) {
}

Row Row::GetPivotRow() {
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

bool Row::IsContradiction() {
  return !x && !y && rhs;
}

bool Row::IsEmpty() {
  return !x && !y && !rhs;
} 

bool Row::IsXSingleton() {
  return (x & (x-1)) == 0 && !y;
}

bool Row::IsYSingleton() {
  return (y & (y-1)) == 0 && !x;
}

bool Row::CommonVariableWith(const Row& other) {
  return (x & other.x) || (y & other.y);
}

Row operator^(const Row& left, const Row& right) {
  return Row(left.x ^ right.x, left.y ^ right.y, left.rhs ^ right.rhs, std::max(left.bitsize, right.bitsize));
}

Row& Row::operator^=(const Row& right) {
  x ^= right.x;
  y ^= right.y;
  rhs ^= right.rhs;
  bitsize = std::max(bitsize, right.bitsize);
  return *this;
}
 
Row operator&(const Row& left, const Row& right) {
  return Row(left.x & right.x, left.y & right.y, left.rhs & right.rhs, std::max(left.bitsize, right.bitsize));
}

Row& Row::operator&=(const Row& right) {
  x &= right.x;
  y &= right.y;
  rhs &= right.rhs;
  bitsize = std::max(bitsize, right.bitsize);
  return *this;
}

Row operator|(const Row& left, const Row& right) {
  return Row(left.x | right.x, left.y | right.y, left.rhs | right.rhs, std::max(left.bitsize, right.bitsize));
}

Row& Row::operator|=(const Row& right) {
  x |= right.x;
  y |= right.y;
  rhs |= right.rhs;
  bitsize = std::max(bitsize, right.bitsize);
  return *this;
}

bool operator==(const Row& left, const Row& right) {
  return left.x == right.x && left.y == right.y && left.rhs == right.rhs;
}

std::ostream& operator<<(std::ostream& stream, const Row& row) {
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

//-----------------------------------------------------------------------------

LinSys::LinSys(std::function<BitVector(BitVector)> fun, unsigned bitsize) : bitsize(bitsize) {
  rows.reserve(bitsize);
  for (unsigned i = 0; i < bitsize; ++i)
    rows.emplace_back(1 << i, fun(1 << i), 0, bitsize); // lower triangle version
}

bool LinSys::AddMasks(Mask& x, Mask& y) {
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

bool LinSys::AddRow(const Row& row) {
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

bool LinSys::ExtractMasks(Mask& x, Mask& y) {
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

std::ostream& operator<<(std::ostream& stream, const LinSys& sys) {
  for (const Row& row : sys.rows)
    stream << row << std::endl;
  return stream;
}
