/*
 * guessmask.cpp
 *
 *  Created on: Jun 30, 2014
 *      Author: cdobraunig
 */

#include "guessmask.h"

int GuessMask::createMask(PermutationBase *perm, GuessWeights& weights){
std::array<std::vector<SboxPos>,2> active_boxes;

  weighted_pos_.clear();
  total_weight_ = 0;

  perm->SboxStatus(active_boxes[1], active_boxes[0]);

  for (auto& set : weights) {
    for (int i = 0; i < 2; ++i) {
      for (auto& box : active_boxes[i]) {
        if (set[box.layer_][i] != 0) {
          total_weight_ += set[box.layer_][i];
          weighted_pos_.push_back(std::tuple<SboxPos, int, bool>(box, set[box.layer_][i], i==1 ));
        }
      }
    }
    if(total_weight_ != 0)
      return 1;
  }

  return 0;
}
int GuessMask::getRandPos(SboxPos& box, bool& active) {
  std::uniform_int_distribution<int> guessbox(0, total_weight_);

  std::mt19937 generator(
          std::chrono::high_resolution_clock::now().time_since_epoch().count());
  int rand = guessbox(generator);
  int current_weight = 0;
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
