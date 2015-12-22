/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
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
  return getParameter(parameter_switch) != nullptr;
}

void Commandlineparser::parse(int& argc, const char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    auto it = params_.find(std::string (argv[i]));
    if (it == params_.end())
      print_fatal_error(std::string("Invalid parameter '") + argv[i] + "'!");
    if (it->second.defaultvalue != nullptr) {
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
    std::string defaulttext = (par.defaultvalue == nullptr) ? "" : std::string(" (default '") + par.defaultvalue + "')";
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
