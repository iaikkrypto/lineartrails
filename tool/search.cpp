/*
 * search.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: cdobraunig
 */

#include "search.h"

Search::Search(Permutation &perm) : perm_(&perm) {

}

void Search::RandomSearch1(unsigned int iterations) {
  std::unique_ptr<Permutation> working_copy, temp_copy;
  double best_prob = -DBL_MAX;
  working_copy.reset(perm_->clone());
  working_copy->checkchar();

  std::vector<SboxPos> active;
  std::vector<SboxPos> inactive;

  std::mt19937 generator(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());

  for (unsigned int i = 0; i < iterations; ++i) {
    temp_copy.reset(working_copy->clone());
    temp_copy->SboxStatus(active, inactive);
    while (active.size() != 0 || inactive.size() != 0) {
      while (inactive.size() != 0) {
        std::uniform_int_distribution<int> guessbox(0, inactive.size() - 1);
        if (temp_copy->guessbestsbox(inactive[guessbox(generator)])
            == false) {
          temp_copy.reset(working_copy->clone());
          active.clear();
          break;
        }
        temp_copy->SboxStatus(active, inactive);
      }

      while (active.size() != 0) {
        std::uniform_int_distribution<int> guessbox(0, active.size() - 1);
        if (temp_copy->guessbestsbox(active[guessbox(generator)]) == false) {
          temp_copy.reset(working_copy->clone());
          temp_copy->SboxStatus(active, inactive);
          break;
        }
        temp_copy->SboxStatus(active, inactive);
      }
    }
    double current_prob = temp_copy->GetProbability().bias;
    if (current_prob > best_prob) {
      best_prob = current_prob;
      std::cout << "iteration: " << i << std::endl;
      temp_copy->PrintWithProbability();
    }
  }
}
