#pragma once

#include <components/document/base.hpp>

template<typename FirstType, typename SecondType>
class json_object {
public:
  using allocator_type = std::pmr::memory_resource;

  explicit json_object(allocator_type *allocator) noexcept;

  ~json_object() = default;

  json_object(json_object &&) noexcept = default;

  json_object(const json_object &) = delete;

  json_object &operator=(json_object &&) noexcept = default;

  json_object &operator=(const json_object &) = delete;

  const json_trie_node<FirstType, SecondType> *get(std::string_view key) const;

  void set(std::string_view key, json_trie_node<FirstType, SecondType> *value);

  void set(std::string_view key, boost::intrusive_ptr<json_trie_node<FirstType, SecondType>> &&value);

  boost::intrusive_ptr<json_trie_node<FirstType, SecondType>> remove(std::string_view key);

  size_t size() const noexcept;

  json_object<FirstType, SecondType> *make_deep_copy() const;

  std::pmr::string to_json(
          std::pmr::string (*)(const FirstType *, std::pmr::memory_resource *),
          std::pmr::string (*)(const SecondType *, std::pmr::memory_resource *)
  ) const;

  bool equals(
          const json_object<FirstType, SecondType> &other,
          bool (*)(const FirstType *, const FirstType *),
          bool (*)(const SecondType *, const SecondType *),
          bool (*)(const FirstType *, const SecondType *)
  ) const;

  json_object<FirstType, SecondType> *make_copy_except_deleter(allocator_type *allocator) const;

  static json_object<FirstType, SecondType> *merge(
          json_object<FirstType, SecondType> &object1,
          json_object<FirstType, SecondType> &object2,
          allocator_type *allocator
  );

private:
  std::pmr::unordered_map<
          std::string,
          boost::intrusive_ptr<json_trie_node<FirstType, SecondType>>
  > map_;
};

template<typename FirstType, typename SecondType>
json_object<FirstType, SecondType>::json_object(json_object::allocator_type *allocator) noexcept
        : map_(allocator) {}

template<typename FirstType, typename SecondType>
const json_trie_node<FirstType, SecondType> *json_object<FirstType, SecondType>::get(std::string_view key) const {
  auto res = map_.find(std::string(key));
  if (res == map_.end()) {
    return nullptr;
  }
  return res->second.get();
}

template<typename FirstType, typename SecondType>
void json_object<FirstType, SecondType>::set(std::string_view key, json_trie_node<FirstType, SecondType> *value) {
  map_[std::string(key)] = value;
}

template<typename FirstType, typename SecondType>
void json_object<FirstType, SecondType>::set(
        std::string_view key,
        boost::intrusive_ptr<json_trie_node<FirstType, SecondType>> &&value
) {
  map_[std::string(key)] = std::forward<boost::intrusive_ptr<json_trie_node<FirstType, SecondType>>>(value);
}

template<typename FirstType, typename SecondType>
boost::intrusive_ptr<json_trie_node<FirstType, SecondType>>
json_object<FirstType, SecondType>::remove(std::string_view key) {
  auto found = map_.find(std::string(key));
  if (found == map_.end()) {
    return nullptr;
  }
  auto copy = found->second;
  map_.erase(found);
  return copy;
}

template<typename FirstType, typename SecondType>
size_t json_object<FirstType, SecondType>::size() const noexcept {
  return map_.size();
}

template<typename FirstType, typename SecondType>
json_object<FirstType, SecondType> *
json_object<FirstType, SecondType>::make_deep_copy() const {
  auto copy = new(map_.get_allocator().resource()->allocate(sizeof(json_object<FirstType, SecondType>)))
          json_object(map_.get_allocator().resource());
  for (auto &it : map_) {
    copy->map_.emplace(it.first, it.second->make_deep_copy());
  }
  return copy;
}

template<typename FirstType, typename SecondType>
std::pmr::string json_object<FirstType, SecondType>::to_json(
        std::pmr::string (*to_json_first)(const FirstType *, std::pmr::memory_resource *),
        std::pmr::string (*to_json_second)(const SecondType *, std::pmr::memory_resource *)
) const {
  std::pmr::string res(map_.get_allocator().resource());
  res.append("{");
  for (auto &it : map_) {
    auto key = it.first;
    if (res.size() > 1) {
      res.append(",");
    }
    res.append("\"").append(key).append("\"").append(":").append(it.second->to_json(to_json_first, to_json_second));
  }
  return res.append("}");
}

template<typename FirstType, typename SecondType>
bool
json_object<FirstType, SecondType>::equals(
        const json_object<FirstType, SecondType> &other,
        bool (* first_equals_first)(const FirstType *, const FirstType *),
        bool (* second_equals_second)(const SecondType *, const SecondType *),
        bool (* first_equals_second)(const FirstType *, const SecondType *)
) const {
  if (size() != other.size()) {
    return false;
  }
  for (auto &it: map_) {
    auto next_node2 = other.map_.find(it.first);
    if (
            next_node2 == other.map_.end() ||
            !it.second->equals(
                    next_node2->second.get(),
                    first_equals_first,
                    second_equals_second,
                    first_equals_second
            )
    ) {
      return false;
    }
  }
  return true;
}

template<typename FirstType, typename SecondType>
json_object<FirstType, SecondType> *
json_object<FirstType, SecondType>::make_copy_except_deleter(allocator_type *allocator) const {
  auto copy = new(allocator->allocate(sizeof(json_object))) json_object(allocator);
  for (auto &it : map_) {
    if (it.second->is_deleter()) {
      continue;
    }
    copy->map_.emplace(it);
  }
  return copy;
}

template<typename FirstType, typename SecondType>
json_object<FirstType, SecondType> *json_object<FirstType, SecondType>::merge(
        json_object<FirstType, SecondType> &object1,
        json_object<FirstType, SecondType> &object2,
        json_object::allocator_type *allocator
) {
  auto res = new(allocator->allocate(sizeof(json_object))) json_object(allocator);
  for (auto &it : object2.map_) {
    if (it.second->is_deleter()) {
      continue;
    }
    auto next = object1.map_.find(it.first);
    if (next == object1.map_.end()) {
      res->map_.emplace(it);
    } else {
      res->map_.emplace(it.first, json_trie_node<FirstType, SecondType>::merge(next->second.get(), it.second.get(), allocator));
    }
  }
  for (auto &it : object1.map_) {
    if (object2.map_.find(it.first) == object2.map_.end()) {
      res->map_.emplace(it);
    }
  }
  return res;
}