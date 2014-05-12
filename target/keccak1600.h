#ifndef KECCAK1600_H_
#define KECCAK1600_H_

#include <vector>
#include <array>
#include <random>
#include <iostream>
#include <iomanip>

#include "layer.h"
#include "mask.h"
#include "statemask.h"
#include "step_linear.h"
#include "step_nonlinear.h"
#include "lrucache.h"


struct Keccak1600State : public StateMask<25,64> {
  Keccak1600State();
  friend std::ostream& operator<<(std::ostream& stream, const Keccak1600State& statemask);
  void print(std::ostream& stream);
  virtual Keccak1600State* clone();
};


#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))
#define ROTL(x,n) (((x)<<(n))|((x)>>(64-(n))))



struct Keccak1600LinearLayer : public LinearLayer {
  Keccak1600LinearLayer& operator=(const Keccak1600LinearLayer& rhs);
  Keccak1600LinearLayer();
  virtual Keccak1600LinearLayer* clone();
  void Init();
  Keccak1600LinearLayer(StateMaskBase *in, StateMaskBase *out);
  virtual bool updateStep(unsigned int step_pos);
  unsigned int GetNumSteps();
  virtual void copyValues(LinearLayer* other);

  static const unsigned int word_size_ = { 64 };
  static const unsigned int words_per_step_ = { 25 };
  static const unsigned int linear_steps_ = {1 };
  std::array<LinearStep<word_size_, words_per_step_>, linear_steps_> keccak_linear_;
};


struct Keccak1600SboxLayer : public SboxLayer<5, 320> {
  Keccak1600SboxLayer& operator=(const Keccak1600SboxLayer& rhs);
  Keccak1600SboxLayer();
  Keccak1600SboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual Keccak1600SboxLayer* clone();
  virtual bool updateStep(unsigned int step_pos);
  Mask GetVerticalMask(unsigned int b, const StateMaskBase& s) const;
  void SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask);

 static const unsigned int cache_size_ = { 0x1000 };
 static std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> cache_;
 static std::shared_ptr<LinearDistributionTable<5>> ldt_;
};



#endif // KECCAK1600_H_
