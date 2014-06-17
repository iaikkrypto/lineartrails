#include "layer.h"

SboxPos::SboxPos(uint8_t layer, uint8_t pos) : layer_(layer),pos_(pos){
}


//-----------------------------------------------------------------------------

Layer::Layer(StateMask *in, StateMask *out) : in(in), out(out) {
}

void Layer::SetMasks(StateMask *inmask, StateMask *outmask) {
  in = inmask;
  out = outmask;
}

//bool Layer::Update(UpdatePos pos) {
//  // TODO delegate to the correct steps.
//  // return false if contradictory
//  return true;
//}

//-----------------------------------------------------------------------------

LinearLayer::LinearLayer(StateMask *in, StateMask *out) : Layer(in, out) {
}


