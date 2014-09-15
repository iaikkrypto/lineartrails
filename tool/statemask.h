/*
 * statemask.h
 *
 *  Created on: Sep 4, 2014
 *      Author: cdobraunig
 */

#ifndef STATEMASK_H_
#define STATEMASK_H_

#include <array>

struct StateMaskBase {
  virtual ~StateMaskBase(){};
  virtual StateMaskBase* clone() = 0;
  virtual void print(std::ostream& stream) = 0;
  virtual void SetState(BitMask value) = 0;
  virtual void SetBit(BitMask value, int word_pos, int bit_pos) = 0;
  virtual std::vector<UpdatePos> diff(const StateMaskBase& other) = 0;
  virtual Mask& operator[](const int index) = 0;
  virtual const Mask& operator[](const int index) const = 0;
  virtual const unsigned int getnumwords() const = 0;
  virtual const unsigned int getnumbits() const = 0;
};

template <unsigned words, unsigned bits>
struct StateMask : public StateMaskBase {
  StateMask();
  virtual ~StateMask(){};
  virtual StateMask<words,bits>* clone() = 0;
  virtual void print(std::ostream& stream) = 0;
  virtual void SetState(BitMask value);
  virtual void SetBit(BitMask value, int word_pos, int bit_pos);
  virtual std::vector<UpdatePos> diff(const StateMaskBase& other);
  virtual Mask& operator[](const int index);
  virtual const Mask& operator[](const int index) const ;
  virtual const unsigned int getnumwords() const {return words;};
  virtual const unsigned int getnumbits() const {return bits;};

 protected:
  std::array<Mask, words> words_;
};

#include "statemask.hpp"

#endif /* STATEMASK_H_ */
