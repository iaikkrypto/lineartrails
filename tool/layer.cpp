#include "layer.h"

SboxPos::SboxPos(uint8_t layer, uint8_t pos) : layer_(layer),pos_(pos){
}


//-----------------------------------------------------------------------------

Layer::Layer(StateMask *in, StateMask *out) : in(in), out(out) {
}

//bool Layer::Update(UpdatePos pos) {
//  // TODO delegate to the correct steps.
//  // return false if contradictory
//  return true;
//}

void Layer::GuessBox(UpdatePos pos){

}
