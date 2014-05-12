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
#include "search.h"


Search::Search(Permutation &perm)
    : perm_(&perm) {

}


void Search::StackSearch1(Commandlineparser& cl_param,
                          Configparser& config_param) {

  std::unique_ptr<Permutation> working_copy;
  std::stack<std::unique_ptr<Permutation>> char_stack;

  double best_prob = -DBL_MAX;
  GuessMask guesses;
  SboxPos guessed_box(0, 0);
  SboxPos backtrack_box(0, 0);
  bool backtrack;
  bool active;

  working_copy = config_param.getPermutation();
  if (working_copy->checkchar() == false) {
    std::cout << "Initial checkchar failed" << std::endl;
    return;
  }
  Settings settings = config_param.getSettings();

  auto start_count = std::chrono::system_clock::now();
  std::mt19937 generator(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_real_distribution<float> push_stack_rand(0.0, 1.0);

  unsigned int interations = (unsigned int) cl_param.getIntParameter("-iter");
  int total_iterations = 0;
  int print_char = cl_param.getIntParameter("-S");
  for (unsigned int i = 0; i < interations; ++i) {
    char_stack.emplace(working_copy->clone());
    char_stack.emplace(working_copy->clone());
    backtrack = false;
    guesses.createMask(char_stack.top().get(), settings);
    unsigned int curr_credit = config_param.getCredits();
    while (guesses.getRandPos(guessed_box, active)) {
      total_iterations++;
      auto duration = std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now() - start_count);
      if (cl_param.getIntParameter("-I") > 0
          && duration.count() > cl_param.getIntParameter("-I")) {
        std::cout << "PRINT-INFO: total iterations: " << total_iterations
                  << ", stack size: " << char_stack.size() << ", credits: "
                  << curr_credit << ", restarts: " << i << std::endl;
        print_char--;
        if (print_char == 0) {
          char_stack.top()->print(std::cout);
          print_char = cl_param.getIntParameter("-S");
        }
        start_count = std::chrono::system_clock::now();
      }

      if (curr_credit == 0)
        break;

      if (backtrack)
        guessed_box = backtrack_box;

      unsigned int wbias = guesses.getSboxWeigthProb();
      unsigned int whamming = guesses.getSboxWeightHamming();
      //FIXME: get rid of the 10
      auto rating = [wbias, whamming] (int bias, int hw_in, int hw_out) {
        return wbias*std::abs(bias) +whamming*((10-hw_in)+(10-hw_out));
      };
      if (char_stack.top()->guessbestsboxrandom(
          guessed_box, rating, guesses.getAlternativeSboxGuesses())) {
//          std::cout << "worked " << char_stack.size() << std::endl;
//          char_stack.top()->print(std::cout);
        backtrack = false;
        if (push_stack_rand(generator) <= guesses.getPushStackProb())
          char_stack.emplace(char_stack.top()->clone());
      } else {
//          std::cout << "failed" << std::endl;
//          char_stack.top()->print(std::cout);
        char_stack.pop();
        curr_credit--;
        backtrack = true;
        backtrack_box = guessed_box;
        if (char_stack.size() == 1)
          char_stack.emplace(working_copy->clone());
      }
      guesses.createMask(char_stack.top().get(), settings);
    }
    double current_prob;
    if (config_param.printActive())
      current_prob = -char_stack.top()->GetActiveSboxes();
    else
      current_prob = char_stack.top()->GetProbability();
    if (current_prob > best_prob && curr_credit > 0) {
      best_prob = current_prob;
      std::cout << "iteration: " << i << std::endl;
      char_stack.top()->PrintWithProbability();
    }
    while (char_stack.size())
      char_stack.pop();
  }
}

void Search::StackSearchKeccak(Commandlineparser& cl_param,
                          Configparser& config_param) {

  std::unique_ptr<Permutation> working_copy;
  std::stack<std::unique_ptr<Permutation>> char_stack;

  double best_prob = -DBL_MAX;
  GuessMask guesses;
  SboxPos guessed_box(0, 0);
  SboxPos backtrack_box(0, 0);
  bool backtrack;
  bool active;

  working_copy = config_param.getPermutation();
  if (working_copy->checkchar() == false) {
    std::cout << "Initial checkchar failed" << std::endl;
    return;
  }
  Settings settings = config_param.getSettings();

  auto start_count = std::chrono::system_clock::now();
  std::mt19937 generator(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_real_distribution<float> push_stack_rand(0.0, 1.0);

  unsigned int interations = (unsigned int) cl_param.getIntParameter("-iter");
  int total_iterations = 0;
  int print_char = cl_param.getIntParameter("-S");
  for (unsigned int i = 0; i < interations; ++i) {
    char_stack.emplace(working_copy->clone());
    char_stack.emplace(working_copy->clone());
    backtrack = false;
    guesses.createMask(char_stack.top().get(), settings);
    unsigned int curr_credit = config_param.getCredits();
    while (guesses.getRandPos(guessed_box, active)) {
      total_iterations++;
      auto duration = std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now() - start_count);
      if (cl_param.getIntParameter("-I") > 0
          && duration.count() > cl_param.getIntParameter("-I")) {
        std::cout << "PRINT-INFO: total iterations: " << total_iterations
                  << ", stack size: " << char_stack.size() << ", credits: "
                  << curr_credit << ", restarts: " << i << std::endl;
        print_char--;
        if (print_char == 0) {
          char_stack.top()->print(std::cout);
          print_char = cl_param.getIntParameter("-S");
        }
        start_count = std::chrono::system_clock::now();
      }

      if (curr_credit == 0)
        break;

      if (backtrack)
        guessed_box = backtrack_box;

      unsigned int wbias = guesses.getSboxWeigthProb();
      unsigned int whamming = guesses.getSboxWeightHamming();
      //FIXME: get rid of the 10
      auto rating = [wbias, whamming] (int bias, int hw_in, int hw_out) {
        return wbias * std::abs(bias) + whamming * ((10 - hw_in) + (10 - hw_out));
      };
      if (char_stack.top()->guessbestsboxrandom(guessed_box, rating, guesses.getAlternativeSboxGuesses())) {
//          std::cout << "worked " << char_stack.size() << std::endl;
//          char_stack.top()->print(std::cout);
        backtrack = false;
        if (push_stack_rand(generator) <= guesses.getPushStackProb())
          char_stack.emplace(char_stack.top()->clone());
      } else {
//          std::cout << "failed" << std::endl;
//          char_stack.top()->print(std::cout);
        char_stack.pop();
        curr_credit--;
        backtrack = true;
        backtrack_box = guessed_box;
        if (char_stack.size() == 1)
          char_stack.emplace(working_copy->clone());
      }
      guesses.createMask(char_stack.top().get(), settings);
    }
    double current_prob;
    current_prob = KeccakProb(char_stack);
    if (current_prob > best_prob && curr_credit > 0) {
      best_prob = current_prob;
      std::cout << "iteration: " << i << std::endl;
      std::cout << "bias without last round: " << best_prob << std::endl;
      char_stack.top()->PrintWithProbability();
    }
    while (char_stack.size())
      char_stack.pop();
  }
}

double Search::KeccakProb(std::stack<std::unique_ptr<Permutation>>& char_stack) {
  double prob = 0.0;
  double temp_prob;

  for (unsigned i = 0; i < char_stack.top()->sbox_layers_.size() - 1; ++i) {
    temp_prob = char_stack.top()->sbox_layers_[i]->GetProbability();
    prob += temp_prob;
  }

  prob += char_stack.top()->sbox_layers_.size() - 2;

  return prob;
}

