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
