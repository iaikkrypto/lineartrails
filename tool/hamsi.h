#ifndef HAMSI_H_
#define HAMSI_H_

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


struct HamsiState : public StateMask {
  HamsiState();
  HamsiState& operator=(const HamsiState& rhs);
  std::vector<UpdatePos> diff(const StateMask& other);
  typename std::array<Mask, 5>::iterator begin();
  typename std::array<Mask, 5>::const_iterator begin() const;
  typename std::array<Mask, 5>::iterator end();
  typename std::array<Mask, 5>::const_iterator end() const;
  Mask& operator[](const int index);
  const Mask& operator[](const int index) const;
  friend std::ostream& operator<<(std::ostream& stream, const HamsiState& statemask);
  void print(std::ostream& stream);
  virtual HamsiState* clone();
  void SetState(BitMask value);
  void SetBit(BitMask value, int word_pos, int bit_pos);
  std::array<Mask, 16> words;
};


#define ROTL32(x,n) (((x)<<(n))|((x)>>(32-(n))))



struct HamsiLinearLayer : public LinearLayer {
  HamsiLinearLayer& operator=(const HamsiLinearLayer& rhs);
  HamsiLinearLayer();
  virtual HamsiLinearLayer* clone();
  void Init();
  HamsiLinearLayer(StateMask *in, StateMask *out);
  virtual bool Update(UpdatePos pos);
  int GetNumLayer();

  std::array<LinearStep<64, 4>, 4> layers;
  static std::unique_ptr<LRU_Cache<WordMaskArray<64, 4>, LinearStepUpdateInfo<64, 4>>> cache_[1];
};


struct HamsiSboxLayer : public SboxLayer<4, 128> {
  HamsiSboxLayer& operator=(const HamsiSboxLayer& rhs);
  HamsiSboxLayer();
  HamsiSboxLayer(StateMask *in, StateMask *out);
  virtual HamsiSboxLayer* clone();
  void InitSboxes();
  virtual bool Update(UpdatePos pos);
  Mask GetVerticalMask(int b, const StateMask& s) const;
  void SetVerticalMask(int b, StateMask& s, const Mask& mask);

 static std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> cache_;
};



#endif // HAMSI_H_
