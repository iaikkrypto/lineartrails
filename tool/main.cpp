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
#include "commandlineparser.h"


void config_search(Commandlineparser& args) {
  Configparser parser;

  parser.parseFile(args.getParameter("-i"));

  Search my_search(*parser.getPermutation());
  my_search.StackSearch1(args, parser);
}

void checkchar(Commandlineparser& args) {
  Configparser parser;

  parser.parseFile(args.getParameter("-i"));

  if (parser.getPermutation()->checkchar())
    std::cout << "checkchar worked" << std::endl;
  else
    std::cout << "checkchar failed" << std::endl;
}

// ==== Main / Search ====
int main(int argc, const char* argv[]) {

  Commandlineparser args("Tool to automatically search for linear characteristics");

  args.addParameter("-iter", "max number of iterations or -1 for unlimited", "-1");
  args.addParameter("-S",    "???", "5");
  args.addParameter("-I",    "???", "2");

  args.addParameter("-i",    "characteristic input file", "char/example.xml");
  args.addParameter("-u",    "requested function: checkchar, search", "search");

  args.addParameter("-h",    "display help", 0);

  args.parse(argc, argv);

  if (args.getBoolParameter("-h")) {
    args.print_help();
  } else if (std::strcmp(args.getParameter("-u"), "checkchar") == 0) {
    std::cout << "Checking characteristic ... " << std::endl;
    std::cout << "Configfile: " << args.getParameter("-i") << std::endl;
    checkchar(args);
  } else {
    std::cout << "Searching ... " << std::endl;
    std::cout << "Configfile: " << args.getParameter("-i") << std::endl;
    std::cout << "Iterations: " << args.getIntParameter("-iter") << std::endl;
    config_search(args);
  }

  return 0;
}
