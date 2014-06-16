#ifndef PERMUTATION_H_
#define PERMUTATION_H_

#include <stdint.h>
#include <vector>

#include "layer.h"
#include "mask.h"
#include "updatequeue.h"
#include "memory"

struct Permutation {
  virtual bool checkchar() = 0;
  virtual bool update() = 0;
  virtual Permutation* clone() const = 0;
  virtual void SboxStatus(std::vector<SboxPos>& active, std::vector<SboxPos>& inactive) = 0;
  virtual void SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive) = 0;
  virtual bool guessbestsbox(SboxPos pos) = 0;
  virtual bool guessbestsbox(SboxPos pos, int num_alternatives) = 0;
  virtual void PrintWithProbability() = 0;
  virtual ProbabilityPair GetProbability() = 0;

  UpdateQueue queue_linear_;
  UpdateQueue queue_nonlinear_;
};

#endif // UPDATEQUEUE_H_
