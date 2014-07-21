/*
 * search.h
 *
 *  Created on: Jun 11, 2014
 *      Author: cdobraunig
 */

#ifndef SEARCH_H_
#define SEARCH_H_

#include <memory>
#include <chrono>
#include <random>
#include <cfloat>
#include <vector>
#include <assert.h>
#include <stack>

#include "permutation.h"
#include "mask.h"
#include "guessmask.h"

class Search {

 public:
  Search(PermutationBase &perm);
  void RandomSearch1(unsigned int iterations, std::function<int(int, int, int)> rating);
  void HeuristicSearch1(unsigned int iterations, GuessWeights weights, std::function<int(int, int, int)> rating, int try_one_box, bool count_active);
  void HeuristicSearch2(unsigned int iterations, GuessWeights weights, std::function<int(int, int, int)> rating, int try_one_box, bool count_active);
  void HeuristicSearch3(unsigned int iterations, GuessWeights weights, std::function<int(int, int, int)> rating, int try_one_box, bool count_active);
  void StackSearch1(unsigned int iterations, GuessWeights weights, std::function<int(int, int, int)> rating, int try_one_box, bool count_active, float push_stack_prob, int printintervall, unsigned int credits);

  PermutationBase *perm_;

};

#endif /* SEARCH_H_ */
