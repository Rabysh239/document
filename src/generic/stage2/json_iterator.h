#ifndef SIMDJSON_SRC_GENERIC_STAGE2_JSON_ITERATOR_H

#ifndef SIMDJSON_CONDITIONAL_INCLUDE
#define SIMDJSON_SRC_GENERIC_STAGE2_JSON_ITERATOR_H
#include "../../generic/stage2/base.h"
#include <boost/json/src.hpp>
#endif // SIMDJSON_CONDITIONAL_INCLUDE

namespace simdjson {
namespace SIMDJSON_IMPLEMENTATION {

namespace stage2 {

template<typename V>
simdjson_inline void walk_document(V &visitor, const boost::json::value &value) noexcept {
  visitor.start_container();
  walk(visitor, value);
  visitor.visit_document_end();
}

template<typename V>
simdjson_inline void walk(V &visitor, const boost::json::value &value) noexcept {
  if (value.is_object()) {
    auto &obj = value.get_object();
    if (obj.empty()) {
      visitor.visit_empty_object();
    } else {
      auto start_index = visitor.start_container();
      for (auto const &[key, val]: obj) {
        visitor.build(key);
        walk(visitor, val);
      }
      visitor.visit_object_end(start_index, obj.size());
    }
  } else if (value.is_array()) {
    auto &arr = value.get_array();
    if (arr.empty()) {
      visitor.visit_empty_array();
    } else {
      auto start_index = visitor.start_container();
      for (auto const &e: arr) {
        walk(visitor, e);
      }
      visitor.visit_array_end(start_index, arr.size());
    }
  } else if (value.is_primitive()) {
    visit_primitive(visitor, value);
  }
}

template<typename V>
simdjson_inline void visit_primitive(V &visitor, const boost::json::value &value) noexcept {
  // Use the fact that most scalars are going to be either strings or numbers.
  if (value.is_string()) {
    auto &str = value.get_string();
    visitor.build(str.c_str(), str.size());
  } else if (value.is_number()) {
    if (value.is_double()) {
      visitor.build(value.get_double());
    } else if (value.is_int64()) {
      visitor.build(value.get_int64());
    } else if (value.is_uint64()) {
      visitor.build(value.get_uint64());
    }
  } else
    // true, false, null are uncommon.
  if (value.is_bool()) {
    visitor.build(value.get_bool());
  } else if (value.is_null()) {
    visitor.visit_null_atom();
  }
}

} // namespace stage2
} // namespace SIMDJSON_IMPLEMENTATION
} // namespace simdjson

#endif // SIMDJSON_SRC_GENERIC_STAGE2_JSON_ITERATOR_H