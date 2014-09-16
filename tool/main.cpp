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

  Search my_search(*(parser.getPermutation()));
  auto myfunction = [] (int bias, int hw_in, int hw_out) {
    return 2*std::abs(bias) +1*((5-hw_in)+(5-hw_out));
  };
  my_search.StackSearch1(args, parser, myfunction, false, 0.1);
}

void checkchar(Commandlineparser& args) {
  Configparser parser;

  parser.parseFile(args.getParameter("-i"));

  if(parser.getPermutation()->checkchar())
    std::cout << "checkchar worked" << std::endl;
  else
    std::cout << "checkchar failed" << std::endl;
}

// ==== Main / Search ====
int main(int argc, const char* argv[]) {

  Commandlineparser args;

  args.addParameter("-iter", "-1");
  args.addParameter("-sba", "3");
  args.addParameter("-S", "5");
  args.addParameter("-I", "2");

  args.addParameter("-i", "char/example.xml");
  args.addParameter("-u", "");

  args.parse(argc, argv);

  std::cout << "Iterations: " << args.getIntParameter("-iter") << std::endl;
  std::cout << "S-box guesses: " << args.getIntParameter("-sba") << std::endl;
  std::cout << "Configfile: " << args.getParameter("-i") << std::endl;

  if (std::strcmp(args.getParameter("-u"), "checkchar") == 0) {
    checkchar(args);
  } else {
    std::cout << "config search" << std::endl;
    config_search(args);
  }

  return 0;
}
