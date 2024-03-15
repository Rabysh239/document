#include "word_trie_node.hpp"

string_split_iterator::string_split_iterator(std::string_view str, char delim, bool end)
        : str_(str), delim_(delim), end_(end) {
  if (!end_) {
    if (str_[0] == delim) {
      str_.remove_prefix(1);
    }
    ++(*this);
  }
}

string_split_iterator::reference string_split_iterator::operator*() const { return current_; }

string_split_iterator::pointer string_split_iterator::operator->() const { return &current_; }

string_split_iterator &string_split_iterator::operator++() {
  if (end_) return *this;

  auto pos = str_.find(delim_);
  if (pos != std::string_view::npos) {
    current_ = str_.substr(0, pos);
    str_.remove_prefix(pos + 1);
  } else if (current_ != str_) {
    current_ = str_;
  } else {
    end_ = true;
  }
  return *this;
}

string_split_iterator string_split_iterator::operator++(int) {
  string_split_iterator tmp = *this;
  ++(*this);
  return tmp;
}

bool operator==(const string_split_iterator &a, const string_split_iterator &b) {
  return a.end_ == b.end_ && (a.end_ || a.str_.data() == b.str_.data());
}

bool operator!=(const string_split_iterator &a, const string_split_iterator &b) {
  return !(a == b);
}

string_splitter::string_splitter(std::string_view str, char delim)
        : str_(str), delim_(delim) {}

string_split_iterator string_splitter::begin() const { return {str_, delim_}; }

string_split_iterator string_splitter::end() const { return {str_, delim_, true}; }