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

#include "permutation.h"
#include "mask.h"

class Search {

 public:
  Search(Permutation &perm);
  void RandomSearch1(unsigned int iterations);


  Permutation *perm_;

};

#endif /* SEARCH_H_ */
