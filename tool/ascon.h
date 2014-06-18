#ifndef ASCON_H_
#define ASCON_H_

#include <vector>
#include <array>
#include <random>

#include "layer.h"
#include "mask.h"
#include "step_linear.h"
#include "step_nonlinear.h"
#include "updatequeue.h"
#include "memory"


struct AsconState : public StateMask {
  AsconState();
  AsconState& operator=(const AsconState& rhs);
  std::vector<UpdatePos> diff(const StateMask& other);
  typename std::array<Mask, 5>::iterator begin();
  typename std::array<Mask, 5>::const_iterator begin() const;
  typename std::array<Mask, 5>::iterator end();
  typename std::array<Mask, 5>::const_iterator end() const;
  Mask& operator[](const int index);
  const Mask& operator[](const int index) const;
  friend std::ostream& operator<<(std::ostream& stream, const AsconState& statemask);
  void print(std::ostream& stream);
  virtual AsconState* clone();
  void SetState(BitMask value);
  void SetBit(BitMask value, int word_pos, int bit_pos);
  std::array<Mask, 5> words;
};


#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))
#define ROTL(x,n) (((x)<<(n))|((x)>>(64-(n))))

template <unsigned round>
BitVector AsconSigma(BitVector in) {
  switch (round) {
    case 0: 
      return in ^ ROTL(in, 19) ^ ROTL(in, 28);
    case 1: 
      return in ^ ROTL(in, 61) ^ ROTL(in, 39);
    case 2: 
      return in ^ ROTL(in,  1) ^ ROTL(in,  6);
    case 3: 
      return in ^ ROTL(in, 10) ^ ROTL(in, 17);
    case 4: 
      return in ^ ROTL(in,  7) ^ ROTL(in, 41);
    default: 
      return 0;
  }
}



struct AsconLinearLayer : public LinearLayer {
  AsconLinearLayer& operator=(const AsconLinearLayer& rhs);
  AsconLinearLayer();
  virtual AsconLinearLayer* clone();
  void Init();
  AsconLinearLayer(StateMask *in, StateMask *out);
  virtual bool Update(UpdatePos pos);

  std::array<LinearStep<64>, 5> sigmas;
};


struct AsconSboxLayer : public SboxLayer<5, 64> {
  AsconSboxLayer& operator=(const AsconSboxLayer& rhs);
  AsconSboxLayer();
  AsconSboxLayer(StateMask *in, StateMask *out);
  virtual AsconSboxLayer* clone();
  void InitSboxes();
  Mask GetVerticalMask(int b, const StateMask& s) const;
  void SetVerticalMask(int b, StateMask& s, const Mask& mask);
};



#endif // ASCON_H_
