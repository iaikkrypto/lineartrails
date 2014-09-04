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


struct PrideState : public StateMaskBase {
  PrideState();
  PrideState& operator=(const PrideState& rhs);
  std::vector<UpdatePos> diff(const StateMaskBase& other);
  typename std::array<Mask, 8>::iterator begin();
  typename std::array<Mask, 8>::const_iterator begin() const;
  typename std::array<Mask, 8>::iterator end();
  typename std::array<Mask, 8>::const_iterator end() const;
  Mask& operator[](const int index);
  const Mask& operator[](const int index) const;
  friend std::ostream& operator<<(std::ostream& stream, const PrideState& statemask);
  void print(std::ostream& stream);
  virtual PrideState* clone();
  void SetState(BitMask value);
  void SetBit(BitMask value, int word_pos, int bit_pos);
  std::array<Mask, 8> words;
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
