#include "document.hpp"
#include <boost/json/src.hpp>
#include <utility>
#include "../../../src/generic/stage2/json_iterator.h"

namespace components::document {

document_t::document_t()
        : allocator_(nullptr),
          element_ind_(nullptr) {}

document_t::document_t(document_t &&other) noexcept
        : mut_src_(std::move(other.mut_src_)),
          immut_src_(std::move(other.immut_src_)),
          builder_(std::move(other.builder_)),
          allocator_(other.allocator_),
          element_ind_(other.element_ind_),
          ancestors_(std::move(other.ancestors_)) {
  other.allocator_ = nullptr;
  other.element_ind_ = nullptr;
}

document_t &document_t::operator=(document_t &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  mut_src_ = std::move(other.mut_src_);
  immut_src_ = std::move(other.immut_src_);
  builder_ = std::move(other.builder_);
  allocator_ = other.allocator_;
  element_ind_ = other.element_ind_;
  ancestors_ = std::move(other.ancestors_);
  other.allocator_ = nullptr;
  other.element_ind_ = nullptr;
  return *this;
}

document_t::document_t(document_t::allocator_type *allocator)
        : allocator_(allocator),
          ancestors_(allocator_),
          element_ind_(new(allocator_->allocate(sizeof(word_trie_node_element))) word_trie_node_element(allocator_)) {}

bool document_t::is_valid() const {
  return allocator_ != nullptr;
}

std::size_t document_t::count(std::string_view json_pointer) const {
  const auto value_ptr = element_ind_->find_node_const(json_pointer);
  if (value_ptr == nullptr) {
    return 0;
  }
  return value_ptr->size();
}

bool document_t::is_exists(std::string_view json_pointer) const {
  return element_ind_->find_node_const(json_pointer) != nullptr;
}

bool document_t::is_null(std::string_view json_pointer) const {
  const auto node_ptr = element_ind_->find_node_const(json_pointer);
  if (node_ptr == nullptr) {
    return false;
  }
  auto first = node_ptr->get_value_first();
  return first != nullptr ? first->is_null() : node_ptr->get_value_second()->is_null();
}

bool document_t::is_bool(std::string_view json_pointer) const { return is_as<bool>(json_pointer); }

bool document_t::is_ulong(std::string_view json_pointer) const { return is_as<uint64_t>(json_pointer); }

bool document_t::is_long(std::string_view json_pointer) const { return is_as<int64_t>(json_pointer); }

bool document_t::is_double(std::string_view json_pointer) const { return is_as<double>(json_pointer); }

bool document_t::is_string(std::string_view json_pointer) const { return is_as<std::string_view>(json_pointer); }

bool document_t::is_array(std::string_view json_pointer) const {
  const auto node_ptr = element_ind_->find_node(json_pointer);
  return node_ptr != nullptr && is_array(*node_ptr);
}

bool document_t::is_dict(std::string_view json_pointer) const {
  const auto node_ptr = element_ind_->find_node(json_pointer);
  return node_ptr != nullptr && is_object(*node_ptr);
}

bool document_t::get_bool(std::string_view json_pointer) const { return get_as<bool>(json_pointer); }

uint64_t document_t::get_ulong(std::string_view json_pointer) const { return get_as<uint64_t>(json_pointer); }

int64_t document_t::get_long(std::string_view json_pointer) const { return get_as<int64_t>(json_pointer); }

double document_t::get_double(std::string_view json_pointer) const { return get_as<double>(json_pointer); }

std::string document_t::get_string(std::string_view json_pointer) const {
  return std::string(get_as<std::string_view>(json_pointer));
}

document_t::ptr document_t::get_array(std::string_view json_pointer) {
  const auto node_ptr = element_ind_->find_node(json_pointer);
  if (node_ptr == nullptr || !is_array(*node_ptr)) {
    return nullptr; // temporarily
  }
  return new document_t({this}, allocator_, node_ptr);
}

document_t::ptr document_t::get_dict(std::string_view json_pointer) {
  const auto node_ptr = element_ind_->find_node(json_pointer);
  if (node_ptr == nullptr || !is_object(*node_ptr)) {
    return nullptr; // temporarily
  }
  return new document_t({this}, allocator_, node_ptr);
}

template<class T>
compare_t equals_(const document_t& doc1, const document_t& doc2, std::string_view json_pointer) {
  T v1 = doc1.get_as<T>(json_pointer);
  T v2 = doc2.get_as<T>(json_pointer);
  if (v1 < v2)
    return compare_t::less;
  if (v1 > v2)
    return compare_t::more;
  return compare_t::equals;
}

compare_t document_t::compare(const document_t& other, std::string_view json_pointer) const {
  if (is_valid() && !other.is_valid())
    return compare_t::less;
  if (!is_valid() && other.is_valid())
    return compare_t::more;
  if (!is_valid())
    return compare_t::equals;
  if (is_exists(json_pointer) && !other.is_exists(json_pointer))
    return compare_t::less;
  if (!is_exists(json_pointer) && other.is_exists(json_pointer))
    return compare_t::more;
  if (!is_exists(json_pointer) && !other.is_exists(json_pointer))
    return compare_t::equals;
  if (is_bool(json_pointer) && other.is_bool(json_pointer))
    return equals_<bool>(*this, other, json_pointer);
  if (is_ulong(json_pointer) && other.is_ulong(json_pointer))
    return equals_<uint64_t>(*this, other, json_pointer);
  if (is_long(json_pointer) && other.is_long(json_pointer))
    return equals_<int64_t>(*this, other, json_pointer);
  if (is_double(json_pointer) && other.is_double(json_pointer))
    return equals_<double>(*this, other, json_pointer);
  if (is_string(json_pointer) && other.is_string(json_pointer))
    return equals_<std::string_view>(*this, other, json_pointer);
  return compare_t::equals;
}

document_t::document_t(ptr ancestor, allocator_type *allocator, word_trie_node_element* index)
        : allocator_(allocator),
          element_ind_(index),
          ancestors_(std::pmr::vector<ptr>({std::move(ancestor)}, allocator_)) {}

error_t document_t::set_(std::string_view json_pointer, const element_from_mutable &value) {
  size_t pos = json_pointer.find_last_of('/');
  if (pos == std::string::npos) {
    return error_t::INVALID_JSON_POINTER;
  }
  auto container_json_pointer = json_pointer.substr(0, pos);
  auto container_node_ptr = element_ind_->find_node(container_json_pointer);
  if (container_node_ptr == nullptr) {
    return error_t::NO_SUCH_CONTAINER;
  }
  auto key = json_pointer.substr(pos + 1);
  auto is_aggregation_terminal = true;
  if (is_object(*container_node_ptr)) {
    container_node_ptr->insert(key, value, is_aggregation_terminal);
  } else if (is_array(*container_node_ptr)) {
    auto index = std::atol(std::string(key).c_str());
    if (index < 0) {
      return error_t::INVALID_INDEX;
    }
    auto correct_index = std::min(size_t(index), container_node_ptr->size());
    container_node_ptr->insert(std::to_string(correct_index), value, is_aggregation_terminal);
  }
  return error_t::SUCCESS;
}

void document_t::build_index(word_trie_node_element &node, const element_from_immutable &value, std::string_view key) {
  node.insert(key, value, !value.is_object());
  if (value.is_object()) {
    const auto obj = value.get_object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
      build_index(*node.find_node(key), it.value(), it.key());
    }
  } else if (value.is_array()) {
    const auto arr = value.get_array();
    int i = 0;
    for (auto it: arr) {
      build_index(*node.find_node(key), it, std::to_string(i++));
    }
  }
}

