#ifndef PRIDE_H_
#define PRIDE_H_

#include <vector>
#include <array>
#include <memory>

#include "layer.h"
#include "mask.h"
#include "statemask.h"
#include "step_linear.h"
#include "step_nonlinear.h"
#include "lrucache.h"


struct PrideState : public StateMask<8,8> {
  PrideState();
  friend std::ostream& operator<<(std::ostream& stream, const PrideState& statemask);
  void print(std::ostream& stream);
  virtual PrideState* clone();
};


#define ROTL32(x,n) (((x)<<(n))|((x)>>(32-(n))))



struct PrideLinearLayer : public LinearLayer {
  PrideLinearLayer& operator=(const PrideLinearLayer& rhs);
  PrideLinearLayer();
  virtual PrideLinearLayer* clone();
  void Init();
  PrideLinearLayer(StateMaskBase *in, StateMaskBase *out);
  virtual bool updateStep(unsigned int step_pos);
  unsigned int GetNumSteps();
  virtual void copyValues(LinearLayer* other){}

  static const unsigned int word_size_ = { 8 };
  static const unsigned int words_per_step_ = { 2 };
  static const unsigned int linear_steps_ = { 4 };
  std::array<LinearStep<word_size_, words_per_step_>, linear_steps_> layers;
};


struct PrideSboxLayer : public SboxLayer<4, 16> {
  PrideSboxLayer& operator=(const PrideSboxLayer& rhs);
  PrideSboxLayer();
  PrideSboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual PrideSboxLayer* clone();
  virtual bool updateStep(unsigned int step_pos);
  Mask GetVerticalMask(unsigned int b, const StateMaskBase& s) const;
  void SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask, bool make_dirty);

  static const unsigned int cache_size_ = { 0x1000 };
 static std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> cache_;
 static std::shared_ptr<LinearDistributionTable<4>> ldt_;
};



#endif // PRIDE_H_
