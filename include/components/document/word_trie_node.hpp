#pragma once

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <iostream>
#include <memory>

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
class word_trie_node : public boost::intrusive_ref_counter<word_trie_node<T, K>> {
public:
  word_trie_node();

  boost::intrusive_ptr<word_trie_node<T, K>> get_node(std::string_view words);

  const word_trie_node<T, K> *find_node_const(std::string_view words) const;

  word_trie_node<T, K> *find_node(std::string_view words);

  const T *get_value_first() const;

  const K *get_value_second() const;

  void insert(std::string_view words, const T &value);

  void insert(std::string_view words, const K &value);

  bool erase(std::string_view words);

  size_t size() const;

private:
  std::unordered_map<std::string, boost::intrusive_ptr<word_trie_node>> children_;
  union u {
    std::unique_ptr<T> t_ptr;
    std::unique_ptr<K> k_ptr;

    u() { nullptr; }
    ~u() {};
  } value_;
  bool is_t;

  word_trie_node<T, K> *find_insert(std::string_view words);
};

template<typename T, typename K>
word_trie_node<T, K>::word_trie_node() : is_t(false) {}

template<typename T, typename K>
boost::intrusive_ptr<word_trie_node<T, K>> word_trie_node<T, K>::get_node(std::string_view words) {
  assert(!words.empty());
  auto *current = this;
  boost::intrusive_ptr<word_trie_node<T, K>> current_ptr;
  for (auto word_sv: string_splitter(words, '/')) {
    auto word = std::string(word_sv);
    if (current->children_.find(word) == current->children_.end()) {
      return nullptr;
    }
    current_ptr = current->children_.at(word);
    current = current_ptr.get();
  }
  return current_ptr;
}

template<typename T, typename K>
const word_trie_node<T, K> *word_trie_node<T, K>::find_node_const(std::string_view words) const {
  const auto *current = this;
  for (auto word_sv: string_splitter(words, '/')) {
    auto word = std::string(word_sv);
    if (current->children_.find(word) == current->children_.end()) {
      return nullptr;
    }
    current = current->children_.at(word).get();
  }
  return current;
}

template<typename T, typename K>
word_trie_node<T, K> *word_trie_node<T, K>::find_node(std::string_view words) {
  return const_cast<word_trie_node<T, K> *>(find_node_const(words));
}

template<typename T, typename K>
const T *word_trie_node<T, K>::get_value_first() const {
  if (!is_t) {
    return nullptr;
  }
  return value_.t_ptr.get();
}

template<typename T, typename K>
const K *word_trie_node<T, K>::get_value_second() const {
  if (is_t) {
    return nullptr;
  }
  return value_.k_ptr.get();
}

template<typename T, typename K>
void word_trie_node<T, K>::insert(std::string_view words, const T &value) {
  auto node = find_insert(words);
  node->is_t = true;
  node->value_.t_ptr = std::make_unique<T>(value);
}

template<typename T, typename K>
void word_trie_node<T, K>::insert(std::string_view words, const K &value) {
  auto node = find_insert(words);
  node->value_.k_ptr = std::make_unique<K>(value);
}

template<typename T, typename K>
bool word_trie_node<T, K>::erase(std::string_view words) {
  word_trie_node *current = this;
  word_trie_node *prev = nullptr;
  std::string word;
  for (auto word_sv: string_splitter(words, '/')) {
    word = std::string(word_sv);
    if (current->children_.find(word) == current->children_.end()) {
      return false;
    }
    current = current->children_[word].get();
  }
  prev->children_.erase(word);
  return true;
}

template<typename T, typename K>
size_t word_trie_node<T, K>::size() const {
  return children_.size();
}

template<typename T, typename K>
word_trie_node<T, K> *word_trie_node<T, K>::find_insert(std::string_view words) {
  word_trie_node *current = this;
  for (auto word_sv: string_splitter(words, '/')) {
    auto word = std::string(word_sv);
    if (current->children_.find(word) == current->children_.end()) {
      current->children_[word] = new word_trie_node;
    }
    current = current->children_[word].get();
  }
  return current;
}

