#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <memory>
#include <vector>
#include <string>
#include <cassert>

#include "../tinyxml2/tinyxml2.h"
#include "permutation.h"
#include "guessmask.h"
#include "permutation_list.h"


struct Configparser {
  Configparser();
  bool parseFile(std::string filename);
  std::unique_ptr<Permutation> getPermutation();
  Settings getSettings();
  unsigned int getCredits();
  bool printActive();
  bool Error(const char *format, ...);
  bool Warning(const char *format, ...);

  std::unique_ptr<Permutation> perm_;
  Settings settings_;
  unsigned int credits_;
  bool print_active_;
};

#endif /* CONFIGPARSER_H_ */
