#ifndef STEPNONLINEAR_H_
#define STEPNONLINEAR_H_

#include <iostream>
#include <functional>
#include <vector>
#include <unordered_map>

#include "mask.h"

template <unsigned bitsize> struct LinearDistributionTable; 
template <unsigned bitsize> std::ostream& operator<<(std::ostream& stream, const LinearDistributionTable<bitsize>& ldt);

template <unsigned bitsize>
struct LinearDistributionTable {
  LinearDistributionTable() = default;
  LinearDistributionTable(std::function<BitVector(BitVector)> fun);

  friend std::ostream& operator<<<>(std::ostream& stream, const LinearDistributionTable<bitsize>& ldt);

  std::vector<std::vector<signed>> ldt; // TODO check datatype!
  std::vector<std::vector<unsigned>> ldt_bool; // TODO check datatype!
};

template <unsigned bitsize> struct NonlinearStep;
template <unsigned bitsize> std::ostream& operator<<(std::ostream& stream, const NonlinearStep<bitsize>& step);

template <unsigned bitsize>
struct NonlinearStep {
  static_assert((bitsize == 3 || bitsize == 5), "Check if nonlinearstep supports your bitsize.");

  NonlinearStep() = default;
  NonlinearStep(std::function<BitVector(BitVector)> fun);
  bool Update(Mask& x, Mask& y);

  friend std::ostream& operator<<<>(std::ostream& stream, const NonlinearStep<bitsize>& step);

  void create_masks(std::vector<unsigned int> &masks, Mask& reference, unsigned int pos = 0, unsigned int current_mask = 0);

  LinearDistributionTable<bitsize> ldt_;
};

#include "step_nonlinear.hpp"

#endif // STEPLINEAR_H_
