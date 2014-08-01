#include <algorithm>

template<unsigned bitsize, unsigned words>
Row<bitsize, words>::Row(std::array<BitVector,words> x, std::array<BitVector,words> y, bool rhs) : x(x), y(y), rhs(rhs) {
}

template<unsigned bitsize, unsigned words>
Row<bitsize, words> Row<bitsize, words>::GetPivotRow() {
  std::array<BitVector,words> xtemp, ytemp;

  for (int i = 0; i < words; ++i) {
    xtemp[i] = ytemp[i] = 0;
  }

  for (int i = 0; i < words; ++i)
    if (x[i]) {
      BitVector xp = x[i];
      xp |= xp >> 1;
      if (bitsize > 2) {
        xp |= xp >> 2;
        xp |= xp >> 4;
        xp |= xp >> 8;
        xp |= xp >> 16;
        xp |= xp >> 32;
      }
      xtemp[i] = {xp-(xp>>1)};
      return Row<bitsize, words>(xtemp, ytemp, 0);
    }
  for (int i = 0; i < words; ++i)
    if (y[i]) {
      BitVector yp = y[i];
      yp |= yp >> 1;
      if (bitsize > 2) {
        yp |= yp >> 2;
        yp |= yp >> 4;
        yp |= yp >> 8;
        yp |= yp >> 16;
        yp |= yp >> 32;
      }
      ytemp[i] = {yp-(yp>>1)};
      return Row<bitsize, words>(xtemp, ytemp, 0);
    }

  return Row<bitsize, words>(xtemp, ytemp, rhs);

}

template <unsigned bitsize, unsigned words>
bool Row<bitsize, words>::IsContradiction() {
  bool ret_val = true;
  for (int i = 0; i < words; ++i)
    ret_val &= !x[i] && !y[i];
  return ret_val && rhs;
}

template <unsigned bitsize, unsigned words>
bool Row<bitsize, words>::IsEmpty() {
  bool ret_val = true;
  for (int i = 0; i < words; ++i)
    ret_val &= !x[i] && !y[i];
  return ret_val && !rhs;
} 

template <unsigned bitsize, unsigned words>
bool Row<bitsize, words>::IsXSingleton() {
  bool y_0 = true;
    for (int i = 0; i < words; ++i)
      y_0 &= !y[i];
  int x_0 = 0;
  for (int i = 0; i < words; ++i)
        x_0 += (x[i] == 0);
  int x_s = 0;
    for (int i = 0; i < words; ++i)
          x_s += ((x[i] & (x[i]-1)) == 0);

  return  y_0 && x_0 == words - 1 && x_s == 1;
}

template <unsigned bitsize, unsigned words>
bool Row<bitsize, words>::IsYSingleton() {
  bool x_0 = true;
    for (int i = 0; i < words; ++i)
      x_0 &= !x[i];
  int y_0 = 0;
  for (int i = 0; i < words; ++i)
        y_0 += (y[i] == 0);
  int y_s = 0;
    for (int i = 0; i < words; ++i)
          y_s += ((y[i] & (y[i]-1)) == 0);

  return  x_0 && y_0 == words - 1 && y_s == 1;
}

template<unsigned bitsize, unsigned words>
bool Row<bitsize, words>::CommonVariableWith(const Row<bitsize, words>& other) {
  bool ret_val = false;
  for (int i = 0; i < words; ++i)
    ret_val |= (x[i] & other.x[i]) || (y[i] & other.y[i]);
  return ret_val;
}

template<unsigned bitsize, unsigned words>
Row<bitsize, words> operator^(const Row<bitsize, words>& left, const Row<bitsize, words>& right) {
  std::array<BitVector,words> xtemp, ytemp;
    for (int i = 0; i < words; ++i){
      xtemp[i] = left.x[i] ^ right.x[i];
      ytemp[i] = left.y[i] ^ right.y[i];
    }

  return Row<bitsize, words>(xtemp, ytemp, left.rhs ^ right.rhs);
}

template<unsigned bitsize, unsigned words>
Row<bitsize, words>& Row<bitsize, words>::operator^=(
    const Row<bitsize, words>& right) {
  for (int i = 0; i < words; ++i) {
    x[i] ^= right.x[i];
    y[i] ^= right.y[i];
  }
  rhs ^= right.rhs;
  return *this;
}
 
