#ifndef SIMDJSON_ELEMENT_INL_H
#define SIMDJSON_ELEMENT_INL_H

#include <simdjson/dom/base.h>
#include <simdjson/dom/element.h>
#include <simdjson/dom/document.h>
#include <simdjson/dom/object.h>
#include <simdjson/internal/tape_type.h>

#include <simdjson/dom/object-inl.h>
#include <simdjson/error-inl.h>

#include <ostream>
#include <limits>

namespace simdjson {

//
// simdjson_result<dom::element> inline implementation
//
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::simdjson_result() noexcept
    : internal::simdjson_result_base<dom::element<K>>() {}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::simdjson_result(dom::element<K> &&value) noexcept
    : internal::simdjson_result_base<dom::element<K>>(std::forward<dom::element<K>>(value)) {}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::simdjson_result(error_code error) noexcept
    : internal::simdjson_result_base<dom::element<K>>(error) {}
template<typename K>
inline simdjson_result<dom::element_type> simdjson_result<dom::element<K>>::type() const noexcept {
  if (error()) { return error(); }
  return first.type();
}

template<typename K>
template<typename T>
simdjson_inline bool simdjson_result<dom::element<K>>::is() const noexcept {
  return !error() && first.template is<T>();
}
template<typename K>
template<typename T>
simdjson_inline simdjson_result<T> simdjson_result<dom::element<K>>::get() const noexcept {
  if (error()) { return error(); }
  return first.template get<T>();
}
template<typename K>
template<typename T>
simdjson_warn_unused simdjson_inline error_code simdjson_result<dom::element<K>>::get(T &value) const noexcept {
  if (error()) { return error(); }
  return first.template get<T>(value);
}

template<typename K>
simdjson_inline simdjson_result<dom::array<K>> simdjson_result<dom::element<K>>::get_array() const noexcept {
  if (error()) { return error(); }
  return first.get_array();
}
template<typename K>
simdjson_inline simdjson_result<dom::object<K>> simdjson_result<dom::element<K>>::get_object() const noexcept {
  if (error()) { return error(); }
  return first.get_object();
}
template<typename K>
simdjson_inline simdjson_result<const char *> simdjson_result<dom::element<K>>::get_c_str() const noexcept {
  if (error()) { return error(); }
  return first.get_c_str();
}
template<typename K>
simdjson_inline simdjson_result<size_t> simdjson_result<dom::element<K>>::get_string_length() const noexcept {
  if (error()) { return error(); }
  return first.get_string_length();
}
template<typename K>
simdjson_inline simdjson_result<std::string_view> simdjson_result<dom::element<K>>::get_string() const noexcept {
  if (error()) { return error(); }
  return first.get_string();
}
template<typename K>
simdjson_inline simdjson_result<int64_t> simdjson_result<dom::element<K>>::get_int64() const noexcept {
  if (error()) { return error(); }
  return first.get_int64();
}
template<typename K>
simdjson_inline simdjson_result<uint64_t> simdjson_result<dom::element<K>>::get_uint64() const noexcept {
  if (error()) { return error(); }
  return first.get_uint64();
}
template<typename K>
simdjson_inline simdjson_result<double> simdjson_result<dom::element<K>>::get_double() const noexcept {
  if (error()) { return error(); }
  return first.get_double();
}
template<typename K>
simdjson_inline simdjson_result<bool> simdjson_result<dom::element<K>>::get_bool() const noexcept {
  if (error()) { return error(); }
  return first.get_bool();
}

template<typename K>
simdjson_inline bool simdjson_result<dom::element<K>>::is_array() const noexcept {
  return !error() && first.is_array();
}
template<typename K>
simdjson_inline bool simdjson_result<dom::element<K>>::is_object() const noexcept {
  return !error() && first.is_object();
}
template<typename K>
simdjson_inline bool simdjson_result<dom::element<K>>::is_string() const noexcept {
  return !error() && first.is_string();
}
template<typename K>
simdjson_inline bool simdjson_result<dom::element<K>>::is_int64() const noexcept {
  return !error() && first.is_int64();
}
template<typename K>
simdjson_inline bool simdjson_result<dom::element<K>>::is_uint64() const noexcept {
  return !error() && first.is_uint64();
}
template<typename K>
simdjson_inline bool simdjson_result<dom::element<K>>::is_double() const noexcept {
  return !error() && first.is_double();
}
template<typename K>
simdjson_inline bool simdjson_result<dom::element<K>>::is_number() const noexcept {
  return !error() && first.is_number();
}
template<typename K>
simdjson_inline bool simdjson_result<dom::element<K>>::is_bool() const noexcept {
  return !error() && first.is_bool();
}

template<typename K>
simdjson_inline bool simdjson_result<dom::element<K>>::is_null() const noexcept {
  return !error() && first.is_null();
}

template<typename K>
simdjson_inline simdjson_result<dom::element<K>> simdjson_result<dom::element<K>>::operator[](std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first[key];
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>> simdjson_result<dom::element<K>>::operator[](const char *key) const noexcept {
  if (error()) { return error(); }
  return first[key];
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>> simdjson_result<dom::element<K>>::at_pointer(const std::string_view json_pointer) const noexcept {
  if (error()) { return error(); }
  return first.at_pointer(json_pointer);
}
#ifndef SIMDJSON_DISABLE_DEPRECATED_API
template<typename K>
[[deprecated("For standard compliance, use at_pointer instead, and prefix your pointers with a slash '/', see RFC6901 ")]]
simdjson_inline simdjson_result<dom::element<K>> simdjson_result<dom::element<K>>::at(const std::string_view json_pointer) const noexcept {
SIMDJSON_PUSH_DISABLE_WARNINGS
SIMDJSON_DISABLE_DEPRECATED_WARNING
  if (error()) { return error(); }
  return first.at(json_pointer);
SIMDJSON_POP_DISABLE_WARNINGS
}
#endif // SIMDJSON_DISABLE_DEPRECATED_API
template<typename K>
simdjson_inline simdjson_result<dom::element<K>> simdjson_result<dom::element<K>>::at(size_t index) const noexcept {
  if (error()) { return error(); }
  return first.at(index);
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>> simdjson_result<dom::element<K>>::at_key(std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first.at_key(key);
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>> simdjson_result<dom::element<K>>::at_key_case_insensitive(std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first.at_key_case_insensitive(key);
}

#if SIMDJSON_EXCEPTIONS

template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::operator bool() const noexcept(false) {
  return get<bool>();
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::operator const char *() const noexcept(false) {
  return get<const char *>();
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::operator std::string_view() const noexcept(false) {
  return get<std::string_view>();
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::operator uint64_t() const noexcept(false) {
  return get<uint64_t>();
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::operator int64_t() const noexcept(false) {
  return get<int64_t>();
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::operator double() const noexcept(false) {
  return get<double>();
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::operator dom::array<K>() const noexcept(false) {
  return get<dom::array>();
}
template<typename K>
simdjson_inline simdjson_result<dom::element<K>>::operator dom::object<K>() const noexcept(false) {
  return get<dom::object>();
}

template<typename K>
simdjson_inline typename dom::array<K>::iterator simdjson_result<dom::element<K>>::begin() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.begin();
}
template<typename K>
simdjson_inline typename dom::array<K>::iterator simdjson_result<dom::element<K>>::end() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.end();
}

#endif // SIMDJSON_EXCEPTIONS

namespace dom {

//
// element inline implementation
//
template<typename K>
simdjson_inline element<K>::element() noexcept : tape{} {}
template<typename K>
simdjson_inline element<K>::element(const internal::tape_ref<K> &_tape) noexcept : tape{_tape} { }

template<typename K>
inline element_type element<K>::type() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  auto tape_type = tape.tape_ref_type();
  return tape_type == internal::tape_type::FALSE_VALUE ? element_type::BOOL : static_cast<element_type>(tape_type);
}

template<typename K>
inline simdjson_result<bool> element<K>::get_bool() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(tape.is_true()) {
    return true;
  } else if(tape.is_false()) {
    return false;
  }
  return INCORRECT_TYPE;
}
template<typename K>
inline simdjson_result<const char *> element<K>::get_c_str() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  switch (tape.tape_ref_type()) {
    case internal::tape_type::STRING: {
      return tape.get_c_str();
    }
    default:
      return INCORRECT_TYPE;
  }
}
template<typename K>
inline simdjson_result<size_t> element<K>::get_string_length() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  switch (tape.tape_ref_type()) {
    case internal::tape_type::STRING: {
      return tape.get_string_length();
    }
    default:
      return INCORRECT_TYPE;
  }
}
template<typename K>
inline simdjson_result<std::string_view> element<K>::get_string() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  switch (tape.tape_ref_type()) {
    case internal::tape_type::STRING:
      return tape.get_string_view();
    default:
      return INCORRECT_TYPE;
  }
}
template<typename K>
inline simdjson_result<uint64_t> element<K>::get_uint64() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_uint64())) { // branch rarely taken
    if(tape.is_int64()) {
      int64_t result = tape.template next_tape_value<int64_t>();
      if (result < 0) {
        return NUMBER_OUT_OF_RANGE;
      }
      return uint64_t(result);
    }
    return INCORRECT_TYPE;
  }
  return tape.template next_tape_value<int64_t>();
}
template<typename K>
inline simdjson_result<int64_t> element<K>::get_int64() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_int64())) { // branch rarely taken
    if(tape.is_uint64()) {
      uint64_t result = tape.template next_tape_value<uint64_t>();
      // Wrapping max in parens to handle Windows issue: https://stackoverflow.com/questions/11544073/how-do-i-deal-with-the-max-macro-in-windows-h-colliding-with-max-in-std
      if (result > uint64_t((std::numeric_limits<int64_t>::max)())) {
        return NUMBER_OUT_OF_RANGE;
      }
      return static_cast<int64_t>(result);
    }
    return INCORRECT_TYPE;
  }
  return tape.template next_tape_value<int64_t>();
}
template<typename K>
inline simdjson_result<double> element<K>::get_double() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  // Performance considerations:
  // 1. Querying tape_ref_type() implies doing a shift, it is fast to just do a straight
  //   comparison.
  // 2. Using a switch-case relies on the compiler guessing what kind of code generation
  //    we want... But the compiler cannot know that we expect the type to be "double"
  //    most of the time.
  // We can expect get<double> to refer to a double type almost all the time.
  // It is important to craft the code accordingly so that the compiler can use this
  // information. (This could also be solved with profile-guided optimization.)
  if(simdjson_unlikely(!tape.is_double())) { // branch rarely taken
    if(tape.is_uint64()) {
      return double(tape.template next_tape_value<uint64_t>());
    } else if(tape.is_int64()) {
      return double(tape.template next_tape_value<int64_t>());
    }
    return INCORRECT_TYPE;
  }
  // this is common:
  return tape.template next_tape_value<double>();
}
template<typename K>
inline simdjson_result<array<K>> element<K>::get_array() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  switch (tape.tape_ref_type()) {
    case internal::tape_type::START_ARRAY:
      return array(tape);
    default:
      return INCORRECT_TYPE;
  }
}
template<typename K>
inline simdjson_result<object<K>> element<K>::get_object() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  switch (tape.tape_ref_type()) {
    case internal::tape_type::START_OBJECT:
      return object(tape);
    default:
      return INCORRECT_TYPE;
  }
}

