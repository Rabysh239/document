#include "document.hpp"
#include <boost/json/src.hpp>
#include <utility>
#include "../../../src/generic/stage2/json_iterator.h"

namespace components::document {

std::size_t document_t::count(std::string_view json_pointer) const {
  const auto value_ptr = element_ind_->find_node_const(json_pointer);
  if (value_ptr == nullptr) {
    return 0;
  }
  return value_ptr->size();
}

bool document_t::is_exists(std::string_view json_pointer) const { return element_ind_->find(json_pointer) != nullptr; }

bool document_t::is_null(std::string_view json_pointer) const {
  const auto value_ptr = element_ind_->find(json_pointer);
  return value_ptr != nullptr && value_ptr->is_null();
}

bool document_t::is_bool(std::string_view json_pointer) const { return is_as<bool>(json_pointer); }

bool document_t::is_ulong(std::string_view json_pointer) const { return is_as<uint64_t>(json_pointer); }

bool document_t::is_long(std::string_view json_pointer) const { return is_as<int64_t>(json_pointer); }

bool document_t::is_double(std::string_view json_pointer) const { return is_as<double>(json_pointer); }

bool document_t::is_string(std::string_view json_pointer) const { return is_as<double>(json_pointer); }

bool document_t::is_array(std::string_view json_pointer) const { return is_as<simdjson::dom::array>(json_pointer); }

bool document_t::is_dict(std::string_view json_pointer) const { return is_as<simdjson::dom::object>(json_pointer); }

bool document_t::get_bool(std::string_view json_pointer) const { return get_as<bool>(json_pointer); }

uint64_t document_t::get_ulong(std::string_view json_pointer) const { return get_as<uint64_t>(json_pointer); }

int64_t document_t::get_long(std::string_view json_pointer) const { return get_as<int64_t>(json_pointer); }

double document_t::get_double(std::string_view json_pointer) const { return get_as<double>(json_pointer); }

std::string document_t::get_string(std::string_view json_pointer) const { return std::string(get_as<std::string_view>(json_pointer)); }

document_t::ptr document_t::get_array(std::string_view json_pointer) {
  const auto value_ptr = element_ind_->find(json_pointer);
  if (value_ptr != nullptr && value_ptr->is_array()) {
    return new document_t(src_ptr_, element_ind_->get_node(json_pointer));
  }
  return nullptr; //temporarily
}

document_t::document_t(document_t::source_ptr source)
        : src_ptr_(std::move(source)),
          element_ind_(new word_trie_node<simdjson::dom::element>),
          builder_(*src_ptr_) {
  build_index(*element_ind_, src_ptr_->root(), "");
}

document_t::document_t(document_t::source_ptr source, word_trie_ptr root_ptr)
        : src_ptr_(std::move(source)),
          element_ind_(std::move(root_ptr)),
          builder_(*src_ptr_) {}

error_t document_t::set_(std::string_view json_pointer, const simdjson::dom::element &value) {
  size_t pos = json_pointer.find_last_of('/');
  if (pos == std::string::npos) {
    return error_t::INVALID_JSON_POINTER;
  }
  auto container_json_pointer = json_pointer.substr(0, pos);
  auto container_node_ptr = element_ind_->get_node(container_json_pointer);
  if (container_node_ptr == nullptr) {
    return error_t::NO_SUCH_CONTAINER;
  }
  auto *container = container_node_ptr->find();
  auto key = json_pointer.substr(pos + 1);
  if (container->is_object()) {
    container_node_ptr->insert(key, value);
  } else if (container->is_array()) {
    auto index = std::atol(std::string(key).c_str());
    if (index < 0) {
      return error_t::INVALID_INDEX;
    }
    auto correct_index = std::min(size_t(index), container_node_ptr->size());
    container_node_ptr->insert(std::to_string(correct_index), value);
  }
  return error_t::SUCCESS;
}

void document_t::build_index(word_trie_node<simdjson::dom::element>& node, const simdjson::dom::element &value, std::string_view key) {
  node.insert(key, value);
  if (value.is_object()) {
    const auto obj = value.get_object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
      build_index(*node.find_node(key), it.value(), it.key());
    }
  } else if (value.is_array()) {
    const auto arr = value.get_array();
    int i = 0;
    for (auto it : arr) {
      build_index(*node.find_node(key), it, std::to_string(i++));
    }
  }
}

document_t::ptr document_t::document_from_json(const std::string &json) {
  source_ptr source = new simdjson::dom::document;
  if (source->allocate(json.size()) != simdjson::SUCCESS) {
    return nullptr;
  }
  auto tree = boost::json::parse(json);
  simdjson::SIMDJSON_IMPLEMENTATION::stage2::tape_builder builder(*source);
  walk_document(builder, tree);
  return new document_t(source);
}
} // namespace components::document