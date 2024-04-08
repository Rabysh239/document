#ifndef SIMDJSON_ELEMENT_INL_H
#define SIMDJSON_ELEMENT_INL_H

#include <simdjson/dom/base.h>
#include <simdjson/dom/element.h>
#include <simdjson/dom/document.h>
#include <simdjson/internal/tape_type.h>

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

#endif // SIMDJSON_EXCEPTIONS

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
