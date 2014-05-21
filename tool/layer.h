#ifndef LAYER_H_
#define LAYER_H_

#include "updatequeue.h"
#include "mask.h"

struct SboxPos {
  SboxPos(uint8_t layer, uint8_t pos);

  uint8_t layer_; uint8_t pos_;
};

struct Layer {
  Layer(StateMask *in, StateMask *out);
  virtual bool Update(UpdatePos pos) = 0;
  virtual void GuessBox(UpdatePos pos);
  StateMask *in;
  StateMask *out;
};

#endif // LAYER_H_
