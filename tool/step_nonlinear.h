/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
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
//  static_assert((bitsize == 3 || bitsize == 5), "Check if nonlinearstep supports your bitsize.");

  NonlinearStep() = default;
  NonlinearStep(std::function<BitVector(BitVector)> fun);
  void Initialize(std::function<BitVector(BitVector)> fun);
  void Initialize(std::shared_ptr<LinearDistributionTable<bitsize>> ldt);
  double GetProbability(Mask& x, Mask& y);
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
  bool has_to_be_active_;
  static std::vector<unsigned int> inmasks_, outmasks_;
  static std::multimap<int, std::pair<unsigned int, unsigned int>, std::greater<int>> valid_masks_;
};

#include "step_nonlinear.hpp"

#endif // STEPLINEAR_H_
