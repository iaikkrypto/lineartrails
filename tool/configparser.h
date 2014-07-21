#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <memory>
#include <vector>
#include <string>
#include <cassert>

#include "../tinyxml2/tinyxml2.h"
#include "permutation.h"
#include "guessmask.h"
#include "ascon_permutation.h" //TODO: make configparser independent from permutation


struct Configparser {
  bool parseFile(std::string filename);
  PermutationBase* getPermutation();
  GuessWeights getWeights();
  unsigned int getCredits();
  bool Error(const char *format, ...);
  bool Warning(const char *format, ...);

  std::unique_ptr<PermutationBase> perm_;
  GuessWeights weights_;
  unsigned int credits_;
};

#endif /* CONFIGPARSER_H_ */
