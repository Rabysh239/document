#pragma once

#include <iostream>

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
