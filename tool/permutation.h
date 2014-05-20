#ifndef PERMUTATION_H_
#define PERMUTATION_H_

#include <stdint.h>
#include <vector>

#include "layer.h"
#include "mask.h"
#include "updatequeue.h"
#include "memory"

struct Permutation {
  virtual int checkchar() = 0;
  virtual int start_guessing(int print_interval) = 0;
  virtual int update() = 0;

  std::vector<std::unique_ptr<Layer>> layers_;
  UpdateQueue queue_linear_;
  UpdateQueue queue_nonlinear_;
};

#endif // UPDATEQUEUE_H_
