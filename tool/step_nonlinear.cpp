#include "step_nonlinear.h"

LinearDistributionTable::LinearDistributionTable(std::function<BitVector(BitVector)> fun, unsigned bitsize) {
  // TODO
}

std::ostream& operator<<(std::ostream& stream, const LinearDistributionTable& ldt) {
  // TODO
  return stream;
}

//-----------------------------------------------------------------------------

NonlinearStep::NonlinearStep(std::function<BitVector(BitVector)> fun, unsigned bitsize) : fun(fun), inout(Mask(bitsize), Mask(bitsize)), bitsize(bitsize) {
}

bool NonlinearStep::AddMasks(Mask& x, Mask& y) {
  // TODO merge with inout
  // then propagate
  return true;
}

bool NonlinearStep::ExtractMasks(Mask& x, Mask& y) {
  // TODO
  return true;
}

std::ostream& operator<<(std::ostream& stream, const NonlinearStep& step) {
  // TODO
  return stream;
}
