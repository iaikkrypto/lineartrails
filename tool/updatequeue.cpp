#include "updatequeue.h"

UpdatePos::UpdatePos(uint8_t state, uint8_t word, uint8_t bit, uint8_t priority) : state(state), word(word), bit(bit), priority(priority) {
}

bool UpdatePos::operator<(const UpdatePos &rhs) const {
  return priority < rhs.priority;
}

// ----------------------------------------------------------------------------

void UpdateQueue::add_item(UpdatePos pos) {
  queue.push(pos);
}

UpdatePos UpdateQueue::next() {
  UpdatePos pos = queue.top();
  queue.pop();
  return pos;
}

bool UpdateQueue::empty(){
  return queue.empty();
}
