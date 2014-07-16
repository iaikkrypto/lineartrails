#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <memory>
#include <vector>
#include <string>

#include "../tinyxml2/tinyxml2.h"
#include "permutation.h"
#include "guessmask.h"
#include "ascon_permutation.h" //TODO: make configparser independent from permutation


struct Configparser {
  void parseFile(std::string filename);
  PermutationBase* getPermutation();
  GuessWeights getWeights();

  std::unique_ptr<PermutationBase> perm_;
  GuessWeights weights;
//  tinyxml2::XMLDocument doc;
};

#endif /* CONFIGPARSER_H_ */
