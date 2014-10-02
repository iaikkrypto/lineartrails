#include "layer.h"

SboxPos::SboxPos(uint8_t layer, uint8_t pos) : layer_(layer),pos_(pos){
}


//-----------------------------------------------------------------------------

Layer::Layer(StateMaskBase *in, StateMaskBase *out) : in(in), out(out) {
}

void Layer::SetMasks(StateMaskBase *inmask, StateMaskBase *outmask) {
  in = inmask;
  out = outmask;
}

//-----------------------------------------------------------------------------

LinearLayer::LinearLayer(StateMaskBase *in, StateMaskBase *out) : Layer(in, out) {
}

bool LinearLayer::Update(){
  bool ret_val = true;

  for(unsigned int i = 0; i < GetNumSteps(); ++i)
    ret_val &= updateStep(i);

  in->resetChangesLinear();
  out->resetChangesLinear();
  return ret_val;
}

//-----------------------------------------------------------------------------

SboxLayerBase::SboxLayerBase(StateMaskBase *in, StateMaskBase *out) : Layer(in, out) {
}


