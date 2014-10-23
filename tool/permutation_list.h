#ifndef PERMUTATION_LIST_H_
#define PERMUTATION_LIST_H_

#include <string>
#include <cassert>

#include "permutation.h"
#include "ascon_permutation.h"
#include "hamsi_permutation.h"
#include "pride_permutation.h"
#include "keccak1600_permutation.h"


extern Permutation* permutation_list(std::string name, int rounds);
#endif /* PERMUTATION_LIST_H_ */
