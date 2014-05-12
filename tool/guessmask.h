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
#ifndef GUESSMASK_H_
#define GUESSMASK_H_

#include <list>
#include <vector>
#include <array>
#include <chrono>
#include <random>
#include <tuple>

#include "layer.h"
#include "permutation.h"



struct Setting {
  std::vector<std::array<float, 2>> guess_weights_;
  float push_stack_probability_;
  float sbox_weight_probability_;
  float sbox_weight_hamming_;
  unsigned int alternative_sbox_guesses_;
};

typedef std::vector<Setting> Settings;


struct GuessMask {

  int createMask(Permutation *perm, Settings& settings);
  int getRandPos(SboxPos& box, bool& active);
  float getPushStackProb();
  float getSboxWeigthProb();
  float getSboxWeightHamming();
  unsigned int getAlternativeSboxGuesses();


  std::list<std::tuple<SboxPos,float, bool>> weighted_pos_;
  float total_weight_;
  Setting* current_setting_;

 private:
  std::array<std::vector<SboxPos>,2> temp_active_boxes_;
};



#endif /* GUESSMASK_H_ */
