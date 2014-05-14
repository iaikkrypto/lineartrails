#include <iostream>
#include <vector>

#include "mask.h"
#include "step_linear.h"
#include "step_nonlinear.h"

// ==== Target Functions ====
BitVector testfun_linear(BitVector in) {
  // y0 = x0 + x1
  // y1 = x1
  return ((in ^ (in >> 1)) & 1) | (in & 2);
}

BitVector testfun_nonlinear(BitVector in) {
  // y0 = x0 + ~x1 * x2
  // y1 = x1 + ~x2 * x0
  // y2 = x2 + ~x0 * x1
  return (in ^ (~((in >> 1) | (in << 2)) & ((in >> 2) | (in << 1)))) & 7;
}

void teststep_linear() {
  unsigned bitsize = 2;

  std::vector<std::pair<Mask, Mask>> testcases {
    {{BM_DUNNO, BM_DUNNO},   {BM_1,     BM_0    }}, // should be 11/10
    {{BM_0,     BM_1    },   {BM_DUNNO, BM_DUNNO}}, // should be 01/01
    {{BM_1,     BM_0    },   {BM_DUNNO, BM_DUNNO}}, // should be 10/11
    {{BM_1,     BM_DUNNO},   {BM_DUNNO, BM_0    }}, // should be 11/10
    {{BM_1,     BM_DUNNO},   {BM_0,     BM_1    }}, // should be contradiction
    {{BM_1,     BM_DUNNO},   {BM_DUNNO, BM_DUNNO}}, // should be 1?/1?
  };

  for (auto challenge : testcases) {
    LinSys sys(testfun_linear, bitsize);
    bool sat = sys.AddMasks(challenge.first, challenge.second);
    //Mask in(bitsize), out(bitsize);
    Mask in(challenge.first), out(challenge.second);
    sat &= sys.ExtractMasks(in, out);
    std::cout << challenge.first << "/" << challenge.second << " > ";
    if (sat)
      std::cout << in << "/" << out << std::endl;
    else {
      for (unsigned i = 0; i < bitsize; ++i)
        std::cout << "#";
      std::cout << "/";
      for (unsigned i = 0; i < bitsize; ++i)
        std::cout << "#";
      std::cout << std::endl;
    }
  }
}

void teststep_nonlinear() {
  unsigned bitsize = 3;

  NonlinearStep sys(testfun_nonlinear, bitsize);
    std::vector<std::pair<Mask, Mask>> testcases {
      {{BM_1, BM_DUNNO, BM_DUNNO},   {BM_1, BM_1, BM_0    }}, // should be 10?/110
      {{BM_0, BM_1, BM_1},   {BM_DUNNO, BM_1, BM_1    }}, // should be ###/###
      {{BM_0, BM_1, BM_1},   {BM_0, BM_0, BM_DUNNO    }}, // should be 011/001
      {{BM_DUNNO, BM_0, BM_1},   {BM_DUNNO, BM_DUNNO, BM_0    }}, // should be 101/1?0
    };
    for (auto challenge : testcases) {
      std::cout << challenge.first << "/" << challenge.second << " > ";
      sys.PropagateMasks(challenge.first, challenge.second);
      std::cout << challenge.first << "/" << challenge.second << std::endl;
    }
}

// ==== Main / Search ====
int main() {
  std::cout << "linear_test" << std::endl;
  teststep_linear();
  std::cout << "nonlinear_test" << std::endl;
  teststep_nonlinear();

  return 0;
}
