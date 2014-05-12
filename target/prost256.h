#ifndef PROST256_H_
#define PROST256_H_

#include <vector>
#include <array>
#include <random>
#include <iostream>
#include <iomanip>

#include "layer.h"
#include "mask.h"
#include "statemask.h"
#include "step_linear.h"
#include "step_nonlinear.h"
#include "lrucache.h"


struct Prost256State : public StateMask<16,32> {
  Prost256State();
  friend std::ostream& operator<<(std::ostream& stream, const Prost256State& statemask);
  void print(std::ostream& stream);
  virtual Prost256State* clone();
};


#define ROTR32(x,n) ((((x)>>(n))|((x)<<(32-(n))))&(~0ULL >> (32)))


struct Prost256SboxLayer : public SboxLayer<4, 128> {
  Prost256SboxLayer& operator=(const Prost256SboxLayer& rhs);
  Prost256SboxLayer();
  Prost256SboxLayer(StateMaskBase *in, StateMaskBase *out);
  virtual Prost256SboxLayer* clone();
  virtual bool updateStep(unsigned int step_pos);
  Mask GetVerticalMask(unsigned int b, const StateMaskBase& s) const;
  void SetVerticalMask(unsigned int b, StateMaskBase& s, const Mask& mask);

 static const unsigned int cache_size_ = { 0x1000 };
 static std::unique_ptr<LRU_Cache<unsigned long long,NonlinearStepUpdateInfo>> cache_;
 static std::shared_ptr<LinearDistributionTable<4>> ldt_;
};

template <unsigned parity>
struct Prost256LinearLayer : public LinearLayer {
  Prost256LinearLayer<parity>& operator=(const Prost256LinearLayer<parity>& rhs);
  Prost256LinearLayer();
  virtual Prost256LinearLayer<parity>* clone();
  void Init();
  Prost256LinearLayer(StateMaskBase *in, StateMaskBase *out);
  virtual bool updateStep(unsigned int step_pos);
  unsigned int GetNumSteps();
  virtual void copyValues(LinearLayer* other);

  static const unsigned int word_size_ = { 32 };
  static const unsigned int words_per_step_ = { 16 };
  static const unsigned int linear_steps_ = {1 };
  std::array<LinearStep<word_size_, words_per_step_>, linear_steps_> prost256_linear_;
};

//-----------------------------------------------------------------------------
template <unsigned parity>
std::array<BitVector, 16> Prost256Linear(std::array<BitVector, 16> in) {
  std::array<BitVector, 16> tmp;
  //the code used in this function is taken and modified from the reference version of Proest available at
  //http://bench.cr.yp.to/ebash.html

/* Multiply by MDS Matrix */
/*1000100100101011*/
  tmp[ 0] = in[ 0] ^ in[ 4] ^ in[ 7] ^ in[10] ^ in[12] ^ in[14] ^ in[15];
/*0100100000011001*/
  tmp[ 1] = in[ 1] ^ in[ 4] ^ in[11] ^ in[12] ^ in[15];
/*0010010011001000*/
  tmp[ 2] = in[ 2] ^ in[ 5] ^ in[ 8] ^ in[ 9] ^ in[12];
/*0001001001100100*/
  tmp[ 3] = in[ 3] ^ in[ 6] ^ in[ 9] ^ in[10] ^ in[13];
/*1001100010110010*/
  tmp[ 4] = in[ 0] ^ in[ 3] ^ in[ 4] ^ in[ 8] ^ in[10] ^ in[11] ^ in[14];
/*1000010010010001*/
  tmp[ 5] = in[ 0] ^ in[ 5] ^ in[ 8] ^ in[11] ^ in[15];
/*0100001010001100*/
  tmp[ 6] = in[ 1] ^ in[ 6] ^ in[ 8] ^ in[12] ^ in[13];
/*0010000101000110*/
  tmp[ 7] = in[ 2] ^ in[ 7] ^ in[ 9] ^ in[13] ^ in[14];
/*0010101110001001*/
  tmp[ 8] = in[ 2] ^ in[ 4] ^ in[ 6] ^ in[ 7] ^ in[ 8] ^ in[12] ^ in[15];
/*0001100101001000*/
  tmp[ 9] = in[ 3] ^ in[ 4] ^ in[ 7] ^ in[ 9] ^ in[12];
/*1100100000100100*/
  tmp[10] = in[ 0] ^ in[ 1] ^ in[ 4] ^ in[10] ^ in[13];
/*0110010000010010*/
  tmp[11] = in[ 1] ^ in[ 2] ^ in[ 5] ^ in[11] ^ in[14];
/*1011001010011000*/
  tmp[12] = in[ 0] ^ in[ 2] ^ in[ 3] ^ in[ 6] ^ in[ 8] ^ in[11] ^ in[12];
/*1001000110000100*/
  tmp[13] = in[ 0] ^ in[ 3] ^ in[ 7] ^ in[ 8] ^ in[13];
/*1000110001000010*/
  tmp[14] = in[ 0] ^ in[ 4] ^ in[ 5] ^ in[ 9] ^ in[14];
/*0100011000100001*/
  tmp[15] = in[ 1] ^ in[ 5] ^ in[ 6] ^ in[10] ^ in[15];

  if(!(parity & 1)) // even rounds
  {
    in[0*4+0] = tmp[0*4+0];
    in[0*4+1] = tmp[0*4+1];
    in[0*4+2] = tmp[0*4+2];
    in[0*4+3] = tmp[0*4+3];

    in[1*4+0] = ROTR32(tmp[1*4+0],4);
    in[1*4+1] = ROTR32(tmp[1*4+1],4);
    in[1*4+2] = ROTR32(tmp[1*4+2],4);
    in[1*4+3] = ROTR32(tmp[1*4+3],4);

    in[2*4+0] = ROTR32(tmp[2*4+0],12);
    in[2*4+1] = ROTR32(tmp[2*4+1],12);
    in[2*4+2] = ROTR32(tmp[2*4+2],12);
    in[2*4+3] = ROTR32(tmp[2*4+3],12);

    in[3*4+0] = ROTR32(tmp[3*4+0],26);
    in[3*4+1] = ROTR32(tmp[3*4+1],26);
    in[3*4+2] = ROTR32(tmp[3*4+2],26);
    in[3*4+3] = ROTR32(tmp[3*4+3],26);
  }
  else
  {
    in[0*4+0] = ROTR32(tmp[0*4+0],1);
    in[0*4+1] = ROTR32(tmp[0*4+1],1);
    in[0*4+2] = ROTR32(tmp[0*4+2],1);
    in[0*4+3] = ROTR32(tmp[0*4+3],1);

    in[1*4+0] = ROTR32(tmp[1*4+0],24);
    in[1*4+1] = ROTR32(tmp[1*4+1],24);
    in[1*4+2] = ROTR32(tmp[1*4+2],24);
    in[1*4+3] = ROTR32(tmp[1*4+3],24);

    in[2*4+0] = ROTR32(tmp[2*4+0],26);
    in[2*4+1] = ROTR32(tmp[2*4+1],26);
    in[2*4+2] = ROTR32(tmp[2*4+2],26);
    in[2*4+3] = ROTR32(tmp[2*4+3],26);

    in[3*4+0] = ROTR32(tmp[3*4+0],31);
    in[3*4+1] = ROTR32(tmp[3*4+1],31);
    in[3*4+2] = ROTR32(tmp[3*4+2],31);
    in[3*4+3] = ROTR32(tmp[3*4+3],31);
  }

   return in;
}

