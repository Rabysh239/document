#ifndef SIMDJSON_OBJECT_INL_H
#define SIMDJSON_OBJECT_INL_H

#include "../../simdjson/dom/base.h"
#include "../../simdjson/dom/object.h"
#include "../../simdjson/dom/document.h"

#include "../../simdjson/dom/element-inl.h"
#include "../../simdjson/error-inl.h"

#include <cstring>

namespace simdjson {

//
// simdjson_result<dom::object> inline implementation
//
template<typename T>
simdjson_inline simdjson_result<dom::object<T>>::simdjson_result() noexcept
    : internal::simdjson_result_base<dom::object<T>>() {}
template<typename T>
simdjson_inline simdjson_result<dom::object<T>>::simdjson_result(dom::object<T> value) noexcept
    : internal::simdjson_result_base<dom::object<T>>(std::forward<dom::object<T>>(value)) {}
template<typename T>
simdjson_inline simdjson_result<dom::object<T>>::simdjson_result(error_code error) noexcept
    : internal::simdjson_result_base<dom::object<T>>(error) {}

template<typename T>
inline simdjson_result<dom::element<T>> simdjson_result<dom::object<T>>::operator[](std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first[key];
}
template<typename T>
inline simdjson_result<dom::element<T>> simdjson_result<dom::object<T>>::operator[](const char *key) const noexcept {
  if (error()) { return error(); }
  return first[key];
}
template<typename T>
inline simdjson_result<dom::element<T>> simdjson_result<dom::object<T>>::at_pointer(std::string_view json_pointer) const noexcept {
  if (error()) { return error(); }
  return first.at_pointer(json_pointer);
}
template<typename T>
inline simdjson_result<dom::element<T>> simdjson_result<dom::object<T>>::at_key(std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first.at_key(key);
}
template<typename T>
inline simdjson_result<dom::element<T>> simdjson_result<dom::object<T>>::at_key_case_insensitive(std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first.at_key_case_insensitive(key);
}

#if SIMDJSON_EXCEPTIONS

template<typename T>
inline typename dom::object<T>::iterator simdjson_result<dom::object<T>>::begin() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.begin();
}
template<typename T>
inline typename dom::object<T>::iterator simdjson_result<dom::object<T>>::end() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.end();
}
template<typename T>
inline size_t simdjson_result<dom::object<T>>::size() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.size();
}

#endif // SIMDJSON_EXCEPTIONS

namespace dom {

//
// object inline implementation
//
template<typename T>
simdjson_inline object<T>::object() noexcept : tape{} {}
template<typename T>
simdjson_inline object<T>::object(const internal::tape_ref<T> &_tape) noexcept : tape{_tape} { }
template<typename T>
inline typename object<T>::iterator object<T>::begin() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  return internal::tape_ref(tape.doc, tape.json_index + 1);
}
template<typename T>
inline typename object<T>::iterator object<T>::end() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  return internal::tape_ref(tape.doc, tape.after_element() - 1);
}
template<typename T>
inline size_t object<T>::size() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  return tape.scope_count();
}

template<typename T>
inline simdjson_result<element<T>> object<T>::operator[](std::string_view key) const noexcept {
  return at_key(key);
}
template<typename T>
inline simdjson_result<element<T>> object<T>::operator[](const char *key) const noexcept {
  return at_key(key);
}
template<typename T>
inline simdjson_result<element<T>> object<T>::at_pointer(std::string_view json_pointer) const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(json_pointer.empty()) { // an empty string means that we return the current node
      return element(this->tape); // copy the current node
  } else if(json_pointer[0] != '/') { // otherwise there is an error
      return INVALID_JSON_POINTER;
  }
  json_pointer = json_pointer.substr(1);
  size_t slash = json_pointer.find('/');
  std::string_view key = json_pointer.substr(0, slash);
  // Grab the child with the given key
  simdjson_result<element<T>> child;

  // If there is an escape character in the key, unescape it and then get the child.
  size_t escape = key.find('~');
  if (escape != std::string_view::npos) {
    // Unescape the key
    std::string unescaped(key);
    do {
      switch (unescaped[escape+1]) {
        case '0':
          unescaped.replace(escape, 2, "~");
          break;
        case '1':
          unescaped.replace(escape, 2, "/");
          break;
        default:
          return INVALID_JSON_POINTER; // "Unexpected ~ escape character in JSON pointer");
      }
      escape = unescaped.find('~', escape+1);
    } while (escape != std::string::npos);
    child = at_key(unescaped);
  } else {
    child = at_key(key);
  }
  if(child.error()) {
    return child; // we do not continue if there was an error
  }
  // If there is a /, we have to recurse and look up more of the path
  if (slash != std::string_view::npos) {
    child = child.at_pointer(json_pointer.substr(slash));
  }
  return child;
}

