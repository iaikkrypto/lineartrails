/*
 * commandlineparser.cpp
 *
 *  Created on: Jul 18, 2014
 *      Author: cdobraunig
 */

#include <iostream>
#include "commandlineparser.h"


Parameter::Parameter(std::string name, std::string helptext, const char *defaultvalue, const char* value) 
    : name(name)
    , helptext(helptext)
    , defaultvalue(defaultvalue)
    , value(value) {
}


Commandlineparser::Commandlineparser(std::string description) 
    : description_(description)
    , maxlen_(1) {
}

void Commandlineparser::addParameter(std::string parameter_switch,
                                     std::string helptext,
                                     const char* parameter) {
  auto added_parameter = params_.emplace(parameter_switch, Parameter(parameter_switch, helptext, parameter, parameter));
  maxlen_ = std::max(maxlen_, (unsigned)parameter_switch.length());
  assert(added_parameter.second);
}

const char* Commandlineparser::getParameter(std::string parameter_switch) {
  auto it = params_.find(parameter_switch);
  assert(it != params_.end());
  return it->second.value;
}

int Commandlineparser::getIntParameter(std::string parameter_switch) {
  return atoi(getParameter(parameter_switch));
}

float Commandlineparser::getFloatParameter(std::string parameter_switch) {
  return atof(getParameter(parameter_switch));
}

bool Commandlineparser::getBoolParameter(std::string parameter_switch) {
  return getParameter(parameter_switch) != 0;
}

void Commandlineparser::parse(int& argc, const char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    auto it = params_.find(std::string (argv[i]));
    if (it == params_.end())
      print_fatal_error(std::string("Invalid parameter '") + argv[i] + "'!");
    if (it->second.defaultvalue != 0) {
      ++i;
      if(i >= argc)
        print_fatal_error(std::string("Parameter for switch '") + argv[i-1] + "' is missing!");
      it->second.value = argv[i];
    } else {
      it->second.value = "true";
    }
  }
}

void Commandlineparser::print_pars() {
  for (auto& entry : params_) {
    const Parameter& par = entry.second;
    std::string pad = "  ";
    for (unsigned l = maxlen_; l >= par.name.length(); l--)
      pad += " ";
    std::string defaulttext = (par.defaultvalue == 0) ? "" : std::string(" (default '") + par.defaultvalue + "')";
    std::cout << par.name << pad << par.helptext << defaulttext << std::endl;
  }
}

void Commandlineparser::print_fatal_error(std::string errmsg) {
  std::cout << "ERROR: " << errmsg << std::endl;
  std::cout << "Usage:" << std::endl;
  print_pars();
  exit(-1);
}

void Commandlineparser::print_help() {
  std::cout << description_ << std::endl;
  std::cout << "Usage:" << std::endl;
  print_pars();
}
