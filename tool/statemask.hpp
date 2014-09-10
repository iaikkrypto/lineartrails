/*
 * statemask.hpp
 *
 *  Created on: Sep 4, 2014
 *      Author: cdobraunig
 */

template <unsigned words, unsigned bits>
StateMask<words, bits>::StateMask(){
  for (int i = 0; i < words; ++i)
    words_[i].reset(bits);
}

template <unsigned words, unsigned bits>
Mask& StateMask<words, bits>::operator[](const int index) {
  return words_[index];
}

template <unsigned words, unsigned bits>
const Mask& StateMask<words, bits>::operator[](const int index) const {
  return words_[index];
}

template <unsigned words, unsigned bits>
void StateMask<words, bits>::SetState(BitMask value){
  for(int j = 0; j< words; ++j){
    for(int i = 0; i< bits; ++i)
      words_[j].bitmasks[i] = value;
    words_[j].reinit_caremask();
  }
}

template <unsigned words, unsigned bits>
void StateMask<words, bits>::SetBit(BitMask value, int word_pos, int bit_pos){
  words_.at(word_pos).set_bit(value, bit_pos);
}

template <unsigned words, unsigned bits>
std::vector<UpdatePos> StateMask<words, bits>::diff(const StateMaskBase& other) {
  BitVector diffword;
  std::vector<UpdatePos> result;
  for (int i = 0; i < words; ++i) {
    diffword = (words_[i].caremask.canbe1 ^ other[i].caremask.canbe1) | (words_[i].caremask.care ^ other[i].caremask.care);
    for (int b = 0; b < bits && diffword; ++b) {
      if (diffword & 1)
        result.emplace_back(0, i, b, 0);
      diffword >>= 1;
    }
  }
  return result;
}
