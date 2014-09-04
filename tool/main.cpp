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

// ==== Target Functions ====
std::array<BitVector, 1> testfun_linear(std::array<BitVector, 1> in) {
  // y0 = x0 + x1
  // y1 = x1
  return {((in[0] ^ (in[0] >> 1)) & 1) | (in[0] & 2)};
}

BitVector testfun_nonlinear(BitVector in) {
  // y0 = x0 + ~x1 * x2
  // y1 = x1 + ~x2 * x0
  // y2 = x2 + ~x0 * x1
  return (in ^ (~((in >> 1) | (in << 2)) & ((in >> 2) | (in << 1)))) & 7;
}

int box_rating(int bias, int hw_in, int hw_out) {
  return std::abs(bias);
}

//void teststep_linear() {
//  std::vector<std::pair<Mask, Mask>> testcases { { { BM_DUNNO, BM_DUNNO }, {
//      BM_1, BM_0 } },  // should be 11/10
//      { { BM_0, BM_1 }, { BM_DUNNO, BM_DUNNO } },  // should be 01/01
//      { { BM_1, BM_0 }, { BM_DUNNO, BM_DUNNO } },  // should be 10/11
//      { { BM_1, BM_DUNNO }, { BM_DUNNO, BM_0 } },  // should be 11/10
//      { { BM_1, BM_DUNNO }, { BM_0, BM_1 } },  // should be contradiction
//      { { BM_1, BM_DUNNO }, { BM_DUNNO, BM_DUNNO } },  // should be 1?/1?
//  };
//
//  for (auto challenge : testcases) {
//    LinearStep<2,1> sys(testfun_linear);
//    bool sat = sys.AddMasks(challenge.first, challenge.second);
//    //Mask in(bitsize), out(bitsize);
//    Mask in(challenge.first), out(challenge.second);
//    sat &= sys.ExtractMasks(in, out);
//    std::cout << challenge.first << "/" << challenge.second << " > ";
//    if (sat)
//      std::cout << in << "/" << out << std::endl;
//    else
//      std::cout << "##/##" << std::endl;
//  }
//}

#define ROTL16(x,n) (((x)<<(n))|((x)>>(16-(n))))
std::array<BitVector, 1> fun_hamsi_linear_mini(std::array<BitVector, 1> in) {
  BitVector a,b,c,d;
  a = in[0] >> 48;
  b = (in[0] >> 32) & 0xffff;
  c = (in[0] >> 16) & 0xffff;
  d = (in[0] >> 00) & 0xffff;
  a = ROTL16(a,13);
  c = ROTL16(c,3);
  b ^= a ^ c;
  d ^= c ^ (a<<3);
  b = ROTL16(b,1);
  d = ROTL16(d,7);
  a ^= b ^ d;
  c ^= d ^ (b <<7);
  a = ROTL16(a,5);
  c = ROTL16(c,6);
  a &= 0xffff;
  b &= 0xffff;
  c &= 0xffff;
  d &= 0xffff;
  return {((a<<48)|(b<<32)|(c<<16)|d)};
}


//void hamsi_linear_mini() {
//  Mask out_mask = {BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,
//      BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,
//      BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,
//      BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO
//  };
//  Mask in_mask = {BM_1,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,
//      BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_DUNNO,
//      BM_DUNNO,BM_DUNNO,BM_DUNNO,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,
//      BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0,BM_0
//  };
//
//    LinearStep<64,1> sys(fun_hamsi_linear_mini);
//    bool sat = sys.AddMasks(in_mask, out_mask);
//    //Mask in(bitsize), out(bitsize);
//    Mask in(in_mask), out(out_mask);
//    sat &= sys.ExtractMasks(in, out);
//    std::cout << in_mask << "/" << out_mask << " > ";
//    if (sat)
//      std::cout << in << "/" << out << std::endl;
//    else
//      std::cout << "##/##" << std::endl;
//
//}

