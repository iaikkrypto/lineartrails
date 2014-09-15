#include "permutation_list.h"
#include "ascon_permutation.h"
#include "hamsi_permutation.h"
#include "pride_permutation.h"


Permutation* permutation_list(std::string name, int rounds) {

  if (name.compare("ascon") == 0)
    return new AsconPermutation(rounds);
  if (name.compare("hamsi") == 0)
    return new HamsiPermutation(rounds);
  if (name.compare("hamsicompression") == 0)
    return new HamsiCompression(rounds);
  if (name.compare("pride") == 0)
    return new PridePermutation(rounds);

  assert(!"instance with this name not found");
  return nullptr;
}

