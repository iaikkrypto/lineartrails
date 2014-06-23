/*
 * cache.h
 *
 *  Created on: Jun 18, 2014
 *      Author: cdobraunig
 */

#ifndef CACHE_H_
#define CACHE_H_

template <typename KEY_TYPE, typename TYPE>
class Cache {
 public:
 virtual bool find(const KEY_TYPE& key, TYPE& content) = 0;
 virtual bool insert(const KEY_TYPE& key, const TYPE& content) = 0;
};

#include "cache.hpp"

#endif /* CACHE_H_ */
