#ifndef PERMUTATION_H_
#define PERMUTATION_H_

#include <stdint.h>
#include <vector>

#include "layer.h"
#include "mask.h"
#include "updatequeue.h"
#include "memory"

struct Permutation {
  virtual bool checkchar() = 0;
  virtual bool update() = 0;

  UpdateQueue queue_linear_;
  UpdateQueue queue_nonlinear_;
};

#endif // UPDATEQUEUE_H_