template<unsigned bitsize, unsigned words>
Row<bitsize, words> operator&(const Row<bitsize, words>& left, const Row<bitsize, words>& right) {
  std::array<BitVector,words> xtemp, ytemp;
    for (int i = 0; i < words; ++i){
      xtemp[i] = left.x[i] & right.x[i];
      ytemp[i] = left.y[i] & right.y[i];
    }

  return Row<bitsize, words>(xtemp, ytemp, left.rhs & right.rhs);
}

template<unsigned bitsize, unsigned words>
Row<bitsize, words>& Row<bitsize, words>::operator&=(const Row<bitsize, words>& right) {
  for (int i = 0; i < words; ++i) {
    x[i] &= right.x[i];
    y[i] &= right.y[i];
  }
  rhs &= right.rhs;
  return *this;
}

template<unsigned bitsize, unsigned words>
Row<bitsize, words> operator|(const Row<bitsize, words>& left, const Row<bitsize, words>& right) {
  std::array<BitVector,words> xtemp, ytemp;
    for (int i = 0; i < words; ++i){
      xtemp[i] = left.x[i] | right.x[i];
      ytemp[i] = left.y[i] | right.y[i];
    }

  return Row<bitsize, words>(xtemp, ytemp, left.rhs | right.rhs);
}

template<unsigned bitsize, unsigned words>
Row<bitsize, words>& Row<bitsize, words>::operator|=(const Row<bitsize, words>& right) {
  for (int i = 0; i < words; ++i) {
    x[i] |= right.x[i];
    y[i] |= right.y[i];
  }
  rhs |= right.rhs;
  return *this;
}

template<unsigned bitsize, unsigned words>
bool operator==(const Row<bitsize, words>& left, const Row<bitsize, words>& right) {
  bool ret_val = true;
  for (int i = 0; i < words; ++i)
    ret_val &= left.x[i] == right.x[i] && left.y[i] == right.y[i];

  return ret_val && left.rhs == right.rhs;
}

template<unsigned bitsize, unsigned words>
bool Row<bitsize, words>::ExtractMaskInfoX(Mask& x) {
if (x.caremask.care & this->x[0]) {
  if (((x.caremask.canbe1 & this->x[0]) != 0) != rhs)
    return false;
} else {
  x.caremask.care |= this->x[0];
  x.caremask.canbe1 &= (~0ULL ^ (this->x[0] * (BitVector)(1-rhs)));
}
return true;
}

template<unsigned bitsize, unsigned words>
bool Row<bitsize, words>::ExtractMaskInfoY(Mask& y) {
if (y.caremask.care & this->y[0]) {
  if (((y.caremask.canbe1 & this->y[0]) != 0) != rhs)
    return false;
} else {
  y.caremask.care |= this->y[0];
  y.caremask.canbe1 &= (~0ULL ^ (this->y[0] * (BitVector)(1-rhs)));
}
return true;
}

