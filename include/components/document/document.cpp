#include "document.hpp"
#include <boost/json/src.hpp>
#include <utility>
#include "../../../src/generic/stage2/json_iterator.h"

namespace components::document {

std::size_t document_t::count(std::string_view json_pointer) const {
  const auto opt_value = prefix_ind_.get(json_pointer);
  if (!opt_value.has_value()) {
    return 0;
  }
  auto &value = opt_value.value();
  if (value.is_array()) {
    return value.get_array().size();
  }
  if (value.is_object()) {
    return value.get_object().size();
  }
  return 0;
}

bool document_t::is_exists(std::string_view json_pointer) const { return prefix_ind_.get(json_pointer).has_value(); }

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

document_t::ptr document_t::get_array(std::string_view json_pointer) const {
  const auto opt_value = prefix_ind_.get(json_pointer);
  if (opt_value.has_value() && opt_value.value().is_array()) {
    return new document_t(src_ptr_, ind_ptr_, json_pointer);
  }
  return nullptr; //temporarily
}

document_t::document_t(document_t::source_ptr source) : src_ptr_(std::move(source)),
                                                        ind_ptr_(new element_index(src_ptr_->root())),
                                                        prefix_ind_(prefix_index("", ind_ptr_)),
                                                        builder_(*src_ptr_) {}

document_t::document_t(document_t::source_ptr source, index_ptr ind_ptr, std::string_view prefix) : src_ptr_(std::move(source)),
                                                                                                    ind_ptr_(std::move(ind_ptr)),
                                                                                                    prefix_ind_(prefix_index(prefix,ind_ptr_)),
                                                                                                    builder_(*src_ptr_) {}

void document_t::set_(std::string_view json_pointer, const simdjson::dom::element &value) {
  size_t pos = json_pointer.find_last_of('/');
  if (pos == std::string::npos) {
    return;
  }
  auto container_json_pointer = json_pointer.substr(0, pos);
  auto opt_container = prefix_ind_.get(container_json_pointer);
  if (!opt_container.has_value()) {
    return;
  }
  auto &container = opt_container.value();
  if (container.is_object()) {
    prefix_ind_.update_or_insert(json_pointer, value);
  } else if (container.is_array()) {
    auto key = std::string(json_pointer.substr(pos + 1));
    auto index = size_t(std::atol(key.c_str()));
    auto correct_index = std::min(index, container.get_array().size());
    prefix_ind_.update_or_insert(std::string(container_json_pointer) + "/" + std::to_string(correct_index), value);
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