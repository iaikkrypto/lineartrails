#ifndef STEPLINEAR_H_
#define STEPLINEAR_H_

#include <vector>
#include <iostream>
#include <functional>

#include "mask.h"

struct Row {
  Row(BitVector x, BitVector y, bool rhs, unsigned bitsize);
  Row GetPivotRow();
  bool IsContradiction();
  bool IsEmpty();
  bool IsXSingleton();
  bool IsYSingleton();
  bool CommonVariableWith(const Row& other);
  Row& operator^=(const Row& right);
  Row& operator&=(const Row& right);
  Row& operator|=(const Row& right);

  friend Row operator^(const Row& left, const Row& right);
  friend Row operator&(const Row& left, const Row& right);
  friend Row operator|(const Row& left, const Row& right);
  friend bool operator==(const Row& left, const Row& right);
  friend std::ostream& operator<<(std::ostream& stream, const Row& row);

  BitVector x;
  BitVector y;
  bool rhs; 
  unsigned bitsize;
};


struct LinSys {
  LinSys(std::function<BitVector(BitVector)> fun, unsigned bitsize=64);
  bool AddMasks(Mask& x, Mask& y);
  bool AddRow(const Row& row);
  bool ExtractMasks(Mask& x, Mask& y);

  friend std::ostream& operator<<(std::ostream& stream, const LinSys& sys);

  unsigned bitsize;
  std::vector<Row> rows;
};

#endif // STEPLINEAR_H_
