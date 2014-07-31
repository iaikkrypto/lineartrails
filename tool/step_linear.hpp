#include <algorithm>

template<unsigned bitsize>
Row<bitsize>::Row(BitVector x, BitVector y, bool rhs) : x(x), y(y), rhs(rhs) {
}

template<unsigned bitsize>
Row<bitsize> Row<bitsize>::GetPivotRow() {
  if (x) {
    BitVector xp = x;
    xp |= xp >> 1;
    if (bitsize > 2) {
      xp |= xp >> 2;
      xp |= xp >> 4;
      xp |= xp >> 8;
      xp |= xp >> 16;
      xp |= xp >> 32;
    }
    return Row<bitsize>(xp-(xp>>1), 0, 0);
  } else if (y) {
    BitVector yp = y;
    yp |= yp >> 1;
    if (bitsize > 2) {
      yp |= yp >> 2;
      yp |= yp >> 4;
      yp |= yp >> 8;
      yp |= yp >> 16;
      yp |= yp >> 32;
    }
    return Row<bitsize>(0, yp-(yp>>1), 0);
  } else {
    return Row<bitsize>(0, 0, rhs);
  }
}

template <unsigned bitsize>
bool Row<bitsize>::IsContradiction() {
  return !x && !y && rhs;
}

template <unsigned bitsize>
bool Row<bitsize>::IsEmpty() {
  return !x && !y && !rhs;
} 

template <unsigned bitsize>
bool Row<bitsize>::IsXSingleton() {
  return (x & (x-1)) == 0 && !y;
}

template <unsigned bitsize>
bool Row<bitsize>::IsYSingleton() {
  return (y & (y-1)) == 0 && !x;
}

template<unsigned bitsize>
bool Row<bitsize>::CommonVariableWith(const Row<bitsize>& other) {
  return (x & other.x) || (y & other.y);
}

template<unsigned bitsize>
Row<bitsize> operator^(const Row<bitsize>& left, const Row<bitsize>& right) {
  return Row<bitsize>(left.x ^ right.x, left.y ^ right.y, left.rhs ^ right.rhs);
}

template<unsigned bitsize>
Row<bitsize>& Row<bitsize>::operator^=(const Row<bitsize>& right) {
  x ^= right.x;
  y ^= right.y;
  rhs ^= right.rhs;
  return *this;
}
 
template<unsigned bitsize>
Row<bitsize> operator&(const Row<bitsize>& left, const Row<bitsize>& right) {
  return Row<bitsize>(left.x & right.x, left.y & right.y, left.rhs & right.rhs);
}

template<unsigned bitsize>
Row<bitsize>& Row<bitsize>::operator&=(const Row<bitsize>& right) {
  x &= right.x;
  y &= right.y;
  rhs &= right.rhs;
  return *this;
}

template<unsigned bitsize>
Row<bitsize> operator|(const Row<bitsize>& left, const Row<bitsize>& right) {
  return Row<bitsize>(left.x | right.x, left.y | right.y, left.rhs | right.rhs);
}

template<unsigned bitsize>
Row<bitsize>& Row<bitsize>::operator|=(const Row<bitsize>& right) {
  x |= right.x;
  y |= right.y;
  rhs |= right.rhs;
  return *this;
}

template<unsigned bitsize>
bool operator==(const Row<bitsize>& left, const Row<bitsize>& right) {
  return left.x == right.x && left.y == right.y && left.rhs == right.rhs;
}

template<unsigned bitsize>
std::ostream& operator<<(std::ostream& stream, const Row<bitsize>& row) {
  // prints in expected order (but not in memory order)
  //for (int xshift = (int)row.bitsize - 1; xshift >= 0; --xshift)
  for (unsigned xshift = 0; xshift < bitsize; ++xshift)
    stream << ((row.x >> xshift) & 1);
    //stream << ((row.x >> xshift) & 1) << " ";
  stream << " ";
  //for (int yshift = (int)row.bitsize - 1; yshift >= 0; --yshift)
  for (unsigned yshift = 0; yshift < bitsize; ++yshift)
    stream << ((row.y >> yshift) & 1);
    //stream << ((row.y >> yshift) & 1) << " ";
  stream << " " << row.rhs;
  return stream;
}

//-----------------------------------------------------------------------------

template<unsigned bitsize>
LinearStep<bitsize>::LinearStep() {
}

template<unsigned bitsize>
LinearStep<bitsize>::LinearStep(std::function<BitVector(BitVector)> fun):fun_(fun) {
  Initialize(fun);
}

template<unsigned bitsize>
void LinearStep<bitsize>::Initialize(std::function<BitVector(BitVector)> fun) {
  fun_ = fun;
  rows.clear();
  rows.reserve(bitsize);
  for (unsigned i = 0; i < bitsize; ++i)
    rows.emplace_back(1ULL << i, fun(1ULL << i), 0); // lower triangle version
}

