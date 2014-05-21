#ifndef LAYER_H_
#define LAYER_H_

#include "updatequeue.h"
#include "mask.h"

struct Layer {
  Layer(StateMask *in, StateMask *out);
  virtual bool Update(UpdatePos pos) = 0;
  virtual void GuessBox(UpdatePos pos);
  StateMask *in;
  StateMask *out;
};

#endif // LAYER_H_
