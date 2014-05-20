#ifndef UPDATEQUEUE_H_
#define UPDATEQUEUE_H_

#include <stdint.h>
#include <queue>

struct UpdatePos {
  UpdatePos(uint8_t state, uint8_t word, uint8_t bit, uint8_t priority);

  bool operator<(const UpdatePos &rhs) const;

  uint8_t state;
  uint8_t word;
  uint8_t bit;
  uint8_t priority;
};

struct UpdateQueue {

  void add_item(UpdatePos pos);
  UpdatePos next();
  bool empty();

  std::priority_queue<UpdatePos> queue;
};

#endif // UPDATEQUEUE_H_
