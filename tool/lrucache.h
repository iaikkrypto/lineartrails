/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
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
