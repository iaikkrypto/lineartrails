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
#include <memory>
#include "ascon_permutation.h"

AsconPermutation::AsconPermutation(unsigned int rounds)
    : Permutation(rounds) {
  for (unsigned int i = 0; i < 2 * rounds + 1; ++i) {
    this->state_masks_[i].reset(new AsconState);
    this->saved_state_masks_[i].reset(new AsconState);
  }
  for (unsigned int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(new AsconSboxLayer);
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2 * i].get(),
                                    this->state_masks_[2 * i + 1].get());
    this->linear_layers_[i].reset(new AsconLinearLayer);
    this->linear_layers_[i]->SetMasks(this->state_masks_[2 * i + 1].get(),
                                      this->state_masks_[2 * i + 2].get());
    this->saved_sbox_layers_[i].reset(new AsconSboxLayer);
    this->saved_sbox_layers_[i]->SetMasks(
        this->saved_state_masks_[2 * i].get(),
        this->saved_state_masks_[2 * i + 1].get());
    this->saved_linear_layers_[i].reset(new AsconLinearLayer);
    this->saved_linear_layers_[i]->SetMasks(
        this->saved_state_masks_[2 * i + 1].get(),
        this->saved_state_masks_[2 * i + 2].get());
  }
  touchall();
}

AsconPermutation::AsconPermutation(const AsconPermutation& other)
    : Permutation(other) {

  for (unsigned int i = 0; i < 2 * rounds_ + 1; ++i) {
    this->saved_state_masks_[i].reset(new AsconState);
  }
  for (unsigned int i = 0; i < rounds_; ++i) {
    this->saved_sbox_layers_[i].reset(new AsconSboxLayer);
    this->saved_sbox_layers_[i]->SetMasks(
        this->saved_state_masks_[2 * i].get(),
        this->saved_state_masks_[2 * i + 1].get());
    this->saved_linear_layers_[i].reset(new AsconLinearLayer);
    this->saved_linear_layers_[i]->SetMasks(
        this->saved_state_masks_[2 * i + 1].get(),
        this->saved_state_masks_[2 * i + 2].get());
  }
}

Permutation::PermPtr AsconPermutation::clone() const {
  return PermPtr(new AsconPermutation(*this));
}

void AsconPermutation::PrintWithProbability(std::ostream& stream,
                                            unsigned int offset) {
  Permutation::PrintWithProbability(stream, 0);
}

void AsconPermutation::touchall() {
  Permutation::touchall();
}

