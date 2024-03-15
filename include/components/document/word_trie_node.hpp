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

template<typename T>
class word_trie_node : public boost::intrusive_ref_counter<word_trie_node<T>> {
public:
  word_trie_node();

  boost::intrusive_ptr<word_trie_node<T>> get_node(std::string_view words);

  const word_trie_node<T> *find_node_const(std::string_view words) const;

  word_trie_node<T> *find_node(std::string_view words);

  const T *find(std::string_view words = "") const;

  void insert(std::string_view words, const T &value);

  bool erase(std::string_view words);

  size_t size() const;

private:
  std::unordered_map<std::string, boost::intrusive_ptr<word_trie_node>> children_;
  std::unique_ptr<T> value_;
};

template<typename T>
word_trie_node<T>::word_trie_node() : value_(nullptr) {}

template<typename T>
boost::intrusive_ptr<word_trie_node<T>> word_trie_node<T>::get_node(std::string_view words) {
  assert(!words.empty());
  auto *current = this;
  boost::intrusive_ptr<word_trie_node<T>> current_ptr;
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

template<typename T>
const word_trie_node<T> *word_trie_node<T>::find_node_const(std::string_view words) const {
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

template<typename T>
word_trie_node<T> *word_trie_node<T>::find_node(std::string_view words) {
  return const_cast<word_trie_node<T> *>(find_node_const(words));
}

template<typename T>
const T *word_trie_node<T>::find(std::string_view words) const {
  auto node = find_node_const(words);
  if (node == nullptr) {
    return nullptr;
  }
  return node->value_.get();
}

template<typename T>
void word_trie_node<T>::insert(std::string_view words, const T &value) {
  word_trie_node *current = this;
  for (auto word_sv: string_splitter(words, '/')) {
    auto word = std::string(word_sv);
    if (current->children_.find(word) == current->children_.end()) {
      current->children_[word] = new word_trie_node;
    }
    current = current->children_[word].get();
  }
  current->value_ = std::make_unique<T>(value);
}

template<typename T>
bool word_trie_node<T>::erase(std::string_view words) {
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

template<typename T>
size_t word_trie_node<T>::size() const {
  return children_.size();
}
