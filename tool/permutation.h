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
  virtual ~PermutationBase() {};
  virtual bool checkchar(std::ostream& stream = std::cout) = 0;
  virtual bool update() = 0;
  virtual PermutationBase* clone() const = 0;
  virtual void print(std::ostream& stream) = 0;
  virtual void SboxStatus(std::vector<SboxPos>& active, std::vector<SboxPos>& inactive) = 0;
  virtual void SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive) = 0;
  virtual bool isActive(SboxPos pos) = 0;
  virtual bool guessbestsbox(SboxPos pos, std::function<int(int, int, int)> rating) = 0;
  virtual bool guessbestsboxrandom(SboxPos pos, std::function<int(int, int, int)> rating, int num_alternatives) = 0;
  virtual bool guessbestsbox(SboxPos pos, std::function<int(int, int, int)> rating, int num_alternatives) = 0;
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0) = 0;
  virtual ProbabilityPair GetProbability() = 0;
  virtual unsigned int GetActiveSboxes() = 0;
  virtual bool setBit(const char cond, unsigned int bit) = 0;

  UpdateQueue queue_linear_;
  UpdateQueue queue_nonlinear_;
};

template <unsigned rounds>
struct Permutation : PermutationBase {
  Permutation() = default;
  Permutation(const Permutation& other);
  virtual bool checkchar(std::ostream& stream = std::cout);
  virtual bool update();
  virtual void print(std::ostream& stream);
  virtual Permutation* clone() const = 0;
  virtual void set(Permutation<rounds>* perm);
  virtual void SboxStatus(std::vector<SboxPos>& active, std::vector<SboxPos>& inactive);
  virtual void SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive);
  virtual bool isActive(SboxPos pos);
  virtual bool guessbestsbox(SboxPos pos, std::function<int(int, int, int)> rating);
  virtual bool guessbestsboxrandom(SboxPos pos, std::function<int(int, int, int)> rating, int num_alternatives);
  virtual bool guessbestsbox(SboxPos pos, std::function<int(int, int, int)> rating, int num_alternatives);
  virtual void PrintWithProbability(std::ostream& stream = std::cout, int offset = 0);
  virtual void touchall();
  virtual ProbabilityPair GetProbability();
  virtual unsigned int GetActiveSboxes();
  virtual bool setBit(BitMask cond, unsigned int bit) = 0;
  bool setBit(const char cond, unsigned int bit, unsigned char num_words, unsigned char num_bits);

  std::array<std::unique_ptr<StateMask>, 2 * rounds + 1> state_masks_;
  std::array<std::unique_ptr<SboxLayerBase>,rounds> sbox_layers_;
  std::array<std::unique_ptr<LinearLayer>, rounds> linear_layers_;
  bool toupdate_linear;
  bool toupdate_nonlinear;
};

#include "permutation.hpp"

#endif // PERMUTATION_H_
