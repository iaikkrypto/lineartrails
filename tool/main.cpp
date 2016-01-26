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
#include <vector>
#include <chrono>
#include <cmath>
#include <array>

#include "mask.h"
#include "step_linear.h"
#include "step_nonlinear.h"
#include "ascon.h"
#include "ascon_permutation.h"
#include "search.h"
#include "configparser.h"
#include "cmdline.h"
#include "commandlineparser.h"


int arg_interval;
int arg_printStatus;
int arg_maxIterations;
std::string arg_inputFile;
enum enum_function arg_function;

void config_search(Commandlineparser& args) {
  Configparser parser;

  bool config_ok = parser.parseFile(args.getParameter("-f"));

  if(config_ok == false)
    exit(config_ok);

  Search my_search(*parser.getPermutation());
  my_search.StackSearch1(args, parser);
}

void config_search_keccak(Commandlineparser& args) {
  Configparser parser;

  bool config_ok = parser.parseFile(args.getParameter("-f"));

  if(config_ok == false)
      exit(config_ok);

  Search my_search(*parser.getPermutation());
  my_search.StackSearchKeccak(args, parser);
}

void checkchar(Commandlineparser& args) {
  Configparser parser;

  bool config_ok = parser.parseFile(args.getParameter("-f"));

  if(config_ok == false)
      exit(config_ok);

  if (parser.getPermutation()->checkchar())
    std::cout << "checkchar worked" << std::endl;
  else
    std::cout << "checkchar failed" << std::endl;
}

// ==== Main / Search ====
int main(int argc, char* argv[]) {
  Commandlineparser args("Tool to automatically search for linear characteristics");
  args.addParameter("-i", "max number of iterations or -1 for unlimited", "-1");
  args.addParameter("-S", "Multiple of -I where current characteristic is printed", "5");
  args.addParameter("-I", "Status update interval", "2");
  args.addParameter("-f", "characteristic input file", "examples/ascon_3_rounds_typeI.xml");
  args.addParameter("-u", "requested function: checkchar, search", "search");
  args.addParameter("-h", "display help", nullptr);
  args.parse(argc, (const char **) argv);

  gengetopt_args_info args_info;
  if (cmdline_parser(argc, argv, &args_info) != 0) {
    std::cerr << "failed parsing command line arguments" << std::endl;
    return EXIT_FAILURE;
  }
  arg_interval = args_info.interval_arg;
  arg_printStatus = args_info.printStatus_arg;
  arg_maxIterations = args_info.maxIterations_arg;
  arg_inputFile = std::string(args_info.inputFile_arg);
  arg_function = args_info.function_arg;
  cmdline_parser_free (&args_info);

  switch (arg_function) {
    case function_arg_checkchar:
      std::cout << "Checking characteristic ... " << std::endl;
      std::cout << "Configfile: " << arg_inputFile << std::endl;
      checkchar(args);
      break;
    case function_arg_keccak:
      std::cout << "Searching ... " << std::endl;
      std::cout << "Configfile: " << arg_inputFile << std::endl;
      std::cout << "Iterations: " << arg_maxIterations << std::endl;
      config_search_keccak(args);
      break;
    case function_arg_search:
      std::cout << "Searching ... " << std::endl;
      std::cout << "Configfile: " << arg_inputFile << std::endl;
      std::cout << "Iterations: " << arg_maxIterations << std::endl;
      config_search(args);
      break;
    case function__NULL:
    default:
      std::cerr << "Invalid choice for -u" << std::endl;
      break;
  }

  return 0;
}
