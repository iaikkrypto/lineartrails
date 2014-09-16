#ifndef LAYER_H_
#define LAYER_H_

#include "updatequeue.h"
#include "mask.h"
#include "statemask.h"
#include "step_nonlinear.h"

struct SboxPos {
  SboxPos(uint8_t layer, uint8_t pos);

  uint8_t layer_; uint8_t pos_;
};

struct Layer {
  Layer() = default;
  virtual ~Layer(){};
  Layer(StateMaskBase *in, StateMaskBase *out);
  void SetMasks(StateMaskBase *inmask, StateMaskBase *outmask);
  virtual bool Update(UpdatePos pos) = 0;
  virtual Layer* clone() = 0;
  virtual int GetNumLayer() = 0;
  StateMaskBase *in;
  StateMaskBase *out;
};

struct LinearLayer: public Layer {
  LinearLayer() = default;
  LinearLayer(StateMaskBase *in, StateMaskBase *out);
  virtual LinearLayer* clone() = 0;
  virtual bool Update(UpdatePos pos) = 0;
  virtual int GetNumLayer() = 0;
};

struct SboxLayerBase: public Layer {
  SboxLayerBase() = default;
  SboxLayerBase(StateMaskBase *in, StateMaskBase *out);
  virtual bool Update(UpdatePos pos)= 0;
  virtual void InitSboxes(std::function<BitVector(BitVector)> fun) = 0;
  virtual void GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating)= 0;
  virtual void GuessBoxRandom(UpdatePos pos, std::function<int(int, int, int)> rating) = 0;
  virtual int GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating, int mask_pos)= 0;
  virtual bool SboxActive(int pos)= 0;
  virtual bool SboxGuessable(int pos)= 0;
  virtual SboxLayerBase* clone() = 0;
  virtual ProbabilityPair GetProbability()= 0;
  virtual int GetNumLayer() = 0;
  virtual void SetSboxActive(int pos, bool active) = 0;
  virtual Mask GetVerticalMask(int b, const StateMaskBase& s) const  = 0;
  virtual void SetVerticalMask(int b, StateMaskBase& s, const Mask& mask) = 0;
};

template <unsigned bits, unsigned boxes>
struct SboxLayer: public SboxLayerBase {
  SboxLayer() = default;
  SboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual bool Update(UpdatePos pos);
  virtual void InitSboxes(std::function<BitVector(BitVector)> fun);
  virtual void GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating);
  virtual void GuessBoxRandom(UpdatePos pos, std::function<int(int, int, int)> rating);
  virtual int GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating, int mask_pos);
  virtual bool SboxActive(int pos);
  virtual bool SboxGuessable(int pos);
  virtual SboxLayer* clone() = 0;
  virtual ProbabilityPair GetProbability();
  virtual int GetNumLayer();
  virtual void SetSboxActive(int pos, bool active);
  virtual Mask GetVerticalMask(int b, const StateMaskBase& s) const  = 0;
  virtual void SetVerticalMask(int b, StateMaskBase& s, const Mask& mask) = 0;
  std::array<NonlinearStep<bits>, boxes> sboxes;
};

//-----------------------------------------------------------------------------
template <unsigned bits, unsigned boxes>
SboxLayer<bits, boxes>::SboxLayer(StateMaskBase *in, StateMaskBase *out) : SboxLayerBase(in, out) {
}

template <unsigned bits, unsigned boxes>
ProbabilityPair SboxLayer<bits, boxes>::GetProbability(){
  ProbabilityPair prob {1,0.0};

  for (int i = 0; i < boxes; ++i){
    Mask copyin(GetVerticalMask(i, *in));
    Mask copyout(GetVerticalMask(i, *out));
    ProbabilityPair temp_prob = sboxes[i].GetProbability(copyin, copyout);
    prob.sign *= temp_prob.sign;
    prob.bias += temp_prob.bias;
  }

  prob.bias += boxes-1;

  return prob;
}

template <unsigned bits, unsigned boxes>
int SboxLayer<bits, boxes>::GetNumLayer(){
  return boxes;
}

template <unsigned bits, unsigned boxes>
bool SboxLayer<bits, boxes>::Update(UpdatePos pos) {
  assert(pos.bit < boxes);
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
  assert(pos < boxes);
  return sboxes[pos].is_active_ | sboxes[pos].has_to_be_active_;
}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::SetSboxActive(int pos, bool active){
  assert(pos < boxes);
  sboxes[pos].has_to_be_active_ = active;
}

template <unsigned bits, unsigned boxes>
bool SboxLayer<bits, boxes>::SboxGuessable(int pos){
  assert(pos < boxes);
  return sboxes[pos].is_guessable_;
}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating) {
  assert(pos.bit < boxes);
  Mask copyin(GetVerticalMask(pos.bit, *in));
  Mask copyout(GetVerticalMask(pos.bit, *out));

  sboxes[pos.bit].TakeBestBox(copyin, copyout, rating);

  SetVerticalMask(pos.bit, *in, copyin);
  SetVerticalMask(pos.bit, *out, copyout);

}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::GuessBoxRandom(UpdatePos pos, std::function<int(int, int, int)> rating) {
  assert(pos.bit < boxes);
  Mask copyin(GetVerticalMask(pos.bit, *in));
  Mask copyout(GetVerticalMask(pos.bit, *out));

  sboxes[pos.bit].TakeBestBoxRandom(copyin, copyout, rating);

  SetVerticalMask(pos.bit, *in, copyin);
  SetVerticalMask(pos.bit, *out, copyout);

}

template <unsigned bits, unsigned boxes>
int SboxLayer<bits, boxes>::GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating, int mask_pos) {
  int choises;
  assert(pos.bit < boxes);
  Mask copyin(GetVerticalMask(pos.bit, *in));
  Mask copyout(GetVerticalMask(pos.bit, *out));

  choises = sboxes[pos.bit].TakeBestBox(copyin, copyout, rating, mask_pos);

  SetVerticalMask(pos.bit, *in, copyin);
  SetVerticalMask(pos.bit, *out, copyout);
  return choises;
}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::InitSboxes(std::function<BitVector(BitVector)> fun){
  std::shared_ptr<LinearDistributionTable<bits>> ldt(new LinearDistributionTable<bits>(fun));
      for (size_t i = 0; i < boxes; i++)
        sboxes[i].Initialize(ldt);
}

#endif // LAYER_H_
