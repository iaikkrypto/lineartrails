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
        if (bitsize > 8)
        xp |= xp >> 8;
        if (bitsize > 16)
        xp |= xp >> 16;
        if (bitsize > 32)
          xp |= xp >> 32;
      }
      xtemp[i] = xp-(xp>>1);
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
        if (bitsize > 32)
          yp |= yp >> 32;
      }
      ytemp[i] = yp-(yp>>1);
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

  return  y_0 && x_0 == words - 1 && x_s == words;
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

  return  x_0 && y_0 == words - 1 && y_s == words;
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
bool Row<bitsize, words>::ExtractMaskInfoX(std::array<Mask*, words>& x) {
  for (unsigned w = 0; w < words; ++w) {
    if (x[w]->caremask.care & this->x[w]) {
      if (((x[w]->caremask.canbe1 & this->x[w]) != 0) != rhs)
        return false;
    } else {
      x[w]->caremask.care |= this->x[w];
      x[w]->caremask.canbe1 &= (~0ULL ^ (this->x[w] * (BitVector) (1 - rhs)));
    }
  }
  return true;
}

template<unsigned bitsize, unsigned words>
bool Row<bitsize, words>::ExtractMaskInfoY(std::array<Mask*, words>& y) {
  for (unsigned w = 0; w < words; ++w) {
    if (y[w]->caremask.care & this->y[w]) {
      if (((y[w]->caremask.canbe1 & this->y[w]) != 0) != rhs)
        return false;
    } else {
      y[w]->caremask.care |= this->y[w];
      y[w]->caremask.canbe1 &= (~0ULL ^ (this->y[w] * (BitVector) (1 - rhs)));
    }
  }
  return true;
}

template<unsigned bitsize, unsigned words>
std::ostream& operator<<(std::ostream& stream, const Row<bitsize, words>& row) {
  assert(!"not implemented yet");
//  // prints in expected order (but not in memory order)
//  //for (int xshift = (int)row.bitsize - 1; xshift >= 0; --xshift)
//  for (unsigned xshift = 0; xshift < bitsize; ++xshift)
//    stream << ((row.x >> xshift) & 1);
//    //stream << ((row.x >> xshift) & 1) << " ";
//  stream << " ";
//  //for (int yshift = (int)row.bitsize - 1; yshift >= 0; --yshift)
//  for (unsigned yshift = 0; yshift < bitsize; ++yshift)
//    stream << ((row.y >> yshift) & 1);
//    //stream << ((row.y >> yshift) & 1) << " ";
//  stream << " " << row.rhs;
//  return stream;
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
            Row<bitsize, words>( x_words, y_words, (y_words[w] & y[w]->caremask.canbe1) != 0)))
          return false;
    }
    y_words[w] = 0;
  }
  return true;
}

template<unsigned bitsize, unsigned words>
bool LinearStep<bitsize, words>::AddRow(const Row<bitsize, words>& row) {
  // assumes that only one variable is set!!
  for (Row<bitsize, words>& other : rows) { // maybe optimize via pivots
    if (other.CommonVariableWith(row)) {
      other ^= row;
      if (other.IsContradiction()) {
        return false;
      } else if (other.IsEmpty()) {
        other = rows.back();
        rows.pop_back();
        return true;
      } else {
        Row<bitsize, words> pivotrow = other.GetPivotRow();
        for (Row<bitsize, words>& third : rows)
          if (&third != &other && third.CommonVariableWith(pivotrow))
            third ^= other;
      }
    }
  }
  return true;
}

template<unsigned bitsize, unsigned words>
bool LinearStep<bitsize, words>::ExtractMasks(std::array<Mask*, words>& x, std::array<Mask*, words>& y) {
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
  for (unsigned w = 0; w < words; ++w) {
    x[w]->init_bitmasks();
    y[w]->init_bitmasks();
  }
  return true;
}

template<unsigned bitsize, unsigned words>
std::ostream& operator<<(std::ostream& stream, const LinearStep<bitsize, words>& sys) {
  for (const Row<bitsize, words>& row : sys.rows)
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
    return ExtractMasks(x, y);
  return false;
}

template<unsigned bitsize, unsigned words>
bool LinearStep<bitsize, words>::Update(
    std::array<Mask*, words> x,
    std::array<Mask*, words> y,
    Cache<WordMaskArray<bitsize, words>, LinearStepUpdateInfo<bitsize, words>>* box_cache) {
  LinearStepUpdateInfo<bitsize, words> stepdata;

//  std::array<WordMask, words> x_data, y_data;
//
//  for (int w = 0; w < words; ++w) {
//    x_data[w] = x[w]->bitmasks;
//    y_data[w] = y[w]->bitmasks;
//  }
//
//  WordMaskArray<bitsize, words> key = { x_data, y_data };
//
//  if (box_cache->find(key, stepdata)) {
//    rows = stepdata.rows;
//    for (int w = 0; w < words; ++w) {
//      x[w]->bitmasks = stepdata.inmask_[w];
//      y[w]->bitmasks = stepdata.outmask_[w];
//      x[w]->reinit_caremask();
//      y[w]->reinit_caremask();
//    }
//    return true;
//  }

  if (Update(x, y)) {
//    stepdata.rows = rows;
//    for (int w = 0; w < words; ++w) {
//      stepdata.inmask_[w] = x[w]->bitmasks;
//      stepdata.outmask_[w] = y[w]->bitmasks;
//    }
//    box_cache->insert(key, stepdata);
    return true;
  }
  for (int w = 0; w < words; ++w)
    for (unsigned int i = 0; i < bitsize; ++i) {
      x[w]->bitmasks[i] = BM_CONTRA;
      y[w]->bitmasks[i] = BM_CONTRA;
    }
  return false;
}

