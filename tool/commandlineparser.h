

#ifndef COMMANDLINEPARSER_H_
#define COMMANDLINEPARSER_H_

#include <map>
#include <string>
#include <stdlib.h>
#include <cassert>

struct Commandlineparser {

  void addParameter(std::string parameter_switch, const char* parameter);
  const char* getParameter(std::string parameter_switch);
  int getIntParameter(std::string parameter_switch);
  float getFloatParameter(std::string parameter_switch);
  void parse(int& argc,const char* argv[]);

  std::map<std::string,const char*> params_;
};

#endif /* COMMANDLINEPARSER_H_ */
