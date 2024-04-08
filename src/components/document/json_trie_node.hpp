#pragma once

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <absl/container/flat_hash_map.h>
#include <memory_resource>
#include <allocator_intrusive_ref_counter.hpp>
#include <mr_utils.hpp>

struct string_view_hash {
  using is_transparent = void;

  size_t operator()(const std::pmr::string& s) const {
    return absl::Hash<std::pmr::string>{}(s);
  }

  size_t operator()(std::string_view sv) const {
    return absl::Hash<std::string_view>{}(sv);
  }
};

struct string_view_eq {
  using is_transparent = void;

  bool operator()(const std::pmr::string &lhs, const std::pmr::string &rhs) const {
    return lhs == rhs;
  }

  bool operator()(const std::pmr::string &lhs, std::string_view rhs) const {
    return lhs == rhs;
  }

  bool operator()(std::string_view lhs, const std::pmr::string &rhs) const {
    return lhs == rhs;
  }
};

template<typename FirstType, typename SecondType>
class json_trie_node : public allocator_intrusive_ref_counter {
public:
  using allocator_type = std::pmr::memory_resource;

  explicit json_trie_node(allocator_type *allocator);

  ~json_trie_node() override;

  json_trie_node(json_trie_node &&) noexcept;

  json_trie_node(const json_trie_node &);

  json_trie_node &operator=(json_trie_node &&) noexcept = delete;

  json_trie_node &operator=(const json_trie_node &) = delete;

  const json_trie_node<FirstType, SecondType> *find(std::string_view key) const;

  const FirstType *get_value_first() const;

  const SecondType *get_value_second() const;

  void insert(std::string_view key, const FirstType &value);

  void insert(std::string_view key, const SecondType &value);

  void insert(std::string_view key, boost::intrusive_ptr<json_trie_node> &&value);

  json_trie_node<FirstType, SecondType> *insert_array(std::string_view key);

  json_trie_node<FirstType, SecondType> *insert_object(std::string_view key);

  void insert_deleter(std::string_view key);

  boost::intrusive_ptr<json_trie_node<FirstType, SecondType>> erase(std::string_view key);

  boost::intrusive_ptr<json_trie_node<FirstType, SecondType>> make_deep_copy(std::string_view key);

  size_t size() const;

  bool is_object() const;

  bool is_array() const;

  bool is_terminal() const;

  bool is_deleter() const;

  std::pmr::string to_json(
          std::pmr::string (*)(FirstType *, std::pmr::memory_resource *),
          std::pmr::string (*)(SecondType *, std::pmr::memory_resource *)
  ) const;

  static json_trie_node<FirstType, SecondType> *merge(
          json_trie_node<FirstType, SecondType> *node1,
          json_trie_node<FirstType, SecondType> *node2,
          allocator_type &allocator
  );

  static bool equals(
          json_trie_node<FirstType, SecondType> *node1,
          json_trie_node<FirstType, SecondType> *node2,
          bool (*)(FirstType *, FirstType *),
          bool (*)(SecondType *, SecondType *),
          bool (*)(FirstType *, SecondType *)
  );

private:
  enum json_type {
    OBJECT,
    ARRAY,
    TERMINAL,
    DELETER,
  };

  union value_type {
    FirstType* first;
    SecondType* second;
  };

  explicit json_trie_node(allocator_type *allocator, value_type value, bool is_first, json_type type);

  allocator_type *allocator_;
  absl::flat_hash_map<
          std::pmr::string,
          boost::intrusive_ptr<json_trie_node>,
          string_view_hash, string_view_eq,
          std::pmr::polymorphic_allocator<std::pair<const std::pmr::string, boost::intrusive_ptr<json_trie_node>>>
  > children_;
  value_type value_;
  bool is_first_;
  json_type type_;

  std::pmr::string to_json_object(
          std::pmr::string (*)(FirstType *, std::pmr::memory_resource *),
          std::pmr::string (*)(SecondType *, std::pmr::memory_resource *)
  ) const;

