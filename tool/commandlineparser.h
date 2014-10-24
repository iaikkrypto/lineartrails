

#ifndef COMMANDLINEPARSER_H_
#define COMMANDLINEPARSER_H_

#include <map>
#include <string>
#include <stdlib.h>
#include <cassert>

struct Parameter {
  std::string name;
  std::string helptext;
  const char* defaultvalue;
  const char* value;

  Parameter(std::string name, std::string helptext, const char *defaultvalue, const char* value);
};

struct Commandlineparser {
  Commandlineparser(std::string description);
  void addParameter(std::string parameter_switch, std::string helptext, const char* parameter);
  const char* getParameter(std::string parameter_switch);
  int getIntParameter(std::string parameter_switch);
  float getFloatParameter(std::string parameter_switch);
  bool getBoolParameter(std::string parameter_switch);
  void parse(int& argc,const char* argv[]);
  void print_pars();
  void print_fatal_error(std::string errmsg);
  void print_help();

  std::string description_;
  unsigned maxlen_;
  std::map<std::string,Parameter> params_;
};

#endif /* COMMANDLINEPARSER_H_ */
