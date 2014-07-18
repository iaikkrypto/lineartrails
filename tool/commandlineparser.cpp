/*
 * commandlineparser.cpp
 *
 *  Created on: Jul 18, 2014
 *      Author: cdobraunig
 */

#include "commandlineparser.h"

void Commandlineparser::addParameter(std::string parameter_switch,
                                     const char* parameter) {
  auto added_parameter = params_.emplace(parameter_switch, parameter);
  assert(added_parameter.second);
}
const char* Commandlineparser::getParameter(std::string parameter_switch) {
  auto it = params_.find(parameter_switch);
  assert(it != params_.end());
  return it->second;
}
int Commandlineparser::getIntParameter(std::string parameter_switch) {
  return atoi(getParameter(parameter_switch));
}
float Commandlineparser::getFloatParameter(std::string parameter_switch) {
  return atoi(getParameter(parameter_switch));
}

void Commandlineparser::parse(int& argc, const char* argv[]){
  for(int i = 1; i < argc; ++i){
    auto it = params_.find(std::string (argv[i]));
    assert(it != params_.end());
    ++i;
    if(i >= argc)
      assert(!"Switch there, but parameter missing.");
    it->second = argv[i];
  }
}
