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

typedef std::vector<std::vector<std::array<float, 2>>> GuessWeights;

struct GuessMask {

  int createMask(Permutation *perm, GuessWeights& weights);
  int getRandPos(SboxPos& box, bool& active);


  std::list<std::tuple<SboxPos,float, bool>> weighted_pos_;
  float total_weight_;
};



#endif /* GUESSMASK_H_ */
