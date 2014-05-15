#include "layer.h"

Layer::Layer(State *in, State *out) : in(in), out(out) {
}

bool Layer::Update(UpdatePos pos) {
  // TODO delegate to the correct steps.
  // return false if contradictory
}
