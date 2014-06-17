#ifndef PERMUTATION_H_
#define PERMUTATION_H_

#include <stdint.h>
#include <vector>

#include "layer.h"
#include "mask.h"
#include "updatequeue.h"
#include "memory"

#include "layer.h"

//TODO:remove
#include "ascon.h"


struct PermutationBase {
  virtual bool checkchar() = 0;
  virtual bool update() = 0;
  virtual PermutationBase* clone() const = 0;
  virtual void SboxStatus(std::vector<SboxPos>& active, std::vector<SboxPos>& inactive) = 0;
  virtual void SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive) = 0;
  virtual bool guessbestsbox(SboxPos pos) = 0;
  virtual bool guessbestsbox(SboxPos pos, int num_alternatives) = 0;
  virtual void PrintWithProbability() = 0;
  virtual ProbabilityPair GetProbability() = 0;

  UpdateQueue queue_linear_;
  UpdateQueue queue_nonlinear_;
};

template <unsigned rounds>
struct Permutation : PermutationBase {
  Permutation() = default;
  virtual bool checkchar() = 0;
  virtual bool update() = 0;
  virtual Permutation* clone() const = 0;
  virtual void SboxStatus(std::vector<SboxPos>& active, std::vector<SboxPos>& inactive);
  virtual void SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive);
  virtual bool guessbestsbox(SboxPos pos) = 0;
  virtual bool guessbestsbox(SboxPos pos, int num_alternatives) = 0;
  virtual void PrintWithProbability() = 0;
  virtual ProbabilityPair GetProbability() = 0;

  std::array<std::unique_ptr<SboxLayerBase>,rounds> sbox_layers_;
  std::array<std::unique_ptr<LinearLayer>, rounds> linear_layers_;
};

#include "permutation.hpp"

#endif // PERMUTATION_H_
