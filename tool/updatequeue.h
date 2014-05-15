#ifndef UPDATEQUEUE_H_
#define UPDATEQUEUE_H_

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

  std::priority_queue<UpdatePos> queue;
};

#endif // UPDATEQUEUE_H_
