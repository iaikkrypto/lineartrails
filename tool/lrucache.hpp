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
template<typename KEY_TYPE, typename TYPE>
LRU_Cache<KEY_TYPE, TYPE>::LRU_Cache(unsigned int max_cache_size)
    : max_cache_size_(max_cache_size),
      cache_size_(0),
      time_(0) {
  cache_.reserve(max_cache_size_);
}

template<typename KEY_TYPE, typename TYPE>
bool LRU_Cache<KEY_TYPE, TYPE>::find(const KEY_TYPE& key, TYPE& content) {
  auto element = cache_.find(key);

  if (element == cache_.end())
    return false;

  content = element->second.first;
  element->second.second = ++time_;

  return true;
}

template<typename KEY_TYPE, typename TYPE>
bool LRU_Cache<KEY_TYPE, TYPE>::insert(const KEY_TYPE& key,
                                               const TYPE& content) {
  if (cache_size_ >= max_cache_size_)
    deleteOldestElement();

//  std::cout << "Insert: " << key << std::endl << "Size: " << cache_size_ << std::endl;

  cache_size_++;
  bool cache_worked = true;
  cache_worked &= cache_.emplace(
      key,
      std::pair<TYPE, unsigned long long>(content, ++time_)).second;
  assert(cache_worked == true);
  return cache_worked;
}

template<typename KEY_TYPE, typename TYPE>
void LRU_Cache<KEY_TYPE, TYPE>::deleteOldestElement() {
  auto lru_element = cache_.begin();
  for (auto element = cache_.begin(); element != cache_.end(); ++element)
    if (element->second.second < lru_element->second.second)
      lru_element = element;
  cache_.erase(lru_element);
  cache_size_--;
}
