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
#include "lrucache.h"


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
//#define ROTL(x,n) (((x)<<(n))|((x)>>(64-(n))))

//27,39,42,59,57, 2,13,63,23,31

template <unsigned round>
std::array<BitVector, 1> AsconSigma(std::array<BitVector, 1> in) {
  switch (round) {
    case 0: 
      return {in[0] ^ ROTR(in[0], 27) ^ ROTR(in[0], 39)};
    case 1: 
      return {in[0] ^ ROTR(in[0], 42) ^ ROTR(in[0], 59)};
    case 2: 
      return {in[0] ^ ROTR(in[0], 57) ^ ROTR(in[0],  2)};
    case 3: 
      return {in[0] ^ ROTR(in[0], 13) ^ ROTR(in[0], 63)};
    case 4: 
      return {in[0] ^ ROTR(in[0], 23) ^ ROTR(in[0], 31)};
    default: 
      return {0};
  }
}



struct AsconLinearLayer : public LinearLayer {
  AsconLinearLayer& operator=(const AsconLinearLayer& rhs);
  AsconLinearLayer();
  virtual AsconLinearLayer* clone();
  void Init();
  AsconLinearLayer(StateMask *in, StateMask *out);
  virtual bool Update(UpdatePos pos);
  int GetNumLayer();

  std::array<LinearStep<64, 1>, 5> sigmas;
  static std::unique_ptr<LRU_Cache<WordMaskArray<64, 1>, LinearStepUpdateInfo<64, 1>>> cache_[5];
};


struct AsconSboxLayer : public SboxLayer<5, 64> {
  AsconSboxLayer& operator=(const AsconSboxLayer& rhs);
  AsconSboxLayer();
  AsconSboxLayer(StateMask *in, StateMask *out);
  virtual AsconSboxLayer* clone();
  void InitSboxes();
  virtual bool Update(UpdatePos pos);
  Mask GetVerticalMask(int b, const StateMask& s) const;
  void SetVerticalMask(int b, StateMask& s, const Mask& mask);

 static std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> cache_;
};



#endif // ASCON_H_