  std::pmr::string to_json_array(
          std::pmr::string (*)(FirstType *, std::pmr::memory_resource *),
          std::pmr::string (*)(SecondType *, std::pmr::memory_resource *)
  ) const;

  boost::intrusive_ptr<json_trie_node<FirstType, SecondType>> make_deep_copy_(json_trie_node<FirstType, SecondType> *node);
};

template<typename FirstType, typename SecondType>
json_trie_node<FirstType, SecondType>::json_trie_node(json_trie_node::allocator_type *allocator)
        : json_trie_node(allocator, {.second = nullptr}, false, OBJECT) {}

template<typename FirstType, typename SecondType>
json_trie_node<FirstType, SecondType>::json_trie_node(
        allocator_type *allocator,
        value_type value,
        bool is_first,
        json_type type
)
        : allocator_intrusive_ref_counter(allocator),
          allocator_(allocator),
          children_(allocator_),
          value_(value),
          is_first_(is_first),
          type_(type) {}

template<typename FirstType, typename SecondType>
json_trie_node<FirstType, SecondType>::~json_trie_node() {
  if (is_first_) {
    mr_delete(allocator_, value_.first);
  } else {
    mr_delete(allocator_, value_.second);
  }
}

template<typename FirstType, typename SecondType>
json_trie_node<FirstType, SecondType>::json_trie_node(json_trie_node &&other) noexcept
        : allocator_(other.allocator_),
          children_(std::move(other.children_)),
          value_(std::move(other.value_)),
          is_first_(other.is_first_),
          type_(other.type_) {
  other.allocator_ = nullptr;
  if (other.is_first_) {
    value_.first = nullptr;
  } else {
    value_.second = nullptr;
  }
}

template<typename FirstType, typename SecondType>
json_trie_node<FirstType, SecondType>::json_trie_node(const json_trie_node &other)
        : allocator_intrusive_ref_counter(other.allocator_),
          allocator_(other.allocator_),
          children_(other.children_.size()),
          is_first_(other.is_first_),
          type_(other.type_) {
  if (is_first_) {
    value_.first = is_terminal() ? new(allocator_->allocate(sizeof(FirstType))) FirstType(*other.value_.first) : nullptr;
  } else {
    value_.second = is_terminal() ? new(allocator_->allocate(sizeof(SecondType))) SecondType(*other.value_.second) : nullptr;
  }
}

template<typename FirstType, typename SecondType>
const json_trie_node<FirstType, SecondType> *json_trie_node<FirstType, SecondType>::find(std::string_view key) const {
  auto res = children_.find(key);
  if (res == children_.end()) {
    return nullptr;
  }
  return res->second.get();
}

template<typename FirstType, typename SecondType>
const FirstType *json_trie_node<FirstType, SecondType>::get_value_first() const {
  if (!is_first_) {
    return nullptr;
  }
  return value_.first;
}

template<typename FirstType, typename SecondType>
const SecondType *json_trie_node<FirstType, SecondType>::get_value_second() const {
  if (is_first_) {
    return nullptr;
  }
  return value_.second;
}

template<typename FirstType, typename SecondType>
void json_trie_node<FirstType, SecondType>::insert(std::string_view key, const FirstType &value) {
  auto node_value = new(allocator_->allocate(sizeof(FirstType))) FirstType(value);
  auto is_first = true;
  children_[key] = new(allocator_->allocate(sizeof(json_trie_node)))
          json_trie_node(allocator_, {.first = node_value}, is_first, TERMINAL);
}

template<typename FirstType, typename SecondType>
void json_trie_node<FirstType, SecondType>::insert(std::string_view key, const SecondType &value) {
  auto node_value = new(allocator_->allocate(sizeof(SecondType))) SecondType(value);
  auto is_first = false;
  children_[key] = new(allocator_->allocate(sizeof(json_trie_node)))
          json_trie_node(allocator_, {.second = node_value}, is_first, TERMINAL);
}

