#ifndef LAYER_H_
#define LAYER_H_

#include "updatequeue.h"
#include "mask.h"
#include "step_nonlinear.h"

struct SboxPos {
  SboxPos(uint8_t layer, uint8_t pos);

  uint8_t layer_; uint8_t pos_;
};

struct Layer {
  Layer() = default;
  Layer(StateMask *in, StateMask *out);
  void SetMasks(StateMask *inmask, StateMask *outmask);
  virtual bool Update(UpdatePos pos) = 0;
  StateMask *in;
  StateMask *out;
};

struct LinearLayer: public Layer {
  LinearLayer() = default;
  LinearLayer(StateMask *in, StateMask *out);
  virtual bool Update(UpdatePos pos) = 0;

};

template <unsigned bits, unsigned boxes>
struct SboxLayer: public Layer {
  SboxLayer() = default;
  SboxLayer(StateMask *in, StateMask *out);
  virtual bool Update(UpdatePos pos);
  virtual void InitSboxes() = 0;
  virtual void GuessBox(UpdatePos pos);
  virtual int GuessBox(UpdatePos pos, int mask_pos);
  virtual bool SboxActive(int pos);
  virtual bool SboxGuessable(int pos);
  virtual ProbabilityPair GetProbability();
  virtual Mask GetVerticalMask(int b, const StateMask& s) const  = 0;
  virtual void SetVerticalMask(int b, StateMask& s, const Mask& mask) = 0;
  std::array<NonlinearStep<bits>, boxes> sboxes;
};

//-----------------------------------------------------------------------------
template <unsigned bits, unsigned boxes>
SboxLayer<bits, boxes>::SboxLayer(StateMask *in, StateMask *out) : Layer(in, out) {
}

template <unsigned bits, unsigned boxes>
ProbabilityPair SboxLayer<bits, boxes>::GetProbability(){
  ProbabilityPair prob {1,0.0};

  for (int i = 0; i < 64; ++i){
    Mask copyin(GetVerticalMask(i, *in));
    Mask copyout(GetVerticalMask(i, *out));
    ProbabilityPair temp_prob = sboxes[i].GetProbability(copyin, copyout);
    prob.sign *= temp_prob.sign;
    prob.bias += temp_prob.bias;
  }

  prob.bias += 63;

  return prob;
}

template <unsigned bits, unsigned boxes>
bool SboxLayer<bits, boxes>::Update(UpdatePos pos) {
  Mask copyin(GetVerticalMask(pos.bit, *in));
  Mask copyout(GetVerticalMask(pos.bit, *out));
  if (!sboxes[pos.bit].Update(copyin, copyout))
    return false;
  SetVerticalMask(pos.bit, *in, copyin);
  SetVerticalMask(pos.bit, *out, copyout);
  return true;
}

template <unsigned bits, unsigned boxes>
bool SboxLayer<bits, boxes>::SboxActive(int pos){
  return sboxes[pos].is_active_;
}

template <unsigned bits, unsigned boxes>
bool SboxLayer<bits, boxes>::SboxGuessable(int pos){
  return sboxes[pos].is_guessable_;
}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::GuessBox(UpdatePos pos) {
  Mask copyin(GetVerticalMask(pos.bit, *in));
  Mask copyout(GetVerticalMask(pos.bit, *out));

  sboxes[pos.bit].TakeBestBox(copyin, copyout);

  SetVerticalMask(pos.bit, *in, copyin);
  SetVerticalMask(pos.bit, *out, copyout);

}

template <unsigned bits, unsigned boxes>
int SboxLayer<bits, boxes>::GuessBox(UpdatePos pos, int mask_pos) {
  int choises;
  Mask copyin(GetVerticalMask(pos.bit, *in));
  Mask copyout(GetVerticalMask(pos.bit, *out));

  choises = sboxes[pos.bit].TakeBestBox(copyin, copyout, mask_pos);

  SetVerticalMask(pos.bit, *in, copyin);
  SetVerticalMask(pos.bit, *out, copyout);
  return choises;
}

#endif // LAYER_H_