template<typename K>
template<typename T>
simdjson_warn_unused simdjson_inline error_code element<K>::get(T &value) const noexcept {
  return get<T>().get(value);
}

template<typename K>
template<typename T>
inline void element<K>::tie(T &value, error_code &error) && noexcept {
  error = get<T>(value);
}

template<typename K>
template<typename T>
simdjson_inline bool element<K>::is() const noexcept {
  auto result = get<T>();
  return !result.error();
}

template<typename K> inline bool element<K>::is_array() const noexcept { return is<array<K>>(); }
template<typename K> inline bool element<K>::is_object() const noexcept { return is<object<K>>(); }
template<typename K> inline bool element<K>::is_string() const noexcept { return is<std::string_view>(); }
template<typename K> inline bool element<K>::is_int64() const noexcept { return is<int64_t>(); }
template<typename K> inline bool element<K>::is_uint64() const noexcept { return is<uint64_t>(); }
template<typename K> inline bool element<K>::is_double() const noexcept { return is<double>(); }
template<typename K> inline bool element<K>::is_bool() const noexcept { return is<bool>(); }
template<typename K> inline bool element<K>::is_number() const noexcept { return is_int64() || is_uint64() || is_double(); }

template<typename K>
inline bool element<K>::is_null() const noexcept {
  return tape.is_null_on_tape();
}