document_t::ptr document_t::document_from_json(const std::string &json, document_t::allocator_type *allocator) {
  auto res = new document_t(allocator);
  if (res->immut_src_.allocate(json.size()) != simdjson::SUCCESS) {
    return nullptr;
  }
  auto tree = boost::json::parse(json);
  simdjson::SIMDJSON_IMPLEMENTATION::stage2::tape_builder<simdjson::dom::tape_writer_to_immutable> builder(res->immut_src_);
  walk_document(builder, tree);
  build_index(*res->element_ind_, res->immut_src_.root(), "");
  return res;
}

document_t::ptr document_t::merge(document_t::ptr &document1, document_t::ptr &document2, document_t::allocator_type *allocator) {
  auto res = new document_t(allocator);
  res->ancestors_.push_back(document1);
  res->ancestors_.push_back(document2);
  res->element_ind_ = word_trie_node_element::merge(document1->element_ind_.get(), document2->element_ind_.get(), *res->allocator_);
  return res;
}

document_t::ptr document_t::split(document_t::ptr &document1, document_t::ptr &document2, document_t::allocator_type *allocator) {
  auto res = new document_t(allocator);
  res->ancestors_.push_back(document1);
  res->ancestors_.push_back(document2);
  res->element_ind_ = word_trie_node_element::split(document1->element_ind_.get(), document2->element_ind_.get(), *res->allocator_);
  return res;
}

bool document_t::is_array(const word_trie_node_element &node) {
  auto first = node.get_value_first();
  return first != nullptr ? first->is_array() : node.get_value_second()->is_array();
}

bool document_t::is_object(const word_trie_node_element &node) {
  auto first = node.get_value_first();
  return first != nullptr ? first->is_object() : node.get_value_second()->is_object();
}
} // namespace components::document