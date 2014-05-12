#ifndef STEPNONLINEAR_H_
#define STEPNONLINEAR_H_

#include <iostream>
#include <functional>
#include <vector>
#include <unordered_map>

#include "mask.h"

struct LinearDistributionTable {
  LinearDistributionTable(std::function<BitVector(BitVector)> fun, unsigned bitsize);

  friend std::ostream& operator<<(std::ostream& stream, const LinearDistributionTable& ldt);

  std::vector<std::vector<unsigned>> ldt;
  std::vector<std::vector<bool>> ldt_bool;
  //std::unordered_map<Mask,Mask> ldt_map; // TODO define hash
};

struct NonlinearStep {
  NonlinearStep(std::function<BitVector(BitVector)> fun, unsigned bitsize);
  bool AddMasks(Mask& x, Mask& y);
  bool ExtractMasks(Mask& x, Mask& y);

  friend std::ostream& operator<<(std::ostream& stream, const NonlinearStep& step);

  std::function<BitVector(BitVector)> fun;
  std::pair<Mask, Mask> inout;
  unsigned bitsize;
};

#endif // STEPLINEAR_H_
