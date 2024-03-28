#pragma once

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <absl/container/flat_hash_map.h>
#include <iostream>
#include <memory>
#include <memory_resource>
#include <atomic>
#include <charconv>
#include <allocator_intrusive_ref_counter.hpp>

class string_split_iterator {
public:
  using iterator_category = std::input_iterator_tag;
  using value_type = std::string_view;
  using difference_type = std::ptrdiff_t;
  using pointer = const value_type *;
  using reference = const value_type &;

  string_split_iterator(std::string_view str, char delim, bool end = false);

  reference operator*() const;

  pointer operator->() const;

  string_split_iterator &operator++();

  string_split_iterator operator++(int);

  friend bool operator==(const string_split_iterator &a, const string_split_iterator &b);

  friend bool operator!=(const string_split_iterator &a, const string_split_iterator &b);

private:
  std::string_view str_;
  char delim_;
  bool end_ = false;
  value_type current_;
};

class string_splitter {
public:
  string_splitter(std::string_view str, char delim);

  string_split_iterator begin() const;

  string_split_iterator end() const;

private:
  std::string_view str_;
  char delim_;
};

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
class word_trie_node : public allocator_intrusive_ref_counter {
public:
  using allocator_type = std::pmr::memory_resource;

  explicit word_trie_node(allocator_type *allocator);

  ~word_trie_node() override;

  word_trie_node(word_trie_node &&) noexcept;

  word_trie_node(const word_trie_node &) = delete;

  word_trie_node &operator=(word_trie_node &&) noexcept;

  word_trie_node &operator=(const word_trie_node &) = delete;

  const word_trie_node<FirstType, SecondType> *find_node_const(std::string_view words) const;

  word_trie_node<FirstType, SecondType> *find_node(std::string_view words);

  const FirstType *get_value_first() const;

  const SecondType *get_value_second() const;

  void insert(std::string_view words, const FirstType &value, bool is_aggregation_terminal);

  void insert(std::string_view words, const SecondType &value, bool is_aggregation_terminal);

  bool erase(std::string_view words);

  size_t size() const;

  static word_trie_node<FirstType, SecondType> *merge(word_trie_node<FirstType, SecondType> *node1, word_trie_node<FirstType, SecondType> *node2, allocator_type &allocator);

  static word_trie_node<FirstType, SecondType> *split(word_trie_node<FirstType, SecondType> *node1, word_trie_node<FirstType, SecondType> *node2, allocator_type &allocator);

private:
  allocator_type *allocator_;
  absl::flat_hash_map<
          std::pmr::string,
          boost::intrusive_ptr<word_trie_node>,
          string_view_hash, string_view_eq,
          std::pmr::polymorphic_allocator<std::pair<const std::pmr::string, boost::intrusive_ptr<word_trie_node>>>
  > children_;
  union {
    FirstType* first;
    SecondType* second;
  } value_;
  bool is_first_;
  bool is_aggregation_terminal_;

  word_trie_node<FirstType, SecondType> *find_insert(std::string_view words);
};

template<typename FirstType, typename SecondType>
word_trie_node<FirstType, SecondType>::word_trie_node(allocator_type *allocator)
        : allocator_intrusive_ref_counter(allocator),
          allocator_(allocator),
          children_(allocator_),
          value_({.second = nullptr}),
          is_first_(false),
          is_aggregation_terminal_(false) {}

template<typename FirstType, typename SecondType>
word_trie_node<FirstType, SecondType>::~word_trie_node() {
  auto ptr = is_first_ ? reinterpret_cast<void *>(value_.first) : reinterpret_cast<void *>(value_.second);
  if (ptr != nullptr) {
    if (is_first_) {
      value_.first->~FirstType();
      allocator_->deallocate(ptr, sizeof(FirstType));
    } else {
      value_.second->~SecondType();
      allocator_->deallocate(ptr, sizeof(SecondType));
    }
  }
}

template<typename FirstType, typename SecondType>
word_trie_node<FirstType, SecondType>::word_trie_node(word_trie_node &&other) noexcept
        : allocator_(other.allocator_),
          children_(std::move(other.children_)),
          value_(std::move(other.value_)),
          is_first_(other.is_first_),
          is_aggregation_terminal_(other.is_aggregation_terminal_) {
  other.allocator_ = nullptr;
  if (other.is_first_) {
    value_.first = nullptr;
  } else {
    value_.second = nullptr;
  }
}

template<typename FirstType, typename SecondType>
word_trie_node<FirstType, SecondType> &word_trie_node<FirstType, SecondType>::operator=(word_trie_node &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  allocator_ = other.allocator_;
  children_ = std::move(other.children_);
  value_ = std::move(other.value_);
  is_first_ = other.is_first_;
  is_aggregation_terminal_ = other.is_aggregation_terminal_;
  other.allocator_ = nullptr;
  if (other.is_first_) {
    value_.first = nullptr;
  } else {
    value_.second = nullptr;
  }
  return *this;
}