template<typename FirstType, typename SecondType>
json_trie_node<FirstType, SecondType> *json_trie_node<FirstType, SecondType>::insert_array(std::string_view key) {
  auto node_value = nullptr;
  auto is_first = false;
  return (children_[key] = new(allocator_->allocate(sizeof(json_trie_node)))
          json_trie_node(allocator_, {.second = node_value}, is_first, ARRAY)).get();
}

template<typename FirstType, typename SecondType>
json_trie_node<FirstType, SecondType> *json_trie_node<FirstType, SecondType>::insert_object(std::string_view key) {
  auto node_value = nullptr;
  auto is_first = false;
  return (children_[key] = new(allocator_->allocate(sizeof(json_trie_node)))
          json_trie_node(allocator_, {.second = node_value}, is_first, OBJECT)).get();
}

template<typename FirstType, typename SecondType>
void json_trie_node<FirstType, SecondType>::insert_deleter(std::string_view key) {
  auto node_value = nullptr;
  auto is_first = false;
  children_[key] = new(allocator_->allocate(sizeof(json_trie_node)))
          json_trie_node(allocator_, {.second = node_value}, is_first, DELETER);
}

template<typename FirstType, typename SecondType>
void json_trie_node<FirstType, SecondType>::insert(std::string_view key, boost::intrusive_ptr<json_trie_node> &&value) {
  children_[key] = std::forward<boost::intrusive_ptr<json_trie_node>>(value);
}

template<typename FirstType, typename SecondType>
boost::intrusive_ptr<json_trie_node<FirstType, SecondType>> json_trie_node<FirstType, SecondType>::erase(std::string_view key) {
  auto found = children_.find(key);
  if (found == children_.end()) {
    return nullptr;
  }
  auto copy = found->second;
  children_.erase(found);
  return copy;
}

template<typename FirstType, typename SecondType>
boost::intrusive_ptr<json_trie_node<FirstType, SecondType>>
json_trie_node<FirstType, SecondType>::make_deep_copy(std::string_view key) {
  auto found = children_.find(key);
  if (found == children_.end()) {
    return nullptr;
  }
  return make_deep_copy_(found->second.get());
}

template<typename FirstType, typename SecondType>
size_t json_trie_node<FirstType, SecondType>::size() const {
  return children_.size();
}

template<typename FirstType, typename SecondType>
bool json_trie_node<FirstType, SecondType>::is_object() const {
  return type_ == OBJECT;
}

template<typename FirstType, typename SecondType>
bool json_trie_node<FirstType, SecondType>::is_array() const {
  return type_ == ARRAY;
}

template<typename FirstType, typename SecondType>
bool json_trie_node<FirstType, SecondType>::is_terminal() const {
  return type_ == TERMINAL;
}

template<typename FirstType, typename SecondType>
bool json_trie_node<FirstType, SecondType>::is_deleter() const {
  return type_ == DELETER;
}

template<typename FirstType, typename SecondType>
std::pmr::string
json_trie_node<FirstType, SecondType>::to_json(
        std::pmr::string (*to_json_first)(FirstType *, std::pmr::memory_resource *),
        std::pmr::string (*to_json_second)(SecondType *, std::pmr::memory_resource *)
) const {
  if (is_object()) {
    return to_json_object(to_json_first, to_json_second);
  }
  if (is_array()) {
    return to_json_array(to_json_first, to_json_second);
  }
  if (is_deleter()) {
    return {"DELETER", allocator_};
  }
  return is_first_ ? to_json_first(value_.first, allocator_) : to_json_second(value_.second, allocator_);
}

