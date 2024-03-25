#pragma once

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <iostream>
#include <memory>
#include <memory_resource>

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

template<typename T, typename K>
class word_trie_node {
public:
  using allocator_type = std::pmr::memory_resource;

  explicit word_trie_node(allocator_type *allocator);

  const word_trie_node<T, K> *find_node_const(std::string_view words) const;

  word_trie_node<T, K> *find_node(std::string_view words);

  const T *get_value_first() const;

  const K *get_value_second() const;

  void insert(std::string_view words, const T &value, bool is_aggregation_terminal);

  void insert(std::string_view words, const K &value, bool is_aggregation_terminal);

  bool erase(std::string_view words);

  size_t size() const;

  static word_trie_node<T, K> *merge(word_trie_node<T, K> *node1, word_trie_node<T, K> *node2, allocator_type &allocator);

  static word_trie_node<T, K> *split(word_trie_node<T, K> *node1, word_trie_node<T, K> *node2, allocator_type &allocator);

private:
  allocator_type *allocator_;
  std::pmr::unordered_map<std::pmr::string, word_trie_node *> children_;
  union {
    T* t_ptr;
    K* k_ptr;
  } value_;
  bool is_t_;
  bool is_aggregation_terminal_;

  word_trie_node<T, K> *find_insert(std::string_view words);
};

template<typename T, typename K>
word_trie_node<T, K>::word_trie_node(allocator_type *allocator)
        : allocator_(allocator),
          children_(allocator_),
          value_({.k_ptr = nullptr}),
          is_t_(false) {}

template<typename T, typename K>
const word_trie_node<T, K> *word_trie_node<T, K>::find_node_const(std::string_view words) const {
  const auto *current = this;
  for (auto word_sv: string_splitter(words, '/')) {
    auto word = std::pmr::string(word_sv, allocator_);
    if (current->children_.find(word) == current->children_.end()) {
      return nullptr;
    }
    current = current->children_.at(word);
  }
  return current;
}

template<typename T, typename K>
word_trie_node<T, K> *word_trie_node<T, K>::find_node(std::string_view words) {
  return const_cast<word_trie_node<T, K> *>(find_node_const(words));
}

template<typename T, typename K>
const T *word_trie_node<T, K>::get_value_first() const {
  if (!is_t_) {
    return nullptr;
  }
  return value_.t_ptr;
}

template<typename T, typename K>
const K *word_trie_node<T, K>::get_value_second() const {
  if (is_t_) {
    return nullptr;
  }
  return value_.k_ptr;
}

template<typename T, typename K>
void word_trie_node<T, K>::insert(std::string_view words, const T &value, bool is_aggregation_terminal) {
  auto node = find_insert(words);
  node->is_t_ = true;
  node->value_.t_ptr = new(allocator_->allocate(sizeof(T))) T(value);
  node->is_aggregation_terminal_ = is_aggregation_terminal;
}

template<typename T, typename K>
void word_trie_node<T, K>::insert(std::string_view words, const K &value, bool is_aggregation_terminal) {
  auto node = find_insert(words);
  node->value_.k_ptr = new(allocator_->allocate(sizeof(K))) K(value);
  node->is_aggregation_terminal_ = is_aggregation_terminal;
}

template<typename T, typename K>
bool word_trie_node<T, K>::erase(std::string_view words) {
  word_trie_node *current = this;
  word_trie_node *prev = nullptr;
  std::pmr::string word;
  for (auto word_sv: string_splitter(words, '/')) {
    word = word_sv;
    auto next = current->children_.find(word);
    if (next == current->children_.end()) {
      return false;
    }
    current = next->second;
  }
  prev->children_.erase(word);
  return true;
}

template<typename T, typename K>
size_t word_trie_node<T, K>::size() const {
  return children_.size();
}

template<typename T, typename K>
word_trie_node<T, K> *word_trie_node<T, K>::merge(word_trie_node<T, K> *node1, word_trie_node<T, K> *node2, allocator_type &allocator) {
  if (node2->is_aggregation_terminal_) {
    return node2;
  }
  auto res = new(allocator.allocate(sizeof(word_trie_node))) word_trie_node(&allocator);
  for (auto &entry : node1->children_) {
    auto next = node2->children_.find(entry.first);
    if (next == node2->children_.end()) {
      res->children_[entry.first] = entry.second;
    } else {
      res->children_[entry.first] = merge(entry.second, next->second, allocator);
    }
  }
  for (auto &entry : node2->children_) {
    if (node1->children_.find(entry.first) == node1->children_.end()) {
      res->children_[entry.first] = entry.second;
    }
  }
  return res;
}

template<typename T, typename K>
word_trie_node<T, K> *word_trie_node<T, K>::split(word_trie_node<T, K> *node1, word_trie_node<T, K> *node2, allocator_type &allocator) {
  if (node2->is_aggregation_terminal_) {
    return nullptr;
  }
  auto res = new(allocator.allocate(sizeof(word_trie_node))) word_trie_node(&allocator);
  for (auto &entry : node1->children_) {
    auto next = node2->children_.find(entry.first);
    if (next == node2->children_.end()) {
      res->children_[entry.first] = entry.second;
    } else {
      auto split_res = split(entry.second, next->second, allocator);
      if (split_res != nullptr) {
        res->children_[entry.first] = split_res;
      }
    }
  }
  return res;
}

template<typename T, typename K>
word_trie_node<T, K> *word_trie_node<T, K>::find_insert(std::string_view words) {
  word_trie_node *current = this;
  for (auto word_sv: string_splitter(words, '/')) {
    auto word = std::pmr::string(word_sv, allocator_);
    auto next = current->children_.find(word);
    if (next == current->children_.end()) {
      current = (current->children_[word] = new(allocator_->allocate(sizeof(word_trie_node))) word_trie_node(allocator_));
      return current;
    } else {
      current = next->second;
    }
  }
  return current;
}

