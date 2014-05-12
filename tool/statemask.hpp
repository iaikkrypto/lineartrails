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
template<unsigned words, unsigned bits>
StateMask<words, bits>::StateMask() {
  for (unsigned int i = 0; i < words; ++i){
    words_[i].reset(bits);
    changes_for_linear_layer_[i] = ~0ULL;
    changes_for_sbox_layer_[i] = ~0ULL;
  }
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
  for (unsigned int j = 0; j < words; ++j) {
    for (unsigned int i = 0; i < bits; ++i)
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

  for (unsigned int i = 0; i < words; ++i) {
    changes[i] |= (words_[i].caremask.canbe1 ^ other[i].caremask.canbe1)
        | (words_[i].caremask.care ^ other[i].caremask.care);
  }

  //TODO: Maybe do not need this if using a better update
  for (unsigned int i = 0; i < words; ++i)
    if(changes[i] != 0)
      return true;

  return false;
}

template<unsigned words, unsigned bits>
bool StateMask<words, bits>::changesforLinear() {
  for (unsigned int i = 0; i < words; ++i)
    if(changes_for_linear_layer_[i] != 0)
      return true;

  return false;
}

template<unsigned words, unsigned bits>
bool StateMask<words, bits>::changesforSbox() {
  for (unsigned int i = 0; i < words; ++i)
    if(changes_for_sbox_layer_[i] != 0)
      return true;

  return false;
}

template<unsigned words, unsigned bits>
void StateMask<words, bits>::copyValues(const StateMaskBase* other) {
  //TODO: maybe use static cast
  const StateMask<words, bits>* ptr = dynamic_cast<const StateMask<words, bits>*>(other);
  words_ = ptr->words_;
  changes_for_linear_layer_ = ptr->changes_for_linear_layer_;
  changes_for_sbox_layer_ = ptr->changes_for_sbox_layer_;

// TODO: Work on faster update
//  for(int i = 0; i< words ; ++i){
//  words_[i] = other[i];
//  changes_for_linear_layer_[i] = other.getChangesforLinearLayer(i);
//  changes_for_sbox_layer_[i] = other.getChangesforSboxLayer(i);
//  }
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
  for (unsigned int i = 0; i < words; ++i)
      changes[i] = 0;
}

template<unsigned words, unsigned bits>
unsigned long long& StateMask<words, bits>::getWordLinear(
    const int index) {

  return changes_for_linear_layer_[index];
}

template<unsigned words, unsigned bits>
unsigned long long& StateMask<words, bits>::getWordSbox(
    const int index) {

  return changes_for_sbox_layer_[index];
}

// TODO: Work on faster update
//template<unsigned words, unsigned bits>
//unsigned long long int StateMask<words, bits>::getChangesforLinearLayer(
//     int index) {
//
//  return changes_for_linear_layer_[index];
//}
//
//template<unsigned words, unsigned bits>
//unsigned long long StateMask<words, bits>::getChangesforSboxLayer(
//     int index) {
//
//  return changes_for_sbox_layer_[index];
//}

