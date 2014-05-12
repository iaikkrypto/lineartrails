#ifndef ICEPOLE_H_
#define ICEPOLE_H_

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


struct IcepoleState : public StateMask<20,64> {
  IcepoleState();
  friend std::ostream& operator<<(std::ostream& stream, const IcepoleState& statemask);
  void print(std::ostream& stream);
  virtual IcepoleState* clone();
};


#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))
#define ROTL(x,n) (((x)<<(n))|((x)>>(64-(n))))



struct IcepoleLinearLayer : public LinearLayer {
  IcepoleLinearLayer& operator=(const IcepoleLinearLayer& rhs);
  IcepoleLinearLayer();
  virtual IcepoleLinearLayer* clone();
  void Init();
  IcepoleLinearLayer(StateMaskBase *in, StateMaskBase *out);
  virtual bool updateStep(unsigned int step_pos);
  unsigned int GetNumSteps();
  virtual void copyValues(LinearLayer* other);

  static const unsigned int word_size_ = { 64 };
  static const unsigned int words_per_step_ = { 20 };
  static const unsigned int linear_steps_ = {1 };
  std::array<LinearStep<word_size_, words_per_step_>, linear_steps_> icepole_linear_;
};


struct IcepoleSboxLayer : public SboxLayer<5, 256> {
  IcepoleSboxLayer& operator=(const IcepoleSboxLayer& rhs);
  IcepoleSboxLayer();
  IcepoleSboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual IcepoleSboxLayer* clone();
  virtual bool updateStep(unsigned int step_pos);
  Mask GetVerticalMask(unsigned int b, const StateMaskBase& s) const;
  void SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask);

 static const unsigned int cache_size_ = { 0x1000 };
 static std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> cache_;
 static std::shared_ptr<LinearDistributionTable<5>> ldt_;
};



#endif // ICEPOLE_H_
