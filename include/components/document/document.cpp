#include "document.hpp"
#include <boost/json/src.hpp>
#include "../../../src/generic/stage2/json_iterator.h"
#include "../../simdjson/dom/document-inl.h"

namespace components::document {

document_t::document_t(simdjson::dom::document &&source) : source_(std::forward<simdjson::dom::document>(source)),
                                                           builder_(source_) {
  build_index(source_.root(), "");
}

bool document_t::is_exists(std::string_view json_pointer) const { return get(json_pointer).error() == simdjson::SUCCESS; }

bool document_t::is_bool(std::string_view json_pointer) const { return get(json_pointer).is_bool(); }

bool document_t::is_ulong(std::string_view json_pointer) const { return get(json_pointer).is_uint64(); }

bool document_t::is_long(std::string_view json_pointer) const { return get(json_pointer).is_int64(); }

bool document_t::is_double(std::string_view json_pointer) const { return get(json_pointer).is_double(); }

bool document_t::is_string(std::string_view json_pointer) const { return get(json_pointer).is_string(); }

simdjson::simdjson_result<simdjson::dom::element> document_t::get(std::string_view json_pointer) const {
  try {
    return simdjson::dom::element(json_pointer_index_.at(json_pointer));
  } catch(...) {
    return simdjson::NO_SUCH_POINTER;
  }
}

bool document_t::get_bool(std::string_view json_pointer) const { return get_as<bool>(json_pointer); }

uint64_t document_t::get_ulong(std::string_view json_pointer) const { return get_as<uint64_t>(json_pointer); }

int64_t document_t::get_long(std::string_view json_pointer) const { return get_as<int64_t>(json_pointer); }

double document_t::get_double(std::string_view json_pointer) const { return get_as<double>(json_pointer); }

std::string document_t::get_string(std::string_view json_pointer) const { return std::string(get_as<std::string_view>(json_pointer)); }

void document_t::build_index(const simdjson::dom::element &value, const std::string& json_pointer) {
  json_pointer_index_[json_pointer] = value;
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

simdjson::error_code document_t::set_(std::string_view json_pointer, const simdjson::dom::element &value) {
  size_t pos = json_pointer.find_last_of('/');
  if (pos == std::string::npos) {
    return simdjson::INVALID_JSON_POINTER;
  }
  auto container_json_pointer = json_pointer.substr(0, pos);
  auto container = get(container_json_pointer);
  if (!container.is_object()) {
    return simdjson::INCORRECT_TYPE;
  }
  auto obj = container.get_object();
  auto key = json_pointer.substr(pos + 1);
  obj.value().insert(key, value);
  json_pointer_index_[json_pointer] = value;
  return simdjson::SUCCESS;
}

document_ptr document_from_json(const std::string &json) {
  simdjson::dom::document source;
  if (source.allocate(json.size()) != simdjson::SUCCESS) {
    return nullptr;
  }
  auto tree = boost::json::parse(json);
  simdjson::SIMDJSON_IMPLEMENTATION::stage2::tape_builder builder(source);
  walk_document(builder, tree);
  return new document_t(std::move(source));
}
} // namespace components::document