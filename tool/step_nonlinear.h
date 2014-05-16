#ifndef STEPNONLINEAR_H_
#define STEPNONLINEAR_H_

#include <iostream>
#include <functional>
#include <vector>
#include <unordered_map>

#include "mask.h"

struct LinearDistributionTable {
  LinearDistributionTable();
  LinearDistributionTable(std::function<BitVector(BitVector)> fun, unsigned bitsize);

  friend std::ostream& operator<<(std::ostream& stream, const LinearDistributionTable& ldt);

  std::vector<std::vector<signed>> ldt;
  std::vector<std::vector<unsigned int>> ldt_bool;
};

struct NonlinearStep {
  NonlinearStep();
  NonlinearStep(std::function<BitVector(BitVector)> fun, unsigned bitsize);
  bool Update(Mask& x, Mask& y);

  friend std::ostream& operator<<(std::ostream& stream, const NonlinearStep& step);

  void create_masks(std::vector<unsigned int> &masks, Mask& reference, unsigned int pos = 0, unsigned int current_mask = 0);
  LinearDistributionTable ldt_;
  unsigned bitsize_;
};

#endif // STEPLINEAR_H_
