/*
 * lrucache.h
 *
 *  Created on: Jun 18, 2014
 *      Author: cdobraunig
 */

#ifndef LRUCACHE_H_
#define LRUCACHE_H_

#include <map>
#include <functional>

#include "cache.h"

template <typename KEY_TYPE, typename TYPE>
class LRU_Cache : public Cache<KEY_TYPE, TYPE> {
 public:
  LRU_Cache(unsigned int max_cache_size);
  virtual bool find(const KEY_TYPE& key, TYPE& content);
  virtual bool insert(const KEY_TYPE& key, const TYPE& content);

 private:
  unsigned int max_cache_size_ = 0x1000;
  unsigned int cache_size_ ;
  unsigned long long time_;

  void deleteOldestElement();

  std::unordered_map<KEY_TYPE, std::pair<TYPE,unsigned long long>> cache_;

};

#include "lrucache.hpp"

#endif /* LRUCACHE_H_ */