template<typename FirstType, typename SecondType>
const word_trie_node<FirstType, SecondType> *word_trie_node<FirstType, SecondType>::find_node_const(std::string_view words) const {
  const auto *current = this;
  for (auto word: string_splitter(words, '/')) {
    auto next = current->children_.find(word);
    if (next == current->children_.end()) {
      return nullptr;
    }
    current = next->second.get();
  }
  return current;
}

template<typename FirstType, typename SecondType>
word_trie_node<FirstType, SecondType> *word_trie_node<FirstType, SecondType>::find_node(std::string_view words) {
  return const_cast<word_trie_node<FirstType, SecondType> *>(find_node_const(words));
}

template<typename FirstType, typename SecondType>
const FirstType *word_trie_node<FirstType, SecondType>::get_value_first() const {
  if (!is_first_) {
    return nullptr;
  }
  return value_.first;
}

template<typename FirstType, typename SecondType>
const SecondType *word_trie_node<FirstType, SecondType>::get_value_second() const {
  if (is_first_) {
    return nullptr;
  }
  return value_.second;
}

template<typename FirstType, typename SecondType>
void word_trie_node<FirstType, SecondType>::insert(std::string_view words, const FirstType &value, bool is_aggregation_terminal) {
  auto node = find_insert(words);
  node->is_first_ = true;
  node->value_.first = new(allocator_->allocate(sizeof(FirstType))) FirstType(value);
  node->is_aggregation_terminal_ = is_aggregation_terminal;
}

template<typename FirstType, typename SecondType>
void word_trie_node<FirstType, SecondType>::insert(std::string_view words, const SecondType &value, bool is_aggregation_terminal) {
  auto node = find_insert(words);
  node->value_.second = new(allocator_->allocate(sizeof(SecondType))) SecondType(value);
  node->is_aggregation_terminal_ = is_aggregation_terminal;
}

template<typename FirstType, typename SecondType>
bool word_trie_node<FirstType, SecondType>::erase(std::string_view words) {
  word_trie_node *current = this;
  word_trie_node *prev = nullptr;
  std::string_view word_s;
  for (auto word: string_splitter(words, '/')) {
    auto next = current->children_.find(word);
    if (next == current->children_.end()) {
      return false;
    }
    current = next->second.get();
    word_s = word;
  }
  prev->children_.erase(word_s);
  return true;
}

template<typename FirstType, typename SecondType>
size_t word_trie_node<FirstType, SecondType>::size() const {
  return children_.size();
}

template<typename FirstType, typename SecondType>
word_trie_node<FirstType, SecondType> *word_trie_node<FirstType, SecondType>::merge(word_trie_node<FirstType, SecondType> *node1, word_trie_node<FirstType, SecondType> *node2, allocator_type &allocator) {
  if (node2->is_aggregation_terminal_) {
    return node2;
  }
  auto res = new(allocator.allocate(sizeof(word_trie_node))) word_trie_node(&allocator);
  for (auto &entry : node1->children_) {
    auto next = node2->children_.find(entry.first);
    if (next == node2->children_.end()) {
      res->children_[entry.first] = entry.second;
    } else {
      res->children_[entry.first] = merge(entry.second.get(), next->second.get(), allocator);
    }
  }
  for (auto &entry : node2->children_) {
    if (node1->children_.find(entry.first) == node1->children_.end()) {
      res->children_[entry.first] = entry.second;
    }
  }
  return res;
}

template<typename FirstType, typename SecondType>
word_trie_node<FirstType, SecondType> *word_trie_node<FirstType, SecondType>::split(word_trie_node<FirstType, SecondType> *node1, word_trie_node<FirstType, SecondType> *node2, allocator_type &allocator) {
  if (node2->is_aggregation_terminal_) {
    return nullptr;
  }
  auto res = new(allocator.allocate(sizeof(word_trie_node))) word_trie_node(&allocator);
  for (auto &entry : node1->children_) {
    auto next = node2->children_.find(entry.first);
    if (next == node2->children_.end()) {
      res->children_[entry.first] = entry.second;
    } else {
      auto split_res = split(entry.second.get(), next->second.get(), allocator);
      if (split_res != nullptr) {
        res->children_[entry.first] = split_res;
      }
    }
  }
  return res;
}

template<typename FirstType, typename SecondType>
word_trie_node<FirstType, SecondType> *word_trie_node<FirstType, SecondType>::find_insert(std::string_view words) {
  word_trie_node *current = this;
  for (auto word: string_splitter(words, '/')) {
    auto next = current->children_.find(word);
    if (next == current->children_.end()) {
      current = (current->children_[word] = new(allocator_->allocate(sizeof(word_trie_node))) word_trie_node(allocator_)).get();
      return current;
    } else {
      current = next->second.get();
    }
  }
  return current;
}

