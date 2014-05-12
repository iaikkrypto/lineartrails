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
#ifndef SEARCH_H_
#define SEARCH_H_

#include <memory>
#include <chrono>
#include <random>
#include <cfloat>
#include <vector>
#include <assert.h>
#include <stack>

#include "permutation.h"
#include "mask.h"
#include "guessmask.h"
#include "configparser.h"
#include "commandlineparser.h"

class Search {

 public:
  Search(Permutation &perm);
  void StackSearch1(Commandlineparser& cl_param, Configparser& config_param);
  void StackSearchKeccak(Commandlineparser& cl_param, Configparser& config_param);

 private:
  double KeccakProb(std::stack<std::unique_ptr<Permutation>>& char_stack);

  Permutation *perm_;

};

#endif /* SEARCH_H_ */