template<unsigned bitsize>
bool LinearStep<bitsize>::AddMasks(Mask& x, Mask& y) {
  BitVector care = x.caremask.care;
  BitVector pat = 1;
  for (unsigned xshift = 0; xshift < bitsize; ++xshift, pat <<= 1) {
    if (!(care & ((~0ULL) << xshift)))
      break;
    if (care & pat)
      if (!AddRow(Row<bitsize>(pat, 0ULL, (pat & x.caremask.canbe1) != 0)))
        return false;
  }
  care = y.caremask.care;
  pat = 1;
  for (unsigned yshift = 0; yshift < bitsize; ++yshift, pat <<= 1) {
    if (!(care & ((~0ULL) << yshift)))
      break;
    if (care & pat)
      if (!AddRow(Row<bitsize>(0ULL, pat, (pat & y.caremask.canbe1) != 0)))
        return false;
  }
  return true;
}

template<unsigned bitsize>
bool LinearStep<bitsize>::AddRow(const Row<bitsize>& row) {
  // assumes that only one variable is set!!
  for (Row<bitsize>& other : rows) { // maybe optimize via pivots
    if (other.CommonVariableWith(row)) {
      other ^= row;
      if (other.IsContradiction()) {
        return false;
      } else if (other.IsEmpty()) {
        other = rows.back();
        rows.pop_back();
        return true;
      } else {
        Row<bitsize> pivotrow = other.GetPivotRow();
        for (Row<bitsize>& third : rows)
          if (&third != &other && third.CommonVariableWith(pivotrow))
            third ^= other;
      }
    }
  }
  return true;
}

template<unsigned bitsize>
bool LinearStep<bitsize>::ExtractMasks(Mask& x, Mask& y) {
  // deletes information from system!!
  for (int i = 0; i< rows.size(); ++i) {
    if (rows[i].IsXSingleton()) {
      if (x.caremask.care & rows[i].x) {
        if (((x.caremask.canbe1 & rows[i].x) != 0) != rows[i].rhs)
          return false;
      } else {
        x.caremask.care |= rows[i].x;
        x.caremask.canbe1 &= (~0ULL ^ (rows[i].x * (BitVector)(1-rows[i].rhs)));
      }
      rows[i] = rows.back();
      rows.pop_back();
      --i;
    } else if (rows[i].IsYSingleton()) {
      if (y.caremask.care & rows[i].y) {
        if (((y.caremask.canbe1 & rows[i].y) != 0) != rows[i].rhs)
          return false;
      } else {
        y.caremask.care |= rows[i].y;
        y.caremask.canbe1 &= (~0ULL ^ (rows[i].y * (BitVector)(1-rows[i].rhs)));
      }
      rows[i] = rows.back();
      rows.pop_back();
      --i;
    }
  }
  x.init_bitmasks();
  y.init_bitmasks();
  return true;
}

template<unsigned bitsize>
std::ostream& operator<<(std::ostream& stream, const LinearStep<bitsize>& sys) {
  for (const Row<bitsize>& row : sys.rows)
    stream << row << std::endl;
  return stream;
}

template <unsigned bitsize>
LinearStep<bitsize>& LinearStep<bitsize>::operator=(const LinearStep<bitsize>& rhs){
  rows = rhs.rows;
  fun_ = rhs.fun_;
  return *this;
}

template <unsigned bitsize>
bool LinearStep<bitsize>::Update(Mask& x, Mask& y) {
  if (AddMasks(x, y))
    return ExtractMasks(x, y);
  return false;
}

template<unsigned bitsize>
bool LinearStep<bitsize>::Update(
    Mask& x, Mask& y,
    Cache<WordMaskPair<bitsize>, LinearStepUpdateInfo<bitsize>>* box_cache) {
  LinearStepUpdateInfo<bitsize> stepdata;

  WordMaskPair<bitsize> key = { x.bitmasks, y.bitmasks };
  Mask in = x;
  Mask out = y;

  if (box_cache->find(key, stepdata)) {
    rows = stepdata.rows;
    x.bitmasks = stepdata.inmask_;
    y.bitmasks = stepdata.outmask_;
    x.reinit_caremask();
    y.reinit_caremask();

    return true;
  }

  if (Update(x, y)) {
    stepdata.rows = rows;
    stepdata.inmask_ = x.bitmasks;
    stepdata.outmask_ = y.bitmasks;
    box_cache->insert(key, stepdata);
    return true;
  }

  for (unsigned int i = 0; i < bitsize; ++i) {
    x.bitmasks[i] = BM_CONTRA;
    y.bitmasks[i] = BM_CONTRA;
  }
  return false;
}