void teststep_nonlinear() {
  NonlinearStep<3> sys(testfun_nonlinear);
  std::vector<std::pair<Mask, Mask>> testcases { { { BM_1, BM_DUNNO, BM_DUNNO },
      { BM_1, BM_1, BM_0 } },  // should be 10?/110
      { { BM_0, BM_1, BM_1 }, { BM_DUNNO, BM_1, BM_1 } },  // should be ###/###
      { { BM_0, BM_1, BM_1 }, { BM_0, BM_0, BM_DUNNO } },  // should be 011/001
      { { BM_DUNNO, BM_0, BM_1 }, { BM_DUNNO, BM_DUNNO, BM_0 } },  // should be 101/1?0
  };
  for (auto challenge : testcases) {
    std::cout << challenge.first << "/" << challenge.second << " > ";
    sys.Update(challenge.first, challenge.second);
    std::cout << challenge.first << "/" << challenge.second << std::endl;
  }
}

void test_statetest() {
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

void test_sboxlayer() {
  AsconState statein, stateout;

  statein.SetState(BM_0);
  statein[0].set_bit(BM_1, 2);
  statein[1].set_bit(BM_1, 2);

  stateout[0].set_bit(BM_1, 2);
  stateout[3].set_bit(BM_0, 2);  //result= sbox in 3 to out 1

  std::cout << "Input state:" << std::endl << statein << std::endl;
  std::cout << "output state:" << std::endl << stateout << std::endl
            << std::endl;

  AsconSboxLayer layer1(&statein, &stateout);

  for (int i = 0; i < 64; ++i)
    layer1.Update(UpdatePos(0, 0, i, 1));

  std::cout << "Input state:" << std::endl << statein << std::endl;
  std::cout << "output state:" << std::endl << stateout << std::endl
            << std::endl;
}

void test_linearlayer() {
  AsconState statein, stateout;

  statein.SetState(BM_0);
  statein[0].set_bit(BM_1, 0);
  statein[0].set_bit(BM_1, 19);
  statein[0].set_bit(BM_1, 28);

  std::cout << "Input state:" << std::endl << statein << std::endl;
  std::cout << "output state:" << std::endl << stateout << std::endl
            << std::endl;

  AsconLinearLayer layer1(&statein, &stateout);

  for (int i = 0; i < 5; ++i)
    layer1.Update(UpdatePos(0, i, 0, 1));

  std::cout << "Input state:" << std::endl << statein << std::endl;
  std::cout << "output state:" << std::endl << stateout << std::endl
            << std::endl;
}

void test_permutation() {
  AsconPermutation<1> perm;

  perm.state_masks_[0]->SetState(BM_0);
  perm.state_masks_[0]->SetBit(BM_1, 3, 0);
  perm.state_masks_[0]->SetBit(BM_1, 4, 0);
  perm.state_masks_[0]->SetBit(BM_1, 3, 7);
  perm.state_masks_[0]->SetBit(BM_1, 4, 7);
  perm.state_masks_[0]->SetBit(BM_1, 3, 41);
  perm.state_masks_[0]->SetBit(BM_1, 4, 41);

  perm.state_masks_[1]->SetBit(BM_1, 4, 0);
  perm.state_masks_[1]->SetBit(BM_0, 1, 0);
  perm.checkchar();
  perm.touchall();
  perm.state_masks_[1]->SetBit(BM_1, 4, 7);
  perm.state_masks_[1]->SetBit(BM_0, 1, 7);
  perm.checkchar();
  perm.touchall();
  perm.state_masks_[1]->SetBit(BM_1, 4, 41);
  perm.state_masks_[1]->SetBit(BM_0, 1, 41);

  perm.checkchar();
}

void test_active() {
  AsconPermutation<1> perm;

//  perm.state_masks_[0].SetState(BM_0);
  perm.state_masks_[0]->SetBit(BM_1, 0, 0);
//  perm.state_masks_[0].words[1].set_bit(BM_1, 0);
//  perm.state_masks_[0].words[0].set_bit(BM_1, 19);
//  perm.state_masks_[0].words[1].set_bit(BM_1, 19);
//  perm.state_masks_[0].words[0].set_bit(BM_1, 28);
//  perm.state_masks_[0].words[1].set_bit(BM_1, 28);
  perm.checkchar();

  std::vector<SboxPos> active;
  std::vector<SboxPos> inactive;

  perm.SboxStatus(active, inactive);

  std::cout << "active sboxes: ";
  for (auto pos : active)
    std::cout << (int) pos.pos_ << ", ";
  std::cout << std::endl;

  std::cout << "inactive sboxes: ";
  for (auto pos : inactive)
    std::cout << (int) pos.pos_ << ", ";
  std::cout << std::endl;

}

void test_active_guess(Commandlineparser& args) {
  AsconPermutation<3> perm;

//  perm.state_masks_[0].SetState(BM_0);
  perm.state_masks_[0]->SetBit(BM_1, 0, 0);
//  perm.state_masks_[0].words[1].set_bit(BM_1, 0);
//  perm.state_masks_[0].words[0].set_bit(BM_1, 19);
//  perm.state_masks_[0].words[1].set_bit(BM_1, 19);
//  perm.state_masks_[0].words[0].set_bit(BM_1, 28);
//  perm.state_masks_[0].words[1].set_bit(BM_1, 28);
  Search my_search(perm);

  my_search.RandomSearch1(args.getIntParameter("-iter"), box_rating);
}

void test_heuristic_guess(Commandlineparser& args) {

  //searchmasks for 2 rounds
//  AsconPermutation<2> perm;
//  perm.state_masks_[0].words[0].set_bit(BM_1, 0);
//  std::vector<std::vector<std::array<int,2>>> weights = {{{1,1},{1,1}}};
//  std::vector<std::vector<std::array<int,2>>> weights = {{{1,0},{1,0}},{{0,1},{0,1}}};
//  std::vector<std::vector<std::array<int,2>>> weights = {{{1,0},{0,0}},{{0,0},{1,0}},{{0,1},{0,1}}};

  //searchmasks for 3 rounds
//  AsconPermutation<3> perm;
//  perm.state_masks_[0].words[0].set_bit(BM_1, 0);
//  std::vector<std::vector<std::array<int,2>>> weights = {{{1,1},{1,1},{1,1}}};
//  std::vector<std::vector<std::array<int,2>>> weights = {{{1,0},{2,0},{3,0}},{{0,1},{0,1},{0,1}}};

  //searchmasks for 4 rounds
  AsconPermutation<4> perm;
  perm.state_masks_[5]->SetBit(BM_1, 2, 35);
  //first variant
//  perm.state_masks_[3]->SetState(BM_0);
//  perm.state_masks_[3]->SetBit(BM_1, 0, 35);
//  perm.state_masks_[3]->SetBit(BM_1, 0, 27);
//  perm.state_masks_[3]->SetBit(BM_1, 0, 9);
//  perm.state_masks_[4]->SetState(BM_0);
//  perm.state_masks_[4]->SetBit(BM_1, 0, 63);
//  perm.state_masks_[4]->SetBit(BM_1, 0, 54);
//  perm.state_masks_[4]->SetBit(BM_1, 0, 35);
  //second variant
//  perm.state_masks_[3]->SetState(BM_0);
//  perm.state_masks_[3]->SetBit(BM_1, 1, 0);
//  perm.state_masks_[3]->SetBit(BM_1, 1, 14);
//  perm.state_masks_[3]->SetBit(BM_1, 1, 58);
//  perm.state_masks_[4]->SetState(BM_0);
//  perm.state_masks_[4]->SetBit(BM_1, 1, 0);
//  perm.state_masks_[4]->SetBit(BM_1, 1, 39);
//  perm.state_masks_[4]->SetBit(BM_1, 1, 61);
  //third variant
//  perm.state_masks_[3]->SetState(BM_0);
//  perm.state_masks_[3]->SetBit(BM_1, 2, 0);
//  perm.state_masks_[3]->SetBit(BM_1, 2, 2);
//  perm.state_masks_[3]->SetBit(BM_1, 2, 12);
//  perm.state_masks_[4]->SetState(BM_0);
//  perm.state_masks_[4]->SetBit(BM_1, 2, 0);
//  perm.state_masks_[4]->SetBit(BM_1, 2, 1);
//  perm.state_masks_[4]->SetBit(BM_1, 2, 6);
  //fourth variant
//  perm.state_masks_[3]->SetState(BM_0);
//  perm.state_masks_[3]->SetBit(BM_1, 3, 0);
//  perm.state_masks_[3]->SetBit(BM_1, 3, 20);
//  perm.state_masks_[3]->SetBit(BM_1, 3, 34);
//  perm.state_masks_[4]->SetState(BM_0);
//  perm.state_masks_[4]->SetBit(BM_1, 3, 0);
//  perm.state_masks_[4]->SetBit(BM_1, 3, 10);
//  perm.state_masks_[4]->SetBit(BM_1, 3, 17);
  //fith variant
//  perm.state_masks_[3]->SetState(BM_0);
//  perm.state_masks_[3]->SetBit(BM_1, 4, 0);
//  perm.state_masks_[3]->SetBit(BM_1, 4, 14);
//  perm.state_masks_[3]->SetBit(BM_1, 4, 18);
//  perm.state_masks_[4]->SetState(BM_0);
//  perm.state_masks_[4]->SetBit(BM_1, 4, 0);
//  perm.state_masks_[4]->SetBit(BM_1, 4, 7);
//  perm.state_masks_[4]->SetBit(BM_1, 4, 41);
//  std::vector<std::vector<std::array<int,2>>> weights = {{{1,1},{1,1},{1,1},{1,1}}};
  GuessWeights weights = { { { 100, 0 }, { 500, 0 }, { 2000, 0 }, { 100, 0 } },
      { { 0, 1 }, { 0, 1 }, { 0, 1 }, { 0, 1 } } };

  Search my_search(perm);
  auto myfunction = [] (int bias, int hw_in, int hw_out) {
    return std::abs(bias) +1*((5-hw_in)+(5-hw_out));
  };
  my_search.HeuristicSearch3(args.getIntParameter("-iter"), weights, myfunction,
                             args.getIntParameter("-sba"), true);
}

void test_active_guess_layered() {
  AsconPermutation<2> perm;

//  perm.state_masks_[0].SetState(BM_0);
  perm.state_masks_[0]->SetBit(BM_1, 3, 0);
  perm.state_masks_[0]->SetBit(BM_1, 4, 0);
  perm.state_masks_[0]->SetBit(BM_1, 3, 7);
  perm.state_masks_[0]->SetBit(BM_1, 4, 7);
  perm.state_masks_[0]->SetBit(BM_1, 3, 41);
  perm.state_masks_[0]->SetBit(BM_1, 4, 41);

  perm.checkchar();

  std::vector<std::vector<SboxPos>> active;
  std::vector<std::vector<SboxPos>> inactive;

  perm.SboxStatus(active, inactive);
  std::default_random_engine generator;

  AsconPermutation<2> temp;
  temp = perm;
  for (int layer = 0; layer < 2; ++layer)
    while (active[layer].size() != 0 || inactive[layer].size() != 0) {
      while (inactive[layer].size() != 0) {
        std::uniform_int_distribution<int> guessbox(0,
                                                    inactive[layer].size() - 1);
        if (temp.guessbestsbox(inactive[layer][guessbox(generator)], box_rating)
            == false) {
          temp = perm;
          layer = -1;
          active[layer].clear();
          break;
        }
        temp.SboxStatus(active, inactive);
      }
      while (active[layer].size() != 0) {
        std::uniform_int_distribution<int> guessbox(0,
                                                    active[layer].size() - 1);
        if (temp.guessbestsbox(active[layer][guessbox(generator)], box_rating)
            == false) {
          temp = perm;
          layer = -1;
          temp.SboxStatus(active, inactive);
          break;
        }
        temp.SboxStatus(active, inactive);
      }
    }
  std::cout << "result" << std::endl;
  temp.PrintWithProbability();
}

void test_config(Commandlineparser& args) {
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

  parser.getPermutation()->checkchar();
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

//  std::cout << "linear_test" << std::endl;
//  teststep_linear();
//  std::cout << "nonlinear_test" << std::endl;
//  teststep_nonlinear();
//  std::cout << "sbox layer test" << std::endl;
//  test_sboxlayer();
//  std::cout << "linear layer test" << std::endl;
//  test_linearlayer();
//  std::cout << "permutation test" << std::endl;
//  test_permutation();

//  std::cout << "active test" << std::endl;
//  test_active();

//  std::cout << "active guess" << std::endl;
//  test_active_guess(iterations);

//    std::cout << "heuristic guess" << std::endl;
//    test_heuristic_guess(iterations, try_one_box);
//
  if (std::strcmp(args.getParameter("-u"), "checkchar") == 0) {
    checkchar(args);
  } else {
    std::cout << "test config" << std::endl;
    test_config(args);
  }

//
//  std::cout << "active guess layered" << std::endl;
//  test_active_guess_layered();

//  hamsi_linear_mini();

  return 0;
}