#if SIMDJSON_EXCEPTIONS

template<typename K> inline element<K>::operator bool() const noexcept(false) { return get<bool>(); }
template<typename K> inline element<K>::operator const char*() const noexcept(false) { return get<const char *>(); }
template<typename K> inline element<K>::operator std::string_view() const noexcept(false) { return get<std::string_view>(); }
template<typename K> inline element<K>::operator uint64_t() const noexcept(false) { return get<uint64_t>(); }
template<typename K> inline element<K>::operator int64_t() const noexcept(false) { return get<int64_t>(); }
template<typename K> inline element<K>::operator double() const noexcept(false) { return get<double>(); }
template<typename K> inline element<K>::operator array<K>() const noexcept(false) { return get<array>(); }
template<typename K> inline element<K>::operator object<K>() const noexcept(false) { return get<object>(); }

template<typename K>
inline typename array<K>::iterator element<K>::begin() const noexcept(false) {
  return get<array>().begin();
}
template<typename K>
inline typename array<K>::iterator element<K>::end() const noexcept(false) {
  return get<array>().end();
}

#endif // SIMDJSON_EXCEPTIONS

template<typename K>
inline simdjson_result<element<K>> element<K>::operator[](std::string_view key) const noexcept {
  return at_key(key);
}
template<typename K>
inline simdjson_result<element<K>> element<K>::operator[](const char *key) const noexcept {
  return at_key(key);
}