template<typename T>
inline simdjson_result<element<T>> object<T>::at_key(std::string_view key) const noexcept {
  iterator end_field = end();
  for (iterator field = begin(); field != end_field; ++field) {
    if (field.key_equals(key)) {
      return field.value();
    }
  }
  return NO_SUCH_FIELD;
}
// In case you wonder why we need this, please see
// https://github.com/simdjson/simdjson/issues/323
// People do seek keys in a case-insensitive manner.
template<typename T>
inline simdjson_result<element<T>> object<T>::at_key_case_insensitive(std::string_view key) const noexcept {
  iterator end_field = end();
  for (iterator field = begin(); field != end_field; ++field) {
    if (field.key_equals_case_insensitive(key)) {
      return field.value();
    }
  }
  return NO_SUCH_FIELD;
}

//
// object<T>::iterator inline implementation
//
template<typename T>
simdjson_inline object<T>::iterator::iterator(const internal::tape_ref<T> &_tape) noexcept : tape{_tape} { }
template<typename T>
inline const key_value_pair<T> object<T>::iterator::operator*() const noexcept {
  return key_value_pair(key(), value());
}
template<typename T>
inline bool object<T>::iterator::operator!=(const object<T>::iterator& other) const noexcept {
  return tape.json_index != other.tape.json_index;
}
template<typename T>
inline bool object<T>::iterator::operator==(const object<T>::iterator& other) const noexcept {
  return tape.json_index == other.tape.json_index;
}
template<typename T>
inline bool object<T>::iterator::operator<(const object<T>::iterator& other) const noexcept {
  return tape.json_index < other.tape.json_index;
}
template<typename T>
inline bool object<T>::iterator::operator<=(const object<T>::iterator& other) const noexcept {
  return tape.json_index <= other.tape.json_index;
}
template<typename T>
inline bool object<T>::iterator::operator>=(const object<T>::iterator& other) const noexcept {
  return tape.json_index >= other.tape.json_index;
}
template<typename T>
inline bool object<T>::iterator::operator>(const object<T>::iterator& other) const noexcept {
  return tape.json_index > other.tape.json_index;
}
template<typename T>
inline typename object<T>::iterator& object<T>::iterator::operator++() noexcept {
  tape.json_index++;
  tape.json_index = tape.after_element();
  return *this;
}
template<typename T>
inline typename object<T>::iterator object<T>::iterator::operator++(int) noexcept {
  object<T>::iterator out = *this;
  ++*this;
  return out;
}
template<typename T>
inline std::string_view object<T>::iterator::key() const noexcept {
  return tape.get_string_view();
}
template<typename T>
inline uint32_t object<T>::iterator::key_length() const noexcept {
  return tape.get_string_length();
}
template<typename T>
inline const char* object<T>::iterator::key_c_str() const noexcept {
  return reinterpret_cast<const char *>(&tape.doc->get_string_buf(size_t(tape.tape_value()) + sizeof(uint32_t)));
}
template<typename T>
inline element<T> object<T>::iterator::value() const noexcept {
  return element(internal::tape_ref(tape.doc, tape.json_index + 1));
}

/**
 * Design notes:
 * Instead of constructing a string_view and then comparing it with a
 * user-provided strings, it is probably more performant to have dedicated
 * functions taking as a parameter the string we want to compare against
 * and return true when they are equal. That avoids the creation of a temporary
 * std::string_view. Though it is possible for the compiler to avoid entirely
 * any overhead due to string_view, relying too much on compiler magic is
 * problematic: compiler magic sometimes fail, and then what do you do?
 * Also, enticing users to rely on high-performance function is probably better
 * on the long run.
 */

template<typename T>
inline bool object<T>::iterator::key_equals(std::string_view o) const noexcept {
  // We use the fact that the key length can be computed quickly
  // without access to the string buffer.
  const uint32_t len = key_length();
  if(o.size() == len) {
    // We avoid construction of a temporary string_view instance.
    return (memcmp(o.data(), key_c_str(), len) == 0);
  }
  return false;
}

template<typename T>
inline bool object<T>::iterator::key_equals_case_insensitive(std::string_view o) const noexcept {
  // We use the fact that the key length can be computed quickly
  // without access to the string buffer.
  const uint32_t len = key_length();
  if(o.size() == len) {
      // See For case-insensitive string comparisons, avoid char-by-char functions
      // https://lemire.me/blog/2020/04/30/for-case-insensitive-string-comparisons-avoid-char-by-char-functions/
      // Note that it might be worth rolling our own strncasecmp function, with vectorization.
      return (simdjson_strncasecmp(o.data(), key_c_str(), len) == 0);
  }
  return false;
}
//
// key_value_pair inline implementation
//
template<typename T>
inline key_value_pair<T>::key_value_pair(std::string_view _key, element<T> _value) noexcept :
  key(_key), value(_value) {}

} // namespace dom

} // namespace simdjson
//
//#if defined(__cpp_lib_ranges)
//static_assert(std::ranges::view<simdjson::dom::object>);
//static_assert(std::ranges::sized_range<simdjson::dom::object>);
//#if SIMDJSON_EXCEPTIONS
//static_assert(std::ranges::view<simdjson::simdjson_result<simdjson::dom::object>>);
//static_assert(std::ranges::sized_range<simdjson::simdjson_result<simdjson::dom::object>>);
//#endif // SIMDJSON_EXCEPTIONS
//#endif // defined(__cpp_lib_ranges)

#endif // SIMDJSON_OBJECT_INL_H
