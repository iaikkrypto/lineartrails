#include "ascon.h"

typename std::array<Mask, 5>::iterator AsconState::begin() {
  return words.begin();
}

typename std::array<Mask, 5>::const_iterator AsconState::begin() const {
  return words.begin();
}

typename std::array<Mask, 5>::iterator AsconState::end() {
  return words.end();
}

typename std::array<Mask, 5>::const_iterator AsconState::end() const {
  return words.end();
}

Mask& AsconState::operator[](const int index) {
  return words[index];
}

//-----------------------------------------------------------------------------

AsconLinearLayer::AsconLinearLayer(StateMask *in, StateMask *out) : Layer(in, out) {
    //: Layer(in, out)
    //, sigmas{{{AsconSigma<0>}, {AsconSigma<1>}, {AsconSigma<2>}, {AsconSigma<3>}, {AsconSigma<4>}}} {
  sigmas[0].Initialize(AsconSigma<0>);
  sigmas[1].Initialize(AsconSigma<1>);
  sigmas[2].Initialize(AsconSigma<2>);
  sigmas[3].Initialize(AsconSigma<3>);
  sigmas[4].Initialize(AsconSigma<4>);
}

bool AsconLinearLayer::Update(UpdatePos pos) {
  return sigmas[pos.word].Update((*in)[pos.word], (*out)[pos.word]);
}

//-----------------------------------------------------------------------------

AsconSboxLayer::AsconSboxLayer(StateMask *in, StateMask *out) : Layer(in, out) {
  // TODO
}

bool AsconSboxLayer::Update(UpdatePos pos) {
  // TODO
  return true;
}
