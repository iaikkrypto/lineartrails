/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#ifndef LAYER_H_
#define LAYER_H_

#include "mask.h"
#include "statemask.h"
#include "step_nonlinear.h"

struct SboxPos {
  SboxPos(uint16_t layer, uint16_t pos);

  uint16_t layer_; uint16_t pos_;
};

struct Layer {
  Layer() = default;
  virtual ~Layer(){};
  Layer(StateMaskBase *in, StateMaskBase *out);
  void SetMasks(StateMaskBase *inmask, StateMaskBase *outmask);
  virtual bool Update() = 0;
  virtual bool updateStep(unsigned int step_pos) = 0;
  virtual Layer* clone() = 0;
  virtual unsigned int GetNumSteps() = 0;
  StateMaskBase *in;
  StateMaskBase *out;
};

struct LinearLayer: public Layer {
  LinearLayer() = default;
  LinearLayer(StateMaskBase *in, StateMaskBase *out);
  virtual bool Update();
  virtual LinearLayer* clone() = 0;
  virtual bool updateStep(unsigned int step_pos) = 0;
  virtual unsigned int GetNumSteps() = 0;
  virtual void copyValues(LinearLayer* other) = 0;
};

struct SboxLayerBase: public Layer {
  SboxLayerBase() = default;
  SboxLayerBase(StateMaskBase *in, StateMaskBase *out);
  virtual bool Update() = 0;
  virtual bool updateStep(unsigned int step_pos) = 0;
  virtual void InitSboxes(std::function<BitVector(BitVector)> fun) = 0;
  virtual void GuessBox(unsigned int step_pos, std::function<int(int, int, int)> rating)= 0;
  virtual void GuessBoxRandom(unsigned int step_pos, std::function<int(int, int, int)> rating) = 0;
  virtual int GuessBox(unsigned int step_pos, std::function<int(int, int, int)> rating, int mask_pos)= 0;
  virtual bool SboxActive(unsigned int step_pos)= 0;
  virtual bool SboxGuessable(unsigned int step_pos)= 0;
  virtual SboxLayerBase* clone() = 0;
  virtual double GetProbability()= 0;
  virtual unsigned int GetNumSteps() = 0;
  virtual void SetSboxActive(unsigned int step_pos, bool active) = 0;
  virtual Mask GetVerticalMask(unsigned int b, const StateMaskBase& s) const  = 0;
  virtual void SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask) = 0;
  virtual void copyValues(SboxLayerBase* other) = 0;
};

template <unsigned bits, unsigned boxes>
struct SboxLayer: public SboxLayerBase {
  SboxLayer() = default;
  SboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual bool Update();
  virtual bool updateStep(unsigned int step_pos);
  virtual void InitSboxes(std::function<BitVector(BitVector)> fun);
  virtual void InitSboxes(std::shared_ptr<LinearDistributionTable<bits>> ldt);
  virtual void GuessBox(unsigned int step_pos, std::function<int(int, int, int)> rating);
  virtual void GuessBoxRandom(unsigned int step_pos, std::function<int(int, int, int)> rating);
  virtual int GuessBox(unsigned int step_pos, std::function<int(int, int, int)> rating, int mask_pos);
  virtual bool SboxActive(unsigned int step_pos);
  virtual bool SboxGuessable(unsigned int step_pos);
  virtual SboxLayer* clone() = 0;
  virtual double GetProbability();
  virtual unsigned int GetNumSteps();
  virtual void SetSboxActive(unsigned int step_pos, bool active);
  virtual Mask GetVerticalMask(unsigned int b, const StateMaskBase& s) const  = 0;
  virtual void SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask) = 0;
  virtual void copyValues(SboxLayerBase* other);
  std::array<NonlinearStep<bits>, boxes> sboxes;
};

//-----------------------------------------------------------------------------
template <unsigned bits, unsigned boxes>
SboxLayer<bits, boxes>::SboxLayer(StateMaskBase *in, StateMaskBase *out) : SboxLayerBase(in, out) {
}

template <unsigned bits, unsigned boxes>
double SboxLayer<bits, boxes>::GetProbability(){
  double prob = {0.0};

  for (unsigned int i = 0; i < boxes; ++i){
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
bool SboxLayer<bits, boxes>::updateStep(unsigned int step_pos) {
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
bool SboxLayer<bits, boxes>::Update(){
  bool ret_val = true;

  for(unsigned int i = 0; i < boxes; ++i)
    ret_val &= updateStep(i);

  in->resetChangesSbox();
  out->resetChangesSbox();
  return ret_val;
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
void SboxLayer<bits, boxes>::GuessBox(unsigned int step_pos, std::function<int(int, int, int)> rating) {
  assert(step_pos < boxes);
  Mask copyin(GetVerticalMask(step_pos, *in));
  Mask copyout(GetVerticalMask(step_pos, *out));

  sboxes[step_pos].TakeBestBox(copyin, copyout, rating);

  SetVerticalMask(step_pos, *in, copyin);
  SetVerticalMask(step_pos, *out, copyout);

}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::GuessBoxRandom(unsigned int step_pos, std::function<int(int, int, int)> rating) {
  assert(step_pos < boxes);
  Mask copyin(GetVerticalMask(step_pos, *in));
  Mask copyout(GetVerticalMask(step_pos, *out));

  sboxes[step_pos].TakeBestBoxRandom(copyin, copyout, rating);

  SetVerticalMask(step_pos, *in, copyin);
  SetVerticalMask(step_pos, *out, copyout);

}

template <unsigned bits, unsigned boxes>
int SboxLayer<bits, boxes>::GuessBox(unsigned int step_pos, std::function<int(int, int, int)> rating, int mask_pos) {
  int choises;
  assert(step_pos < boxes);
  Mask copyin(GetVerticalMask(step_pos, *in));
  Mask copyout(GetVerticalMask(step_pos, *out));

  choises = sboxes[step_pos].TakeBestBox(copyin, copyout, rating, mask_pos);

  SetVerticalMask(step_pos, *in, copyin);
  SetVerticalMask(step_pos, *out, copyout);
  return choises;
}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::InitSboxes(std::function<BitVector(BitVector)> fun){
  std::shared_ptr<LinearDistributionTable<bits>> ldt(new LinearDistributionTable<bits>(fun));
      for (size_t i = 0; i < boxes; i++)
        sboxes[i].Initialize(ldt);
}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::InitSboxes(std::shared_ptr<LinearDistributionTable<bits>> ldt){

      for (size_t i = 0; i < boxes; i++)
        sboxes[i].Initialize(ldt);
}

template <unsigned bits, unsigned boxes>
void SboxLayer<bits, boxes>::copyValues(SboxLayerBase* other){
  SboxLayer<bits, boxes>* ptr = dynamic_cast<SboxLayer<bits, boxes>*> (other);

  sboxes = ptr->sboxes;
}

#endif // LAYER_H_
