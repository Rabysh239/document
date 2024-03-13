#include "element_index.hpp"

element_index::element_index(const simdjson::dom::element &value) {
  build_index(value, "");
}

std::optional<simdjson::dom::element> element_index::get(std::string_view key) const {
  // library methods are const but not marked
  auto it = const_cast<element_tree&>(data_).longest_match(std::string(key));
  if (it != const_cast<element_tree&>(data_).end()) {
    return it->second;
  } else {
    return std::nullopt;
  }
}

void element_index::update_or_insert(std::string_view key, const simdjson::dom::element &value)  {
  data_[std::string(key)] = value;
}

void element_index::build_index(const simdjson::dom::element &value, const std::string &json_pointer) {
  data_[json_pointer] = value;
  if (value.is_object()) {
    const auto obj = value.get_object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
      build_index(it.value(), json_pointer + "/" + std::string(it.key()));
    }
  } else if (value.is_array()) {
    const auto arr = value.get_array();
    int i = 0;
    for (auto it : arr) {
      build_index(it, json_pointer + "/" + std::to_string(i++));
    }
  }
}

prefix_index::prefix_index(std::string_view prefix, index_ptr &ind_ptr) : prefix_(std::string(prefix)),
                                                                    ind_ptr_(ind_ptr) {}

std::optional<simdjson::dom::element> prefix_index::get(std::string_view key) const {
  return ind_ptr_->get(prefix_ + std::string(key));
}

void prefix_index::update_or_insert(std::string_view key, const simdjson::dom::element &value) {
  ind_ptr_->update_or_insert(prefix_ + std::string(key), value);
}

