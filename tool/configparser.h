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
#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <memory>
#include <vector>
#include <string>
#include <cassert>
#include <initializer_list>

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
  bool Error(std::initializer_list<std::string> msg);
  bool Warning(std::initializer_list<std::string> msg);

  std::unique_ptr<Permutation> perm_;
  Settings settings_;
  unsigned int credits_;
  bool print_active_;
};

#endif /* CONFIGPARSER_H_ */
