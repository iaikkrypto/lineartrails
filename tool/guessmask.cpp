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
#include "guessmask.h"

int GuessMask::createMask(Permutation *perm, Settings& settings){

  weighted_pos_.clear();
  total_weight_ = 0;

  perm->SboxStatus(temp_active_boxes_[1], temp_active_boxes_[0]);

  for (auto& set : settings) {
    for (int i = 0; i < 2; ++i) {
      for (auto& box : temp_active_boxes_[i]) {
        if (set.guess_weights_[box.layer_][i] != 0) {
          total_weight_ += set.guess_weights_[box.layer_][i];
          weighted_pos_.push_back(std::tuple<SboxPos, float, bool>(box, set.guess_weights_[box.layer_][i], i==1 ));
        }
      }
    }
    if(total_weight_ != 0){
      current_setting_ = &set;
      return 1;
    }
  }

  return 0;
}
int GuessMask::getRandPos(SboxPos& box, bool& active) {
  std::uniform_real_distribution<float> guessbox(0, total_weight_);

  std::mt19937 generator(
          std::chrono::high_resolution_clock::now().time_since_epoch().count());
  float rand = guessbox(generator);
  float current_weight = 0;
  for (auto it = weighted_pos_.begin(); it != weighted_pos_.end(); ++it) {
    current_weight += std::get<1>(*it);
    if (current_weight >= rand) {
      box = std::get<0>(*it);
      active = std::get<2>(*it);
      total_weight_ -= std::get<1>(*it);
      weighted_pos_.erase(it);
      return 1;
    }
  }

  return 0;
}

float GuessMask::getPushStackProb(){
return current_setting_->push_stack_probability_;
}
float GuessMask::getSboxWeigthProb(){
  return current_setting_->sbox_weight_probability_;
}
float GuessMask::getSboxWeightHamming(){
  return current_setting_->sbox_weight_hamming_;
}

unsigned int GuessMask::getAlternativeSboxGuesses(){
  return current_setting_->alternative_sbox_guesses_;
}
