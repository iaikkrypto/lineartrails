#ifndef ASCON_H_
#define ASCON_H_

#include <vector>
#include <array>
#include <random>
#include <iostream>

#include "layer.h"
#include "mask.h"
#include "statemask.h"
#include "step_linear.h"
#include "step_nonlinear.h"
#include "lrucache.h"


struct AsconState : public StateMask<5,64> {
  AsconState();
  friend std::ostream& operator<<(std::ostream& stream, const AsconState& statemask);
  void print(std::ostream& stream);
  virtual AsconState* clone();
};


#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))
//#define ROTL(x,n) (((x)<<(n))|((x)>>(64-(n))))

template <unsigned round>
std::array<BitVector, 1> AsconSigma(std::array<BitVector, 1> in) {
  switch (round) {
    case 0: 
      return {in[0] ^ ROTR(in[0], 19) ^ ROTR(in[0], 28)};
    case 1:
      return {in[0] ^ ROTR(in[0], 61) ^ ROTR(in[0], 39)};
    case 2: 
      return {in[0] ^ ROTR(in[0],  1) ^ ROTR(in[0],  6)};
    case 3: 
      return {in[0] ^ ROTR(in[0], 10) ^ ROTR(in[0], 17)};
    case 4:
      return {in[0] ^ ROTR(in[0],  7) ^ ROTR(in[0], 41)};
    default: 
      return {0};
  }
}


struct AsconLinearLayer : public LinearLayer {
  AsconLinearLayer& operator=(const AsconLinearLayer& rhs);
  AsconLinearLayer();
  virtual AsconLinearLayer* clone();
  void Init();
  AsconLinearLayer(StateMaskBase *in, StateMaskBase *out);
  bool Update();
  virtual bool updateStep(unsigned int step_pos);
  unsigned int GetNumSteps();
  virtual void copyValues(LinearLayer* other);

  static const unsigned int word_size_ = { 64 };
  static const unsigned int words_per_step_ = { 1 };
  static const unsigned int linear_steps_ = { 5 };
  std::array<LinearStep<word_size_, words_per_step_>, linear_steps_> sigmas;
};


struct AsconSboxLayer : public SboxLayer<5, 64> {
  AsconSboxLayer& operator=(const AsconSboxLayer& rhs);
  AsconSboxLayer();
  AsconSboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual AsconSboxLayer* clone();
  virtual bool Update();
  virtual bool updateStep(unsigned int step_pos);
  Mask GetVerticalMask(unsigned int b, const StateMaskBase& s) const;
  void SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask, bool make_dirty);

 static const unsigned int cache_size_ = { 0x1000 };
 static std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> cache_;
 static std::shared_ptr<LinearDistributionTable<5>> ldt_;
};



#endif // ASCON_H_
