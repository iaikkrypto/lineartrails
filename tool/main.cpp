#include <iostream>
#include <vector>

#include "mask.h"
#include "step_linear.h"
#include "step_nonlinear.h"
#include "ascon.h"

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
  std::vector<std::pair<Mask, Mask>> testcases {
    {{BM_DUNNO, BM_DUNNO},   {BM_1,     BM_0    }}, // should be 11/10
    {{BM_0,     BM_1    },   {BM_DUNNO, BM_DUNNO}}, // should be 01/01
    {{BM_1,     BM_0    },   {BM_DUNNO, BM_DUNNO}}, // should be 10/11
    {{BM_1,     BM_DUNNO},   {BM_DUNNO, BM_0    }}, // should be 11/10
    {{BM_1,     BM_DUNNO},   {BM_0,     BM_1    }}, // should be contradiction
    {{BM_1,     BM_DUNNO},   {BM_DUNNO, BM_DUNNO}}, // should be 1?/1?
  };

  for (auto challenge : testcases) {
    LinearStep<2> sys(testfun_linear);
    bool sat = sys.AddMasks(challenge.first, challenge.second);
    //Mask in(bitsize), out(bitsize);
    Mask in(challenge.first), out(challenge.second);
    sat &= sys.ExtractMasks(in, out);
    std::cout << challenge.first << "/" << challenge.second << " > ";
    if (sat)
      std::cout << in << "/" << out << std::endl;
    else
      std::cout << "##/##" << std::endl;
  }
}

void teststep_nonlinear() {
  NonlinearStep<3> sys(testfun_nonlinear);
    std::vector<std::pair<Mask, Mask>> testcases {
      {{BM_1, BM_DUNNO, BM_DUNNO},   {BM_1, BM_1, BM_0    }}, // should be 10?/110
      {{BM_0, BM_1, BM_1},   {BM_DUNNO, BM_1, BM_1    }}, // should be ###/###
      {{BM_0, BM_1, BM_1},   {BM_0, BM_0, BM_DUNNO    }}, // should be 011/001
      {{BM_DUNNO, BM_0, BM_1},   {BM_DUNNO, BM_DUNNO, BM_0    }}, // should be 101/1?0
    };
    for (auto challenge : testcases) {
      std::cout << challenge.first << "/" << challenge.second << " > ";
      sys.Update(challenge.first, challenge.second);
      std::cout << challenge.first << "/" << challenge.second << std::endl;
    }
}

void test_statetest(){
  AsconState state0;
  std::cout << state0 << std::endl << std::endl;
  std::cout << std::hex << "canbe1: " << state0[0].caremask.canbe1 << std::endl;
  std::cout << "care: " << state0[0].caremask.care << std::dec << std::endl;
  state0[0].set_bit(BM_1, 0);
  std::cout << std::hex << "canbe1: " << state0[0].caremask.canbe1 << std::endl;
  std::cout << "care: " << state0[0].caremask.care << std::dec << std::endl;
  state0[0].set_bit(BM_0, 1);
  std::cout << std::hex << "canbe1: " << state0[0].caremask.canbe1 << std::endl;
  std::cout << "care: " << state0[0].caremask.care << std::dec << std::endl;
  std::cout << state0 << std::endl << std::endl;
  state0.SetState(BM_1);
  std::cout << std::hex << "canbe1: " << state0[0].caremask.canbe1 << std::endl;
  std::cout << "care: " << state0[0].caremask.care << std::dec << std::endl;
  std::cout << state0 << std::endl << std::endl;
  state0.SetState(BM_0);
  std::cout << std::hex << "canbe1: " << state0[0].caremask.canbe1 << std::endl;
  std::cout << "care: " << state0[0].caremask.care << std::dec << std::endl;
  std::cout << state0 << std::endl << std::endl;

  std::cout << "Introducing second state" << std::endl;
  AsconState state1;
  std::cout << std::hex << "canbe1: " << state1[0].caremask.canbe1 << std::endl;
  std::cout << "care: " << state1[0].caremask.care << std::dec << std::endl;
  std::cout << state1 << std::endl << std::endl;

  state1 = state0;

  std::cout << std::hex << "canbe1: " << state1[0].caremask.canbe1 << std::endl;
  std::cout << "care: " << state1[0].caremask.care << std::dec << std::endl;
  std::cout << state1 << std::endl << std::endl;
}

