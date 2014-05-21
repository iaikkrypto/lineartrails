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
#include "permutation.h"
#include "memory"


struct AsconState : public StateMask {
  AsconState();
  AsconState& operator=(AsconState rhs);
  std::vector<UpdatePos> diff(const StateMask& other);
  typename std::array<Mask, 5>::iterator begin();
  typename std::array<Mask, 5>::const_iterator begin() const;
  typename std::array<Mask, 5>::iterator end();
  typename std::array<Mask, 5>::const_iterator end() const;
  Mask& operator[](const int index);
  const Mask& operator[](const int index) const;
  friend std::ostream& operator<<(std::ostream& stream, const AsconState& statemask);
  void SetState(BitMask value);
  std::array<Mask, 5> words;
};


#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))

template <unsigned round>
BitVector AsconSigma(BitVector in) {
  switch (round) {
    case 0: 
      return in ^ ROTR(in, 19) ^ ROTR(in, 28);
    case 1: 
      return in ^ ROTR(in, 61) ^ ROTR(in, 39);
    case 2: 
      return in ^ ROTR(in,  1) ^ ROTR(in,  6);
    case 3: 
      return in ^ ROTR(in, 10) ^ ROTR(in, 17);
    case 4: 
      return in ^ ROTR(in,  7) ^ ROTR(in, 41);
    default: 
      return 0;
  }
}



struct AsconLinearLayer : public Layer {
  AsconLinearLayer& operator=(const AsconLinearLayer& rhs);
  AsconLinearLayer(StateMask *in, StateMask *out);
  virtual bool Update(UpdatePos pos);

  std::array<LinearStep<64>, 5> sigmas;
};

struct AsconSboxLayer : public Layer {
  AsconSboxLayer& operator=(const AsconSboxLayer& rhs);
  AsconSboxLayer(StateMask *in, StateMask *out);
  virtual bool Update(UpdatePos pos);
  void GuessBox(UpdatePos pos);
  Mask GetVerticalMask(int b, const StateMask& s) const;
  void SetVerticalMask(int b, StateMask& s, const Mask& mask);
  std::array<NonlinearStep<5>, 64> sboxes;
};

struct AsconPermutation : public Permutation {
  AsconPermutation& operator=(AsconPermutation& rhs);
  AsconPermutation(int number_steps);
  virtual bool checkchar();
  bool randomsboxguess();
  bool anythingtoguess();
  virtual bool update();
  void touchall();
  friend std::ostream& operator<<(std::ostream& stream, const AsconPermutation& permutation);

  std::vector<AsconSboxLayer> sbox_layers_;
  std::vector<AsconLinearLayer> linear_layers_;
  std::vector<AsconState> state_masks_;
  bool toupdate_linear;
  bool toupdate_nonlinear;
};

#endif // ASCON_H_
