#ifndef STEPNONLINEAR_H_
#define STEPNONLINEAR_H_

#include <iostream>
#include <functional>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <memory>
#include <map>
#include <assert.h>
#include <iterator>
#include <chrono>
#include <random>

#include "cache.h"
#include "mask.h"

struct NonlinearStepUpdateInfo{
  bool is_active_;
  bool is_guessable_;
  WordMask inmask_;
  WordMask outmask_;
};


template <unsigned bitsize> struct LinearDistributionTable; 
template <unsigned bitsize> std::ostream& operator<<(std::ostream& stream, const LinearDistributionTable<bitsize>& ldt);

template <unsigned bitsize>
struct LinearDistributionTable {
  LinearDistributionTable() = default;
  LinearDistributionTable(std::function<BitVector(BitVector)> fun);
  void Initialize(std::function<BitVector(BitVector)> fun);
  LinearDistributionTable<bitsize>& operator=(const LinearDistributionTable<bitsize>& rhs);

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
  void Initialize(std::function<BitVector(BitVector)> fun);
  void Initialize(std::shared_ptr<LinearDistributionTable<bitsize>> ldt);
  ProbabilityPair GetProbability(Mask& x, Mask& y);
  bool Update(Mask& x, Mask& y);
  bool Update(Mask& x, Mask& y, Cache<unsigned long long, NonlinearStepUpdateInfo>* box_cache);
  void TakeBestBox(Mask& x, Mask& y, std::function<int(int, int, int)> rating);
  int TakeBestBox(Mask& x, Mask& y, std::function<int(int, int, int)> rating, int pos);
  void TakeBestBoxRandom(Mask& x, Mask& y, std::function<int(int, int, int)> rating);
  unsigned long long getKey(Mask& in, Mask& out);
  void create_masks(std::vector<unsigned int> &masks, Mask& reference, unsigned int pos = 0, unsigned int current_mask = 0);
  NonlinearStep<bitsize>& operator=(const NonlinearStep<bitsize>& rhs);

  friend std::ostream& operator<<<>(std::ostream& stream, const NonlinearStep<bitsize>& step);

  std::shared_ptr<LinearDistributionTable<bitsize>> ldt_;
  bool is_active_;
  bool is_guessable_;
};

#include "step_nonlinear.hpp"

#endif // STEPLINEAR_H_