void test_sboxlayer(){
  AsconState statein, stateout;

  statein.SetState(BM_0);
  statein[0].set_bit(BM_1,2);
  statein[1].set_bit(BM_1,2);

  stateout[0].set_bit(BM_1,2);
  stateout[3].set_bit(BM_0,2); //result= sbox in 3 to out 1

  std::cout << "Input state:" << std::endl << statein << std::endl;
  std::cout << "output state:" << std::endl << stateout << std::endl << std::endl;

  AsconSboxLayer layer1(&statein, &stateout);

  for(int i = 0; i < 64; ++i)
    layer1.Update(UpdatePos (0,0,i,1));

  std::cout << "Input state:" << std::endl << statein << std::endl;
  std::cout << "output state:" << std::endl << stateout << std::endl << std::endl;
}

void test_linearlayer(){
  AsconState statein, stateout;

  statein.SetState(BM_0);
  statein[0].set_bit(BM_1,0);
  statein[0].set_bit(BM_1,19);
  statein[0].set_bit(BM_1,28);

  std::cout << "Input state:" << std::endl << statein << std::endl;
  std::cout << "output state:" << std::endl << stateout << std::endl << std::endl;

  AsconLinearLayer layer1(&statein, &stateout);

  for(int i = 0; i < 5; ++i)
    layer1.Update(UpdatePos (0,i,0,1));

  std::cout << "Input state:" << std::endl << statein << std::endl;
  std::cout << "output state:" << std::endl << stateout << std::endl << std::endl;
}

void test_permutation(){
  AsconPermutation perm(1);

  perm.state_masks_[0].SetState(BM_0);
  perm.state_masks_[0].words[0].set_bit(BM_1, 0);
  perm.state_masks_[0].words[1].set_bit(BM_1, 0);
  perm.state_masks_[0].words[0].set_bit(BM_1, 19);
  perm.state_masks_[0].words[1].set_bit(BM_1, 19);
  perm.state_masks_[0].words[0].set_bit(BM_1, 28);
  perm.state_masks_[0].words[1].set_bit(BM_1, 28);

  perm.state_masks_[1].words[0].set_bit(BM_1, 0);
  perm.state_masks_[1].words[3].set_bit(BM_0, 0);
  perm.state_masks_[1].words[0].set_bit(BM_1, 19);
  perm.state_masks_[1].words[3].set_bit(BM_0, 19);
  perm.state_masks_[1].words[0].set_bit(BM_1, 28);
  perm.state_masks_[1].words[3].set_bit(BM_0, 28);

  perm.checkchar();
}

void test_guess(){
  AsconPermutation perm(1);

  perm.state_masks_[0].SetState(BM_0);
  perm.state_masks_[0].words[0].set_bit(BM_1, 0);
  perm.state_masks_[0].words[1].set_bit(BM_1, 0);
  perm.state_masks_[0].words[0].set_bit(BM_1, 19);
  perm.state_masks_[0].words[1].set_bit(BM_1, 19);
  perm.state_masks_[0].words[0].set_bit(BM_1, 28);
  perm.state_masks_[0].words[1].set_bit(BM_1, 28);
  perm.checkchar();
  AsconPermutation temp(1);
  temp = perm;
  while(temp.anythingtoguess() == true){
    if(temp.randomsboxguess() == false)
      temp = perm;
//    std::cout << temp << std::endl;
  }
  std::cout << "result" << std::endl << temp << std::endl;
}

// ==== Main / Search ====
int main() {
  std::cout << "linear_test" << std::endl;
  teststep_linear();
  std::cout << "nonlinear_test" << std::endl;
  teststep_nonlinear();
//  std::cout << "sbox layer test" << std::endl;
//  test_sboxlayer();
//  std::cout << "linear layer test" << std::endl;
//  test_linearlayer();
  std::cout << "permutation test" << std::endl;
  test_permutation();
  std::cout << "guess test" << std::endl;
  test_guess();

  return 0;
}
