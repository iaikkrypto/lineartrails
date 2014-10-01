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
  virtual bool Update(unsigned int step_pos) = 0;
  virtual Layer* clone() = 0;
  virtual unsigned int GetNumSteps() = 0;
  StateMaskBase *in;
  StateMaskBase *out;
};

struct LinearLayer: public Layer {
  LinearLayer() = default;
  LinearLayer(StateMaskBase *in, StateMaskBase *out);
  virtual LinearLayer* clone() = 0;
  virtual bool Update(unsigned int step_pos) = 0;
  virtual unsigned int GetNumSteps() = 0;
};

struct SboxLayerBase: public Layer {
  SboxLayerBase() = default;
  SboxLayerBase(StateMaskBase *in, StateMaskBase *out);
  virtual bool Update(unsigned int step_pos)= 0;
  virtual void InitSboxes(std::function<BitVector(BitVector)> fun) = 0;
  virtual void GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating)= 0;
  virtual void GuessBoxRandom(UpdatePos pos, std::function<int(int, int, int)> rating) = 0;
  virtual int GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating, int mask_pos)= 0;
  virtual bool SboxActive(unsigned int step_pos)= 0;
  virtual bool SboxGuessable(unsigned int step_pos)= 0;
  virtual SboxLayerBase* clone() = 0;
  virtual double GetProbability()= 0;
  virtual unsigned int GetNumSteps() = 0;
  virtual void SetSboxActive(unsigned int step_pos, bool active) = 0;
  virtual Mask GetVerticalMask(unsigned int b, const StateMaskBase& s) const  = 0;
  virtual void SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask) = 0;
};

template <unsigned bits, unsigned boxes>
struct SboxLayer: public SboxLayerBase {
  SboxLayer() = default;
  SboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual bool Update(unsigned int step_pos);
  virtual void InitSboxes(std::function<BitVector(BitVector)> fun);
  virtual void GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating);
  virtual void GuessBoxRandom(UpdatePos pos, std::function<int(int, int, int)> rating);
  virtual int GuessBox(UpdatePos pos, std::function<int(int, int, int)> rating, int mask_pos);
  virtual bool SboxActive(unsigned int step_pos);
  virtual bool SboxGuessable(unsigned int step_pos);
  virtual SboxLayer* clone() = 0;
  virtual double GetProbability();
  virtual unsigned int GetNumSteps();
  virtual void SetSboxActive(unsigned int step_pos, bool active);
  virtual Mask GetVerticalMask(unsigned int b, const StateMaskBase& s) const  = 0;
  virtual void SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask) = 0;
  std::array<NonlinearStep<bits>, boxes> sboxes;
};

//-----------------------------------------------------------------------------
template <unsigned bits, unsigned boxes>
SboxLayer<bits, boxes>::SboxLayer(StateMaskBase *in, StateMaskBase *out) : SboxLayerBase(in, out) {
}

template <unsigned bits, unsigned boxes>
double SboxLayer<bits, boxes>::GetProbability(){
  double prob = {0.0};

  for (int i = 0; i < boxes; ++i){
    Mask copyin(GetVerticalMask(i, *in));
    Mask copyout(GetVerticalMask(i, *out));
    double temp_prob = sboxes[i].GetProbability(copyin, copyout);
    prob += temp_prob;
  }

  prob += boxes-1;

  return prob;
}

template <unsigned bits, unsigned boxes>
unsigned int SboxLayer<bits, boxes>::GetNumSteps(){
  return boxes;
}

template <unsigned bits, unsigned boxes>
bool SboxLayer<bits, boxes>::Update(unsigned int step_pos) {
  assert(step_pos < boxes);
  Mask copyin(GetVerticalMask(step_pos, *in));
  Mask copyout(GetVerticalMask(step_pos, *out));
  if (!sboxes[step_pos].Update(copyin, copyout))
    return false;
  SetVerticalMask(step_pos, *in, copyin);
  SetVerticalMask(step_pos, *out, copyout);
  return true;
}

template <unsigned bits, unsigned boxes>
bool SboxLayer<bits, boxes>::SboxActive(unsigned int step_pos){
  assert(step_pos < boxes);
  return sboxes[step_pos].is_active_ | sboxes[step_pos].has_to_be_active_;
}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::SetSboxActive(unsigned int step_pos, bool active){
  assert(step_pos < boxes);
  sboxes[step_pos].has_to_be_active_ = active;
}

template <unsigned bits, unsigned boxes>
bool SboxLayer<bits, boxes>::SboxGuessable(unsigned int step_pos){
  assert(step_pos < boxes);
  return sboxes[step_pos].is_guessable_;
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
