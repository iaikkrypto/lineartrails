/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
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
  virtual bool diffLinear(const StateMaskBase& other) = 0;
  virtual bool diffSbox(const StateMaskBase& other) = 0;
  virtual void copyValues(const StateMaskBase* other) = 0;
  virtual void resetChangesLinear() = 0;
  virtual void resetChangesSbox() = 0;
  virtual bool changesforLinear() = 0;
  virtual bool changesforSbox() = 0;
  virtual unsigned long long& getWordLinear(const int index) = 0;
  virtual unsigned long long& getWordSbox(const int index) = 0;
  virtual Mask& operator[](const int index) = 0;
  virtual const Mask& operator[](const int index) const = 0;
  virtual const unsigned int getnumwords() const = 0;
  virtual const unsigned int getnumbits() const = 0;
// TODO: Work on faster update
//  virtual unsigned long long int getChangesforLinearLayer(const int i) const = 0;
//  virtual unsigned long long getChangesforSboxLayer(const int i) const = 0;
};

template <unsigned words, unsigned bits>
struct StateMask : public StateMaskBase {
  StateMask();
  virtual ~StateMask(){};
  virtual StateMask<words,bits>* clone() = 0;
  virtual void print(std::ostream& stream) = 0;
  virtual void SetState(BitMask value);
  virtual void SetBit(BitMask value, int word_pos, int bit_pos);
  virtual bool diffLinear(const StateMaskBase& other);
  virtual bool diffSbox(const StateMaskBase& other);
  virtual void copyValues(const StateMaskBase* other);
  virtual void resetChangesLinear();
  virtual void resetChangesSbox();
  void resetChanges(std::array<unsigned long long, words>& changes);
  virtual bool changesforLinear();
  virtual bool changesforSbox();
  virtual unsigned long long& getWordLinear(const int index);
  virtual unsigned long long& getWordSbox(const int index);
  virtual Mask& operator[](const int index);
  virtual const Mask& operator[](const int index) const ;
  virtual const unsigned int getnumwords() const {return words;};
  virtual const unsigned int getnumbits() const {return bits;};
// TODO: Work on faster update
//  virtual unsigned long long int getChangesforLinearLayer(const int i) const;
//  virtual unsigned long long getChangesforSboxLayer(const int i) const;

 protected:
  bool diff(const StateMaskBase& other, std::array<unsigned long long, words>& changes);

public:
  std::array<Mask, words> words_;
  std::array<unsigned long long, words> changes_for_linear_layer_;
  std::array<unsigned long long, words> changes_for_sbox_layer_;
};

#include "statemask.hpp"

#endif /* STATEMASK_H_ */