template <unsigned parity>
Prost256LinearLayer<parity>& Prost256LinearLayer<parity>::operator=(const Prost256LinearLayer<parity>& rhs) {
  prost256_linear_ = rhs.prost256_linear_;
  return *this;
}

template <unsigned parity>
Prost256LinearLayer<parity>::Prost256LinearLayer() {
  Init();
}

template <unsigned parity>
unsigned int Prost256LinearLayer<parity>::GetNumSteps() {
  return linear_steps_;
}

template <unsigned parity>
Prost256LinearLayer<parity>* Prost256LinearLayer<parity>::clone() {
  //TODO: write copy constructor
  Prost256LinearLayer<parity>* obj = new Prost256LinearLayer<parity>(in, out);
  obj->prost256_linear_ = this->prost256_linear_;
  return obj;
}

template <unsigned parity>
Prost256LinearLayer<parity>::Prost256LinearLayer(StateMaskBase *in, StateMaskBase *out)
    : LinearLayer(in, out) {
  Init();
}

template <unsigned parity>
void Prost256LinearLayer<parity>::Init() {
  prost256_linear_[0].Initialize(Prost256Linear<parity>);

}

template <unsigned parity>
bool Prost256LinearLayer<parity>::updateStep(unsigned int step_pos) {
assert(step_pos <= linear_steps_);
bool ret_val;
ret_val = prost256_linear_[step_pos].Update( {
        &((*in)[0]), &((*in)[1]), &((*in)[2]), &((*in)[3]), &((*in)[4]),
        &((*in)[5]), &((*in)[6]), &((*in)[7]), &((*in)[8]), &((*in)[9]),
        &((*in)[10]), &((*in)[11]), &((*in)[12]), &((*in)[13]), &((*in)[14]), &((*in)[15])},
       {&((*out)[0]), &((*out)[1]), &((*out)[2]), &((*out)[3]), &((*out)[4]),
        &((*out)[5]), &((*out)[6]), &((*out)[7]), &((*out)[8]), &((*out)[9]),
        &((*out)[10]), &((*out)[11]), &((*out)[12]), &((*out)[13]), &((*out)[14]), &((*out)[15])});


//  TODO: Test faster update
//for(int i = 0; i < 16; ++i){
//  in->getWordSbox(i) |= (*in)[i].changes_;
//  out->getWordSbox(i) |= (*out)[i].changes_;
//}
return ret_val;
}

template <unsigned parity>
void Prost256LinearLayer<parity>::copyValues(LinearLayer* other){
  Prost256LinearLayer* ptr = dynamic_cast<Prost256LinearLayer*> (other);
  prost256_linear_ = ptr->prost256_linear_;
}



#endif // PROST256_H_
