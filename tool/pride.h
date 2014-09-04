#ifndef PRIDE_H_
#define PRIDE_H_

#include <vector>
#include <array>
#include <random>

#include "layer.h"
#include "mask.h"
#include "statemask.h"
#include "step_linear.h"
#include "step_nonlinear.h"
#include "updatequeue.h"
#include "memory"
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
  virtual bool Update(UpdatePos pos);
  int GetNumLayer();

  std::array<LinearStep<8, 2>, 4> layers;
  static std::unique_ptr<LRU_Cache<WordMaskArray<8, 2>, LinearStepUpdateInfo<8, 2>>> cache_[4];
};


struct PrideSboxLayer : public SboxLayer<4, 16> {
  PrideSboxLayer& operator=(const PrideSboxLayer& rhs);
  PrideSboxLayer();
  PrideSboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual PrideSboxLayer* clone();
  void InitSboxes();
  virtual bool Update(UpdatePos pos);
  Mask GetVerticalMask(int b, const StateMaskBase& s) const;
  void SetVerticalMask(int b, StateMaskBase& s, const Mask& mask);

 static std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> cache_;
};



#endif // PRIDE_H_
