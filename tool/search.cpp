/*
 * search.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: cdobraunig
 */

#include "search.h"

Search::Search(PermutationBase &perm) : perm_(&perm) {

}

void Search::RandomSearch1(unsigned int iterations) {
  std::unique_ptr<PermutationBase> working_copy, temp_copy;
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
        if (temp_copy->guessbestsbox(inactive[guessbox(generator)]) == false) {
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

//Search without stack using weighted guesses
//TODO: Separate finding guess pos from actual guessing
void Search::HeuristicSearch1(unsigned int iterations, std::vector<std::vector<std::array<int, 2>>>weights, int try_one_box) {

  std::unique_ptr<PermutationBase> working_copy;
  std::unique_ptr<PermutationBase> temp_copy;
  double best_prob = -DBL_MAX;
  working_copy.reset(perm_->clone());
  working_copy->checkchar();

  std::array<std::vector<std::vector<SboxPos>>,2> active_boxes;

  working_copy->SboxStatus(active_boxes[1], active_boxes[0]);

  size_t num_rounds = active_boxes[1].size();
  size_t num_settings = weights.size();

  std::mt19937 generator(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());

  for (unsigned int i = 0; i < iterations; ++i) {
    bool fail = true;
    while (fail){
      std::vector<std::vector<std::array<int,2>>> working_weights = weights;
      std::vector<int> total_weights;

      temp_copy.reset(working_copy->clone());
      for(auto& setting : weights){
        assert(setting.size() == num_rounds);
        int total_weight = 0;
        for(auto& layer : setting)
          total_weight += layer[0] + layer[1];
        total_weights.push_back(total_weight);
      }

      size_t current_setting = 0;
      while(current_setting < num_settings){
        fail = false;
        temp_copy->SboxStatus(active_boxes[1], active_boxes[0]);

        std::uniform_int_distribution<int> distr_setting(0,total_weights[current_setting]);
        int random_weight = distr_setting(generator);

        for(int active = 0; active <= 1; ++active){
            for(size_t layer = 0; layer < working_weights[current_setting].size(); ++layer){
              random_weight -= working_weights[current_setting][layer][active];
              if(random_weight < 1){
                  if(active_boxes[active][layer].size() == 0){
                    if(active_boxes[0][layer].size() == 0){
                        total_weights[current_setting] -= working_weights[current_setting][layer][active];
                        working_weights[current_setting][layer][active] = 0;
                    }
                  }
                  else{
                    std::uniform_int_distribution<int> guessbox(0, active_boxes[active][layer].size() - 1);
                    if (temp_copy->guessbestsbox(active_boxes[active][layer][guessbox(generator)],try_one_box) == false) {
                      current_setting = num_settings + 1;
                      fail = true;
                    }
                  }
                  active = 2;
                  break;
              }
            }
        }

        if(current_setting < num_settings)
          if(total_weights[current_setting] == 0)
            current_setting = current_setting + 1;
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

