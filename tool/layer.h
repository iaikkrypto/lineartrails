#ifndef LAYER_H_
#define LAYER_H_

#include "updatequeue.h"

struct Layer {
  Layer(State *in, State *out);

  bool Update(UpdatePos pos);

  State *in;
  State *out;
};

struct AsconLinearLayer : public Layer {

};

struct AsconSboxLayer : public Layer {

};


#endif // LAYER_H_
