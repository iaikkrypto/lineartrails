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

//bool Layer::Update(UpdatePos pos) {
//  // TODO delegate to the correct steps.
//  // return false if contradictory
//  return true;
//}

//-----------------------------------------------------------------------------

LinearLayer::LinearLayer(StateMaskBase *in, StateMaskBase *out) : Layer(in, out) {
}

//-----------------------------------------------------------------------------

SboxLayerBase::SboxLayerBase(StateMaskBase *in, StateMaskBase *out) : Layer(in, out) {
}