template<typename K>
inline simdjson_result<element<K>> element<K>::at_pointer(std::string_view json_pointer) const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  switch (tape.tape_ref_type()) {
    case internal::tape_type::START_OBJECT:
      return object(tape).at_pointer(json_pointer);
    case internal::tape_type::START_ARRAY:
      return array(tape).at_pointer(json_pointer);
    default: {
      if(!json_pointer.empty()) { // a non-empty string is invalid on an atom
        return INVALID_JSON_POINTER;
      }
      // an empty string means that we return the current node
      dom::element copy(*this);
      return simdjson_result<element>(std::move(copy));
    }
  }
}
#ifndef SIMDJSON_DISABLE_DEPRECATED_API
template<typename K>
[[deprecated("For standard compliance, use at_pointer instead, and prefix your pointers with a slash '/', see RFC6901 ")]]
inline simdjson_result<element<K>> element<K>::at(std::string_view json_pointer) const noexcept {
  // version 0.4 of simdjson allowed non-compliant pointers
  auto std_pointer = (json_pointer.empty() ? "" : "/") + std::string(json_pointer.begin(), json_pointer.end());
  return at_pointer(std_pointer);
}
#endif // SIMDJSON_DISABLE_DEPRECATED_API

template<typename K>
inline simdjson_result<element<K>> element<K>::at(size_t index) const noexcept {
  return get<array>().at(index);
}
template<typename K>
inline simdjson_result<element<K>> element<K>::at_key(std::string_view key) const noexcept {
  return get<object>().at_key(key);
}
template<typename K>
inline simdjson_result<element<K>> element<K>::at_key_case_insensitive(std::string_view key) const noexcept {
  return get<object>().at_key_case_insensitive(key);
}
template<typename K>
inline bool element<K>::operator<(const element &other) const noexcept {
  return tape.json_index < other.tape.json_index;
}
template<typename K>
inline bool element<K>::operator==(const element &other) const noexcept {
  return tape.json_index == other.tape.json_index;
}

template<typename K>
inline bool element<K>::dump_raw_tape(std::ostream &out) const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  return tape.doc->dump_raw_tape(out);
}


inline std::ostream& operator<<(std::ostream& out, element_type type) {
  switch (type) {
    case element_type::ARRAY:
      return out << "array";
    case element_type::OBJECT:
      return out << "object";
    case element_type::INT64:
      return out << "int64_t";
    case element_type::UINT64:
      return out << "uint64_t";
    case element_type::DOUBLE:
      return out << "double";
    case element_type::STRING:
      return out << "string";
    case element_type::BOOL:
      return out << "bool";
    case element_type::NULL_VALUE:
      return out << "null";
    default:
      return out << "unexpected content!!!"; // abort() usage is forbidden in the library
  }
}

} // namespace dom

} // namespace simdjson

#endif // SIMDJSON_ELEMENT_INL_H
