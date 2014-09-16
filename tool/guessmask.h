/*
 * guessmask.h
 *
 *  Created on: Jun 30, 2014
 *      Author: cdobraunig
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
};

typedef std::vector<Setting> Settings;


struct GuessMask {

  int createMask(Permutation *perm, Settings& settings);
  int getRandPos(SboxPos& box, bool& active);
  float getPushStackProb();
  float getSboxWeigthProb();
  float getSboxWeightHamming();


  std::list<std::tuple<SboxPos,float, bool>> weighted_pos_;
  float total_weight_;
  Setting* current_setting_;
};



#endif /* GUESSMASK_H_ */
