/*
 * statemask.hpp
 *
 *  Created on: Sep 4, 2014
 *      Author: cdobraunig
 */

template<unsigned words, unsigned bits>
StateMask<words, bits>::StateMask() {
  for (int i = 0; i < words; ++i)
    words_[i].reset(bits);
}

template<unsigned words, unsigned bits>
Mask& StateMask<words, bits>::operator[](const int index) {
  return words_[index];
}

template<unsigned words, unsigned bits>
const Mask& StateMask<words, bits>::operator[](const int index) const {
  return words_[index];
}

template<unsigned words, unsigned bits>
void StateMask<words, bits>::SetState(BitMask value) {
  for (int j = 0; j < words; ++j) {
    for (int i = 0; i < bits; ++i)
      words_[j].bitmasks[i] = value;
    words_[j].reinit_caremask();
  }
}

template<unsigned words, unsigned bits>
void StateMask<words, bits>::SetBit(BitMask value, int word_pos, int bit_pos) {
  words_.at(word_pos).set_bit(value, bit_pos);
}

template<unsigned words, unsigned bits>
bool StateMask<words, bits>::diffLinear(const StateMaskBase& other) {
  return diff(other, changes_for_linear_layer_);
}

template<unsigned words, unsigned bits>
bool StateMask<words, bits>::diffSbox(const StateMaskBase& other) {
return diff(other, changes_for_sbox_layer_);
}

template<unsigned words, unsigned bits>
bool StateMask<words, bits>::diff(
    const StateMaskBase& other,
    std::array<unsigned long long, words>& changes) {

  for (int i = 0; i < words; ++i) {
    changes[i] |= (words_[i].caremask.canbe1 ^ other[i].caremask.canbe1)
        | (words_[i].caremask.care ^ other[i].caremask.care);
  }

  //TODO: Maybe do not need this if using a better update
  for (int i = 0; i < words; ++i)
    if(changes[i] != 0)
      return true;

  return false;
}

template<unsigned words, unsigned bits>
void StateMask<words, bits>::resetChangesLinear() {
  resetChanges(changes_for_linear_layer_);
}

template<unsigned words, unsigned bits>
void StateMask<words, bits>::resetChangesSbox() {
  resetChanges(changes_for_sbox_layer_);
}

template<unsigned words, unsigned bits>
void StateMask<words, bits>::resetChanges(
    std::array<unsigned long long, words>& changes) {
  for (int i = 0; i < words; ++i)
      changes[i] = 0;
}

template<unsigned words, unsigned bits>
unsigned long long StateMask<words, bits>::getWordLinear(
    const int index) {

  return changes_for_linear_layer_[index];
}

template<unsigned words, unsigned bits>
unsigned long long StateMask<words, bits>::getWordSbox(
    const int index) {

  return changes_for_sbox_layer_[index];
}
