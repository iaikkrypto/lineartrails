#ifndef HAMSI_H_
#define HAMSI_H_

#include <vector>
#include <array>
#include <random>
#include <memory>

#include "layer.h"
#include "mask.h"
#include "statemask.h"
#include "step_linear.h"
#include "step_nonlinear.h"
#include "lrucache.h"


struct HamsiState : public StateMask<16,32> {
  HamsiState();
  friend std::ostream& operator<<(std::ostream& stream, const HamsiState& statemask);
  void print(std::ostream& stream);
  virtual HamsiState* clone();
};

struct HamsiStateFeedForward : public HamsiState{
  HamsiStateFeedForward(HamsiState* other_state);
  friend std::ostream& operator<<(std::ostream& stream, const HamsiStateFeedForward& statemask);
  void print(std::ostream& stream);
  virtual HamsiStateFeedForward* clone();
  virtual HamsiStateFeedForward* clone(HamsiState* other_state);
  virtual Mask& operator[](const int index);
  virtual const Mask& operator[](const int index) const ;

  HamsiState* other_state_;
};


#define ROTL32(x,n) (((x)<<(n))|((x)>>(32-(n))))



struct HamsiLinearLayer : public LinearLayer {
  HamsiLinearLayer& operator=(const HamsiLinearLayer& rhs);
  HamsiLinearLayer();
  virtual HamsiLinearLayer* clone();
  void Init();
  HamsiLinearLayer(StateMaskBase *in, StateMaskBase *out);
  virtual bool Update(UpdatePos pos);
  int GetNumLayer();

  static const unsigned int word_size_ = { 32 };
  static const unsigned int words_per_step_ = { 4 };
  static const unsigned int linear_steps_ = { 4 };
  static const unsigned int cache_size_ = { 0x1000 };
  std::array<LinearStep<word_size_, words_per_step_>, linear_steps_> layers;
  static std::unique_ptr<
      LRU_Cache<WordMaskArray<word_size_, words_per_step_>,
          LinearStepUpdateInfo<word_size_, words_per_step_>>> cache_[1];
};


struct HamsiSboxLayer : public SboxLayer<4, 128> {
  HamsiSboxLayer& operator=(const HamsiSboxLayer& rhs);
  HamsiSboxLayer();
  HamsiSboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual HamsiSboxLayer* clone();
  virtual bool Update(UpdatePos pos);
  Mask GetVerticalMask(int b, const StateMaskBase& s) const;
  void SetVerticalMask(int b, StateMaskBase& s, const Mask& mask);

 static const unsigned int cache_size_ = { 0x1000 };
 static std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> cache_;
};



#endif // HAMSI_H_
