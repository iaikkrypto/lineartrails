#ifndef STEPLINEAR_H_
#define STEPLINEAR_H_

#include <vector>
#include <iostream>
#include <functional>

#include "mask.h"

template <unsigned bitsize> struct Row; // forward declaration for friends below
template <unsigned bitsize> Row<bitsize> operator^(const Row<bitsize>& left, const Row<bitsize>& right);
template <unsigned bitsize> Row<bitsize> operator&(const Row<bitsize>& left, const Row<bitsize>& right);
template <unsigned bitsize> Row<bitsize> operator|(const Row<bitsize>& left, const Row<bitsize>& right);
template <unsigned bitsize> bool operator==(const Row<bitsize>& left, const Row<bitsize>& right);
template <unsigned bitsize> std::ostream& operator<<(std::ostream& stream, const Row<bitsize>& row);

template <unsigned bitsize>
struct Row {
  static_assert((bitsize == 64 || bitsize == 2), "Check if linearstep supports your bitsize.");

  Row(BitVector x, BitVector y, bool rhs);
  Row GetPivotRow();
  bool IsContradiction();
  bool IsEmpty();
  bool IsXSingleton();
  bool IsYSingleton();
  bool CommonVariableWith(const Row<bitsize>& other);
  Row<bitsize>& operator^=(const Row<bitsize>& right);
  Row<bitsize>& operator&=(const Row<bitsize>& right);
  Row<bitsize>& operator|=(const Row<bitsize>& right);

  friend Row<bitsize> operator^<>(const Row<bitsize>& left, const Row<bitsize>& right);
  friend Row<bitsize> operator&<>(const Row<bitsize>& left, const Row<bitsize>& right);
  friend Row<bitsize> operator|<>(const Row<bitsize>& left, const Row<bitsize>& right);
  friend bool operator==<>(const Row<bitsize>& left, const Row<bitsize>& right);
  friend std::ostream& operator<<<>(std::ostream& stream, const Row<bitsize>& row);

  BitVector x;
  BitVector y;
  bool rhs; 
};

//-----------------------------------------------------------------------------

template <unsigned bitsize> struct LinearStep; // template for friends below
template <unsigned bitsize> std::ostream& operator<<(std::ostream& stream, const LinearStep<bitsize>& sys);

template <unsigned bitsize>
struct LinearStep {
  static_assert((bitsize == 64 || bitsize == 2), "Check if linearstep supports your bitsize.");

  LinearStep();
  LinearStep(std::function<BitVector(BitVector)> fun);
  void Initialize(std::function<BitVector(BitVector)> fun);
  bool AddMasks(Mask& x, Mask& y);
  bool AddRow(const Row<bitsize>& row);
  bool ExtractMasks(Mask& x, Mask& y);
  bool Update(Mask& x, Mask& y);

  friend std::ostream& operator<<<>(std::ostream& stream, const LinearStep<bitsize>& sys);

  std::vector<Row<bitsize>> rows;
};

#include "step_linear.hpp"

#endif // STEPLINEAR_H_
