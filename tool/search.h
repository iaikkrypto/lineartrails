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

#include "permutation.h"
#include "mask.h"

class Search {

 public:
  Search(Permutation &perm);
  void RandomSearch1(unsigned int iterations);
  void HeuristicSearch1(unsigned int iterations, std::vector<std::vector<std::array<int,2>>> weights);


  Permutation *perm_;

};

#endif /* SEARCH_H_ */