template<unsigned bitsize, unsigned words>
std::ostream& operator<<(std::ostream& stream, const Row<bitsize, words>& row) {
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

template<unsigned bitsize, unsigned words>
LinearStep<bitsize, words>::LinearStep() {
}

template<unsigned bitsize, unsigned words>
LinearStep<bitsize, words>::LinearStep(std::function<std::array<BitVector, words>(std::array<BitVector, words>)> fun):fun_(fun) {
  Initialize(fun);
}

template<unsigned bitsize, unsigned words>
void LinearStep<bitsize, words>::Initialize(std::function<std::array<BitVector, words>(std::array<BitVector, words>)> fun) {
  fun_ = fun;
  rows.clear();
  rows.reserve(bitsize);

  std::array<BitVector, words> x_words;

  for (unsigned w = 0; w < words; ++w)
    x_words[w] = 0;


  for (unsigned w = 0; w < words; ++w) {
    for (unsigned i = 0; i < bitsize; ++i) {
      x_words[w] = 1ULL << i;
      rows.emplace_back(x_words, fun(x_words), 0);  // lower triangle version
    }
    x_words[w] = 0;
  }
}

template<unsigned bitsize, unsigned words>
bool LinearStep<bitsize, words>::AddMasks(std::array<Mask*, words>& x, std::array<Mask*, words>& y) {

  std::array<BitVector, words> x_words, y_words;

  for (unsigned w = 0; w < words; ++w)
    x_words[w] = y_words[w] = 0;

  for (unsigned w = 0; w < words; ++w) {
    x_words[w] = 1;
    for (unsigned xshift = 0; xshift < bitsize; ++xshift, x_words[w] <<= 1) {
      if (!(x[w]->caremask.care & ((~0ULL) << xshift)))
        break;
      if (x[w]->caremask.care & x_words[w])
        if (!AddRow(
            Row<bitsize, words>(x_words, y_words,
                                (x_words[w] & x[w]->caremask.canbe1) != 0)))
          return false;
    }
    x_words[w] = 0;
  }

  for (unsigned w = 0; w < words; ++w) {
    y_words[w] = 1;
    for (unsigned yshift = 0; yshift < bitsize; ++yshift, y_words[w] <<= 1) {
      if (!(y[w]->caremask.care & ((~0ULL) << yshift)))
        break;
      if (y[w]->caremask.care & y_words[w])
        if (!AddRow(
            Row<bitsize, 1>( x_words, y_words, (y_words[w] & y[w]->caremask.canbe1) != 0)))
          return false;
    }
  }
  return true;
}

template<unsigned bitsize, unsigned words>
bool LinearStep<bitsize, words>::AddRow(const Row<bitsize, words>& row) {
  // assumes that only one variable is set!!
  for (Row<bitsize, 1>& other : rows) { // maybe optimize via pivots
    if (other.CommonVariableWith(row)) {
      other ^= row;
      if (other.IsContradiction()) {
        return false;
      } else if (other.IsEmpty()) {
        other = rows.back();
        rows.pop_back();
        return true;
      } else {
        Row<bitsize, 1> pivotrow = other.GetPivotRow();
        for (Row<bitsize, 1>& third : rows)
          if (&third != &other && third.CommonVariableWith(pivotrow))
            third ^= other;
      }
    }
  }
  return true;
}

template<unsigned bitsize, unsigned words>
bool LinearStep<bitsize, words>::ExtractMasks(Mask& x, Mask& y) {
  // deletes information from system!!
  for (int i = 0; i < rows.size(); ++i) {
    if (rows[i].IsXSingleton()) {
      if (!rows[i].ExtractMaskInfoX(x))
        return false;
      rows[i] = rows.back();
      rows.pop_back();
      --i;
    } else if (rows[i].IsYSingleton()) {
      if (!rows[i].ExtractMaskInfoY(y))
        return false;
      rows[i] = rows.back();
      rows.pop_back();
      --i;
    }
  }
  x.init_bitmasks();
  y.init_bitmasks();
  return true;
}

template<unsigned bitsize, unsigned words>
std::ostream& operator<<(std::ostream& stream, const LinearStep<bitsize, words>& sys) {
  for (const Row<bitsize, 1>& row : sys.rows)
    stream << row << std::endl;
  return stream;
}

template <unsigned bitsize, unsigned words>
LinearStep<bitsize, words>& LinearStep<bitsize, words>::operator=(const LinearStep<bitsize, words>& rhs){
  rows = rhs.rows;
  fun_ = rhs.fun_;
  return *this;
}

template <unsigned bitsize, unsigned words>
bool LinearStep<bitsize, words>::Update(std::array<Mask*, words> x,std::array<Mask*, words>  y) {
  if (AddMasks(x, y))
    return ExtractMasks(*(x[0]), *(y[0]));
  return false;
}

//template<unsigned bitsize>
//bool LinearStep<bitsize>::Update(
//    Mask& x, Mask& y,
//    Cache<WordMaskPair<bitsize>, LinearStepUpdateInfo<bitsize>>* box_cache) {
//  LinearStepUpdateInfo<bitsize> stepdata;
//
//  WordMaskPair<bitsize> key = { x.bitmasks, y.bitmasks };
//  Mask in = x;
//  Mask out = y;
//
//  if (box_cache->find(key, stepdata)) {
//    rows = stepdata.rows;
//    x.bitmasks = stepdata.inmask_;
//    y.bitmasks = stepdata.outmask_;
//    x.reinit_caremask();
//    y.reinit_caremask();
//
//    return true;
//  }
//
//  if (Update(x, y)) {
//    stepdata.rows = rows;
//    stepdata.inmask_ = x.bitmasks;
//    stepdata.outmask_ = y.bitmasks;
//    box_cache->insert(key, stepdata);
//    return true;
//  }
//
//  for (unsigned int i = 0; i < bitsize; ++i) {
//    x.bitmasks[i] = BM_CONTRA;
//    y.bitmasks[i] = BM_CONTRA;
//  }
//  return false;
//}

