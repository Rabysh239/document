#include "document.hpp"
#include <boost/json/src.hpp>
#include "../../../src/generic/stage2/tape_builder.h"
#include "../../simdjson/dom/document-inl.h"

namespace components::document {

document_t::document_t(simdjson::dom::element value) : value_(value) {
  build_index(value, "");
}

bool document_t::is_exists(std::string_view json_pointer, const std::string &key) const { return get(json_pointer, key).error() == simdjson::SUCCESS; }

bool document_t::is_exists(std::string_view json_pointer, std::string_view key) const { return get(json_pointer, key).error() == simdjson::SUCCESS; }

bool document_t::is_exists(std::string_view json_pointer, uint32_t index) const { return get(json_pointer, index).error() == simdjson::SUCCESS; }

bool document_t::is_bool(std::string_view json_pointer, const std::string &key) const { return get(json_pointer, key).is_bool(); }

bool document_t::is_bool(std::string_view json_pointer, uint32_t index) const { return get(json_pointer, index).is_bool(); }

simdjson::simdjson_result<simdjson::dom::element> document_t::get(std::string_view json_pointer) const {
  try {
    return simdjson::dom::element(json_pointer_index.at(json_pointer));
  } catch(...) {
    return simdjson::NO_SUCH_POINTER;
  }
}

simdjson::simdjson_result<simdjson::dom::element> document_t::get(std::string_view json_pointer, const std::string& key) const {
  const auto value = get(json_pointer);
  if (value.is_array()) {
    try {
      return get(value, uint32_t(atol(key.c_str())));
    } catch (...) {
      return simdjson::INDEX_OUT_OF_BOUNDS;
    }
  }
  return value.get_object()[key];
}

simdjson::simdjson_result<simdjson::dom::element> document_t::get(std::string_view json_pointer, std::string_view key) const {
  const auto value = get(json_pointer);
  if (value.is_array()) {
    try {
      return get(value, uint32_t(atol(key.data())));
    } catch (...) {
      return simdjson::INDEX_OUT_OF_BOUNDS;
    }
  }
  return value.get_object()[key];
}

simdjson::simdjson_result<simdjson::dom::element> document_t::get(std::string_view json_pointer, uint32_t index) const {
  auto value = get(json_pointer);
  return get(value.value(), index);
}

simdjson::simdjson_result<simdjson::dom::element> document_t::get(const simdjson::dom::element &value, uint32_t index) {
  return value.get_array().at(index);
}

bool document_t::get_bool(std::string_view json_pointer, const std::string &key) const {
  return get_as<bool>(json_pointer, key);
}

void document_t::build_index(const simdjson::dom::element &value, const std::string& json_pointer) {
  json_pointer_index.insert(json_pointer, value);
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

document_ptr document_from_json(simdjson::dom::document &dest, const std::string &json) {
  if (dest.allocate(json.size()) != simdjson::SUCCESS) {
    return nullptr;
  }
  auto tree = boost::json::parse(json);
  simdjson::SIMDJSON_IMPLEMENTATION::stage2::tape_builder::parse_document(dest, tree);
  return new document_t(dest.root());
}
} // namespace components::document