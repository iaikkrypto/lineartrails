#ifndef ASCON_H_
#define ASCON_H_

#include "layer.h"
#include "mask.h"
#include "step_linear.h"
#include "step_nonlinear.h"


struct AsconState : public StateMask {
  typename std::array<Mask, 5>::iterator begin();
  typename std::array<Mask, 5>::const_iterator begin() const;
  typename std::array<Mask, 5>::iterator end();
  typename std::array<Mask, 5>::const_iterator end() const;
  Mask& operator[](const int index);
  std::array<Mask, 5> words;
};


#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))

template <unsigned round>
BitVector AsconSigma(BitVector in) {
#if (round == 0)
  return in ^ ROTR(in, 19) ^ ROTR(in, 28);
#elif (round == 1)
  return in ^ ROTR(in, 61) ^ ROTR(in, 39);
#elif (round == 2)
  return in ^ ROTR(in,  1) ^ ROTR(in,  6);
#elif (round == 3)
  return in ^ ROTR(in, 10) ^ ROTR(in, 17);
#elif (round == 4)
  return in ^ ROTR(in,  7) ^ ROTR(in, 41);
#else
#error Ascon Sigma: Bad round number
#endif
}

BitVector AsconSboxFun(BitVector in) {
  // TODO
  return in;
}


struct AsconLinearLayer : public Layer {
  AsconLinearLayer(StateMask *in, StateMask *out);
  bool Update(UpdatePos pos);

  std::array<LinearStep<64>, 5> sigmas;
};

struct AsconSboxLayer : public Layer {
  AsconSboxLayer(StateMask *in, StateMask *out);
  bool Update(UpdatePos pos);

  //std::array<NonlinearStep<5>, 64> sboxes;
  std::array<NonlinearStep, 64> sboxes;
};

#endif // ASCON_H_