template<typename FirstType, typename SecondType>
json_trie_node<FirstType, SecondType> *
json_trie_node<FirstType, SecondType>::merge(
        json_trie_node<FirstType, SecondType> *node1,
        json_trie_node<FirstType, SecondType> *node2,
        allocator_type &allocator
) {
  if (!node2->is_object()) {
    return node2;
  }
  auto res = new(allocator.allocate(sizeof(json_trie_node))) json_trie_node(&allocator);
  for (auto &it : node2->children_) {
    if (it.second->is_deleter()) {
      continue;
    }
    auto next = node1->children_.find(it.first);
    if (next == node1->children_.end()) {
      res->children_[it.first] = it.second;
    } else {
      res->children_[it.first] = merge(next->second.get(), it.second.get(), allocator);
    }
  }
  for (auto &entry : node1->children_) {
    if (node2->children_.find(entry.first) == node2->children_.end()) {
      res->children_[entry.first] = entry.second;
    }
  }
  return res;
}

template<typename FirstType, typename SecondType>
bool json_trie_node<FirstType, SecondType>::equals(
        json_trie_node<FirstType, SecondType> *node1,
        json_trie_node<FirstType, SecondType> *node2,
        bool (* first_equals_first)(FirstType *, FirstType *),
        bool (* second_equals_second)(SecondType *, SecondType *),
        bool (* first_equals_second)(FirstType *, SecondType *)
) {
  if (node1->type_ != node2->type_) {
    return false;
  }
  if (node1->is_terminal() || node1->is_deleter()) {
    if (node1->is_first_ == node2->is_first_) {
      if (node1->is_first_) {
        return first_equals_first(node1->value_.first, node2->value_.first);
      }
      return second_equals_second(node1->value_.second, node2->value_.second);
    }
    if (node1->is_first_) {
      return first_equals_second(node1->value_.first, node2->value_.second);
    }
    return first_equals_second(node2->value_.first, node1->value_.second);
  }
  if (node1->size() != node2->size()) {
    return false;
  }
  for (auto &it: node1->children_) {
    auto next_node2 = node2->children_.find(it.first);
    if (
            next_node2 == node2->children_.end() ||
            !equals(
                    it.second.get(),
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
std::pmr::string json_trie_node<FirstType, SecondType>::to_json_object(
        std::pmr::string (*to_json_first)(FirstType *, std::pmr::memory_resource *),
        std::pmr::string (*to_json_second)(SecondType *, std::pmr::memory_resource *)
) const {
  std::pmr::string res(allocator_);
  res.append("{");
  for (auto &it : children_) {
    auto key = it.first;
    if (res.size() > 1) {
      res.append(",");
    }
    res.append("\"").append(key).append("\"").append(":").append(it.second->to_json(to_json_first, to_json_second));
  }
  return res.append("}");
}

template<typename FirstType, typename SecondType>
std::pmr::string json_trie_node<FirstType, SecondType>::to_json_array(
        std::pmr::string (*to_json_first)(FirstType *, std::pmr::memory_resource *),
        std::pmr::string (*to_json_second)(SecondType *, std::pmr::memory_resource *)
) const {
  json_trie_node<FirstType, SecondType> *temp_arr[size()];
  for (auto &it : children_) {
    temp_arr[std::atol(it.first.c_str())] = it.second.get();
  }
  std::pmr::string res(allocator_);
  res.append("[");
  for (auto &it : temp_arr) {
    if (res.size() > 1) {
      res.append(",");
    }
    res.append(it->to_json(to_json_first, to_json_second));
  }
  return res.append("]");
}

template<typename FirstType, typename SecondType>
boost::intrusive_ptr<json_trie_node<FirstType, SecondType>>
json_trie_node<FirstType, SecondType>::make_deep_copy_(json_trie_node<FirstType, SecondType> *node) {
  auto node_copy = new(allocator_->allocate(sizeof(json_trie_node))) json_trie_node(*node);
  for (auto &it : node->children_) {
    node_copy->children_[it.first] = it.second->make_deep_copy_(it.second.get());
  }
  return node_copy;
}