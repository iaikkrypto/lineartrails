#ifndef STEPLINEAR_H_
#define STEPLINEAR_H_

#include <vector>
#include <iostream>
#include <functional>
#include <array>

#include "cache.h"
#include "mask.h"

template <unsigned bitsize, unsigned words> struct Row; // forward declaration for friends below
template <unsigned bitsize, unsigned words> Row<bitsize, words> operator^(const Row<bitsize, words>& left, const Row<bitsize, words>& right);
template <unsigned bitsize, unsigned words> Row<bitsize, words> operator&(const Row<bitsize, words>& left, const Row<bitsize, words>& right);
template <unsigned bitsize, unsigned words> Row<bitsize, words> operator|(const Row<bitsize, words>& left, const Row<bitsize, words>& right);
template <unsigned bitsize, unsigned words> bool operator==(const Row<bitsize, words>& left, const Row<bitsize, words>& right);
template <unsigned bitsize, unsigned words> std::ostream& operator<<(std::ostream& stream, const Row<bitsize, words>& row);

template <unsigned bitsize, unsigned words>
struct Row {
  static_assert((bitsize == 64 || bitsize == 2), "Check if linearstep supports your bitsize.");

  Row(std::array<BitVector,words> x, std::array<BitVector,words> y, bool rhs);
  Row GetPivotRow();
  bool IsContradiction();
  bool IsEmpty();
  bool IsXSingleton();
  bool IsYSingleton();
  bool CommonVariableWith(const Row<bitsize, words>& other);
  bool ExtractMaskInfoX(Mask& x);
  bool ExtractMaskInfoY(Mask& y);
  Row<bitsize, words>& operator^=(const Row<bitsize, words>& right);
  Row<bitsize, words>& operator&=(const Row<bitsize, words>& right);
  Row<bitsize, words>& operator|=(const Row<bitsize, words>& right);

  friend Row<bitsize, words> operator^<>(const Row<bitsize, words>& left, const Row<bitsize, words>& right);
  friend Row<bitsize, words> operator&<>(const Row<bitsize, words>& left, const Row<bitsize, words>& right);
  friend Row<bitsize, words> operator|<>(const Row<bitsize, words>& left, const Row<bitsize, words>& right);
  friend bool operator==<>(const Row<bitsize, words>& left, const Row<bitsize, words>& right);
  friend std::ostream& operator<<<>(std::ostream& stream, const Row<bitsize, words>& row);

 private:
  std::array<BitVector,words> x;
  std::array<BitVector,words> y;
  bool rhs; 
};

//-----------------------------------------------------------------------------

template <unsigned bitsize>
struct LinearStepUpdateInfo{
  std::vector<Row<bitsize, 1>> rows;
  WordMask inmask_;
  WordMask outmask_;
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
  bool AddRow(const Row<bitsize, 1>& row);
  bool ExtractMasks(Mask& x, Mask& y);
  bool Update(Mask& x, Mask& y);
  LinearStep<bitsize>& operator=(const LinearStep<bitsize>& rhs);
//  bool Update(Mask& x, Mask& y, Cache<WordMaskPair<bitsize>, LinearStepUpdateInfo<bitsize>>* box_cache);

  friend std::ostream& operator<<<>(std::ostream& stream, const LinearStep<bitsize>& sys);

  std::function<BitVector(BitVector)> fun_;
  std::vector<Row<bitsize, 1>> rows;
};

#include "step_linear.hpp"

#endif // STEPLINEAR_H_
