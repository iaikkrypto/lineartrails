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
