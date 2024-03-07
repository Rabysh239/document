#ifndef SIMDJSON_ELEMENT_INL_H
#define SIMDJSON_ELEMENT_INL_H

#include "../../simdjson/dom/base.h"
#include "../../simdjson/dom/element.h"
#include "../../simdjson/dom/document.h"
#include "../../simdjson/dom/object.h"
#include "../../simdjson/internal/tape_type.h"

#include "../../simdjson/dom/array-inl.h"
#include "../../simdjson/dom/object-inl.h"
#include "../../simdjson/error-inl.h"

#include <ostream>
#include <limits>
#include <utility>

namespace simdjson {

//
// simdjson_result<dom::element> inline implementation
//
simdjson_inline simdjson_result<dom::element>::simdjson_result() noexcept
    : internal::simdjson_result_base<dom::element>() {}
simdjson_inline simdjson_result<dom::element>::simdjson_result(std::shared_ptr<dom::element> value) noexcept
    : internal::simdjson_result_base<dom::element>(std::move(value)) {}
simdjson_inline simdjson_result<dom::element>::simdjson_result(error_code error) noexcept
    : internal::simdjson_result_base<dom::element>(error) {}
inline simdjson_result<dom::element_type> simdjson_result<dom::element>::type() const noexcept {
  if (error()) { return error(); }
  return first->type();
}

template<typename T>
simdjson_inline bool simdjson_result<dom::element>::is() const noexcept {
  return !error() && first->is<T>();
}
template<typename T>
simdjson_inline simdjson_result<T> simdjson_result<dom::element>::get() const noexcept {
  if (error()) { return error(); }
  return first->get<T>();
}
template<typename T>
simdjson_warn_unused simdjson_inline error_code simdjson_result<dom::element>::get(T &value) const noexcept {
  if (error()) { return error(); }
  return first->get<T>(value);
}

simdjson_inline simdjson_result<dom::array> simdjson_result<dom::element>::get_array() const noexcept {
  if (error()) { return error(); }
  return first->get_array();
}
simdjson_inline simdjson_result<dom::object> simdjson_result<dom::element>::get_object() const noexcept {
  if (error()) { return error(); }
  return first->get_object();
}
simdjson_inline simdjson_result<const char *> simdjson_result<dom::element>::get_c_str() const noexcept {
  if (error()) { return error(); }
  return first->get_c_str();
}
simdjson_inline simdjson_result<size_t> simdjson_result<dom::element>::get_string_length() const noexcept {
  if (error()) { return error(); }
  return first->get_string_length();
}
simdjson_inline simdjson_result<std::string_view> simdjson_result<dom::element>::get_string() const noexcept {
  if (error()) { return error(); }
  return first->get_string();
}
simdjson_inline simdjson_result<int64_t> simdjson_result<dom::element>::get_int64() const noexcept {
  if (error()) { return error(); }
  return first->get_int64();
}
simdjson_inline simdjson_result<uint64_t> simdjson_result<dom::element>::get_uint64() const noexcept {
  if (error()) { return error(); }
  return first->get_uint64();
}
simdjson_inline simdjson_result<double> simdjson_result<dom::element>::get_double() const noexcept {
  if (error()) { return error(); }
  return first->get_double();
}
simdjson_inline simdjson_result<bool> simdjson_result<dom::element>::get_bool() const noexcept {
  if (error()) { return error(); }
  return first->get_bool();
}

simdjson_inline bool simdjson_result<dom::element>::is_array() const noexcept {
  return !error() && first->is_array();
}
simdjson_inline bool simdjson_result<dom::element>::is_object() const noexcept {
  return !error() && first->is_object();
}
simdjson_inline bool simdjson_result<dom::element>::is_string() const noexcept {
  return !error() && first->is_string();
}
simdjson_inline bool simdjson_result<dom::element>::is_int64() const noexcept {
  return !error() && first->is_int64();
}
simdjson_inline bool simdjson_result<dom::element>::is_uint64() const noexcept {
  return !error() && first->is_uint64();
}
simdjson_inline bool simdjson_result<dom::element>::is_double() const noexcept {
  return !error() && first->is_double();
}
simdjson_inline bool simdjson_result<dom::element>::is_number() const noexcept {
  return !error() && first->is_number();
}
simdjson_inline bool simdjson_result<dom::element>::is_bool() const noexcept {
  return !error() && first->is_bool();
}

simdjson_inline bool simdjson_result<dom::element>::is_null() const noexcept {
  return !error() && first->is_null();
}

simdjson_inline simdjson_result<dom::element> simdjson_result<dom::element>::operator[](std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first->operator[](key);
}
simdjson_inline simdjson_result<dom::element> simdjson_result<dom::element>::operator[](const char *key) const noexcept {
  if (error()) { return error(); }
  return first->operator[](key);
}
simdjson_inline simdjson_result<dom::element> simdjson_result<dom::element>::at_pointer(const std::string_view json_pointer) const noexcept {
  if (error()) { return error(); }
  return first->at_pointer(json_pointer);
}
//#ifndef SIMDJSON_DISABLE_DEPRECATED_API
//[[deprecated("For standard compliance, use at_pointer instead, and prefix your pointers with a slash '/', see RFC6901 ")]]
//simdjson_inline simdjson_result<dom::element> simdjson_result<dom::element>::at(const std::string_view json_pointer) const noexcept {
//SIMDJSON_PUSH_DISABLE_WARNINGS
//SIMDJSON_DISABLE_DEPRECATED_WARNING
//  if (error()) { return error(); }
//  return first.at(json_pointer);
//SIMDJSON_POP_DISABLE_WARNINGS
//}
//#endif // SIMDJSON_DISABLE_DEPRECATED_API
simdjson_inline simdjson_result<dom::element> simdjson_result<dom::element>::at(size_t index) const noexcept {
  if (error()) { return error(); }
  return first->at(index);
}
simdjson_inline simdjson_result<dom::element> simdjson_result<dom::element>::at_key(std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first->at_key(key);
}
//simdjson_inline simdjson_result<dom::element> simdjson_result<dom::element>::at_key_case_insensitive(std::string_view key) const noexcept {
//  if (error()) { return error(); }
//  return first->at_key_case_insensitive(key);
//}

#if SIMDJSON_EXCEPTIONS

simdjson_inline simdjson_result<dom::element>::operator bool() const noexcept(false) {
  return get<bool>();
}
simdjson_inline simdjson_result<dom::element>::operator const char *() const noexcept(false) {
  return get<const char *>();
}
simdjson_inline simdjson_result<dom::element>::operator std::string_view() const noexcept(false) {
  return get<std::string_view>();
}
simdjson_inline simdjson_result<dom::element>::operator uint64_t() const noexcept(false) {
  return get<uint64_t>();
}
simdjson_inline simdjson_result<dom::element>::operator int64_t() const noexcept(false) {
  return get<int64_t>();
}
simdjson_inline simdjson_result<dom::element>::operator double() const noexcept(false) {
  return get<double>();
}
simdjson_inline simdjson_result<dom::element>::operator dom::array() const noexcept(false) {
  return get<dom::array>();
}
//simdjson_inline simdjson_result<dom::element>::operator dom::object() const noexcept(false) {
//  return get<dom::object>();
//}


simdjson_inline dom::array::iterator simdjson_result<dom::element>::begin() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first->begin();
}
simdjson_inline dom::array::iterator simdjson_result<dom::element>::end() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first->end();
}

#endif // SIMDJSON_EXCEPTIONS

namespace dom {

//
// element inline implementation
//

inline simdjson_result<bool> element::get_bool() const noexcept {
    wait_until_usable();
    switch (type()) {
        case element_type::BOOL:
            return get_pure_bool();
        default:
            return INCORRECT_TYPE;
    }
}
inline simdjson_result<uint64_t> element::get_uint64() const noexcept {
    wait_until_usable();
    if(simdjson_unlikely(type() != element_type::UINT64)) { // branch rarely taken
        switch (type()) {
            case element_type::INT64: {
                int64_t result = get_pure_int64();
                if (result < 0) {
                    return NUMBER_OUT_OF_RANGE;
                }
                return uint64_t(result);
            }
            default:
                return INCORRECT_TYPE;
        }
    }
    return get_pure_uint64();
}
inline simdjson_result<int64_t> element::get_int64() const noexcept {
    wait_until_usable();
    if(simdjson_unlikely(type() != element_type::INT64)) { // branch rarely taken
        switch (type()) {
            case element_type::UINT64: {
                uint64_t result = get_pure_uint64();
                // Wrapping max in parens to handle Windows issue: https://stackoverflow.com/questions/11544073/how-do-i-deal-with-the-max-macro-in-windows-h-colliding-with-max-in-std
                if (result > uint64_t((std::numeric_limits<int64_t>::max)())) {
                    return NUMBER_OUT_OF_RANGE;
                }
                return static_cast<int64_t>(result);
            }
            default:
                return INCORRECT_TYPE;
        }
    }
    return get_pure_int64();
}
inline simdjson_result<double> element::get_double() const noexcept {
    wait_until_usable();
    // Performance considerations:
    // 1. Querying tape_ref_type() implies doing a shift, it is fast to just do a straight
    //   comparison.
    // 2. Using a switch-case relies on the compiler guessing what kind of code generation
    //    we want... But the compiler cannot know that we expect the type to be "double"
    //    most of the time.
    // We can expect get<double> to refer to a double type almost all the time.
    // It is important to craft the code accordingly so that the compiler can use this
    // information. (This could also be solved with profile-guided optimization.)
    if(simdjson_unlikely(type() != element_type::DOUBLE)) { // branch rarely taken
        switch (type()) {
            case element_type::UINT64:
                return double(get_pure_uint64());
            case element_type::INT64:
                return double(get_pure_int64());
            default:
                return INCORRECT_TYPE;
        }
    }
    // this is common:
    return get_pure_double();
}
inline simdjson_result<array> element::get_array() const noexcept {
    wait_until_usable();
    switch (type()) {
        case element_type::ARRAY:
            return get_pure_array();
        default:
            return INCORRECT_TYPE;
    }
}
inline simdjson_result<object> element::get_object() const noexcept {
    wait_until_usable();
    switch (type()) {
        case element_type::OBJECT:
            return get_pure_object();
        default:
            return INCORRECT_TYPE;
    }
}

inline simdjson_result<element> element::at_pointer(std::string_view json_pointer) const noexcept {
    wait_until_usable();
    switch (type()) {
        case element_type::OBJECT:
            return get_pure_object().at_pointer(json_pointer);
        case element_type::ARRAY:
            return get_pure_array().at_pointer(json_pointer);
        default: {
            if(!json_pointer.empty()) { // a non-empty string is invalid on an atom
                return INVALID_JSON_POINTER;
            }
            // an empty string means that we return the current node
            return get_instance();
        }
    }
}

template<typename T>
simdjson_warn_unused simdjson_inline error_code element::get(T &value) const noexcept {
    return get<T>().get(value);
}
// An element-specific version prevents recursion with simdjson_result::get<element>(value)
//template<>
//simdjson_warn_unused simdjson_inline error_code element_impl::get<element>(element &value) const noexcept {
//  value = element_impl(tape);
//  return SUCCESS;
//}
template<typename T>
inline void element::tie(T &value, error_code &error) && noexcept {
  error = get<T>(value);
}

template<typename T>
simdjson_inline bool element::is() const noexcept {
    auto result = get<T>();
    return !result.error();
}

template<> inline simdjson_result<array> element::get<array>() const noexcept { return get_array(); }
template<> inline simdjson_result<object> element::get<object>() const noexcept { return get_object(); }
template<> inline simdjson_result<const char *> element::get<const char *>() const noexcept { return get_c_str(); }
template<> inline simdjson_result<std::string_view> element::get<std::string_view>() const noexcept { return get_string(); }
template<> inline simdjson_result<int64_t> element::get<int64_t>() const noexcept { return get_int64(); }
template<> inline simdjson_result<uint64_t> element::get<uint64_t>() const noexcept { return get_uint64(); }
template<> inline simdjson_result<double> element::get<double>() const noexcept { return get_double(); }
template<> inline simdjson_result<bool> element::get<bool>() const noexcept { return get_bool(); }

inline bool element::is_array() const noexcept { return is<array>(); }
inline bool element::is_object() const noexcept { return is<object>(); }
inline bool element::is_string() const noexcept { return is<std::string_view>(); }
inline bool element::is_int64() const noexcept { return is<int64_t>(); }
inline bool element::is_uint64() const noexcept { return is<uint64_t>(); }
inline bool element::is_double() const noexcept { return is<double>(); }
inline bool element::is_bool() const noexcept { return is<bool>(); }
inline bool element::is_number() const noexcept { return is_int64() || is_uint64() || is_double(); }

inline bool element::is_null() const noexcept {
    return type() == element_type::NULL_VALUE;
}

#if SIMDJSON_EXCEPTIONS

inline element::operator bool() const noexcept(false) { return get<bool>(); }
inline element::operator const char*() const noexcept(false) { return get<const char *>(); }
inline element::operator std::string_view() const noexcept(false) { return get<std::string_view>(); }
inline element::operator uint64_t() const noexcept(false) { return get<uint64_t>(); }
inline element::operator int64_t() const noexcept(false) { return get<int64_t>(); }
inline element::operator double() const noexcept(false) { return get<double>(); }
inline element::operator array() const noexcept(false) { return get<array>(); }
//inline element::operator object() const noexcept(false) { return get<object>(); }

inline array::iterator element::begin() const noexcept(false) {
  return get<array>().begin();
}
inline array::iterator element::end() const noexcept(false) {
  return get<array>().end();
}

#endif // SIMDJSON_EXCEPTIONS

inline simdjson_result<element> element::operator[](std::string_view key) const noexcept {
    return at_key(key);
}
inline simdjson_result<element> element::operator[](const char *key) const noexcept {
    return at_key(key);
}

inline simdjson_result<element> element::at(size_t index) const noexcept {
    return get<array>().at(index);
}
inline simdjson_result<element> element::at_key(std::string_view key) const noexcept {
    return get<object>().at_key(key);
}
//inline simdjson_result<element> element::at_key_case_insensitive(std::string_view key) const noexcept {
//    return get<object>().at_key_case_insensitive(key);
//}

//#ifndef SIMDJSON_DISABLE_DEPRECATED_API
//[[deprecated("For standard compliance, use at_pointer instead, and prefix your pointers with a slash '/', see RFC6901 ")]]
//inline simdjson_result<element> element_impl::at(std::string_view json_pointer) const noexcept {
//  // version 0.4 of simdjson allowed non-compliant pointers
//  auto std_pointer = (json_pointer.empty() ? "" : "/") + std::string(json_pointer.begin(), json_pointer.end());
//  return at_pointer(std_pointer);
//}
//#endif // SIMDJSON_DISABLE_DEPRECATED_API

//inline bool element::operator<(const element &other) const noexcept {
//  return tape.json_index < other.tape.json_index;
//}
//inline bool element::operator==(const element &other) const noexcept {
//  return tape.json_index == other.tape.json_index;
//}

simdjson_inline element_impl::element_impl() noexcept : tape{} {}
simdjson_inline element_impl::element_impl(const internal::tape_ref &_tape) noexcept : tape{_tape} { }

inline element_type element_impl::type() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  auto tape_type = tape.tape_ref_type();
  return tape_type == internal::tape_type::FALSE_VALUE ? element_type::BOOL : static_cast<element_type>(tape_type);
}

inline simdjson_result<const char *> element_impl::get_c_str() const noexcept {
    SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
    switch (tape.tape_ref_type()) {
        case internal::tape_type::STRING: {
            return tape.get_c_str();
        }
        default:
            return INCORRECT_TYPE;
    }
}
inline simdjson_result<size_t> element_impl::get_string_length() const noexcept {
    SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
    switch (tape.tape_ref_type()) {
        case internal::tape_type::STRING: {
            return tape.get_string_length();
        }
        default:
            return INCORRECT_TYPE;
    }
}
inline simdjson_result<std::string_view> element_impl::get_string() const noexcept {
    SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
    switch (tape.tape_ref_type()) {
        case internal::tape_type::STRING:
            return tape.get_string_view();
        default:
            return INCORRECT_TYPE;
    }
}

inline void element_impl::wait_until_usable() const noexcept {
    SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
}

inline bool element_impl::get_pure_bool() const noexcept {
    return tape.is_true();
}
inline uint64_t element_impl::get_pure_uint64() const noexcept {
    return tape.next_tape_value<uint64_t>();
}
inline int64_t element_impl::get_pure_int64() const noexcept {
    return tape.next_tape_value<int64_t>();
}
inline double element_impl::get_pure_double() const noexcept {
    return tape.next_tape_value<double>();
}
inline simdjson_result<array> element_impl::get_pure_array() const noexcept {
    return array(tape);
}
inline simdjson_result<object> element_impl::get_pure_object() const noexcept {
    return {std::make_shared<object_impl>(object_impl(tape))};
}

inline std::shared_ptr<element> element_impl::get_instance() const noexcept {
    return std::make_shared<element_impl>(element_impl(*this));
}

inline bool element_impl::dump_raw_tape(std::ostream &out) const noexcept {
    SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
    return tape.doc->dump_raw_tape(out);
}

element_d::element_d() noexcept : value_type(element_type::NULL_VALUE) {}
element_d::element_d(std::shared_ptr<array> _value) noexcept : value(std::move(_value)), value_type(element_type::ARRAY) {}
element_d::element_d(std::shared_ptr<object> _value) noexcept : value(std::move(_value)), value_type(element_type::OBJECT) {}
element_d::element_d(std::string _value) noexcept : value(std::move(_value)), value_type(element_type::STRING) {}
element_d::element_d(int64_t _value) noexcept : value(_value), value_type(element_type::INT64) {}
element_d::element_d(uint64_t _value)  noexcept: value(_value), value_type(element_type::UINT64) {}
element_d::element_d(double _value) noexcept : value(_value), value_type(element_type::DOUBLE) {}
element_d::element_d(bool _value) noexcept : value(_value), value_type(element_type::BOOL) {}

element_type element_d::type() const noexcept {
    return value_type;
}

inline simdjson_result<const char *> element_d::get_c_str() const noexcept {
    switch (value_type) {
        case element_type::STRING:
            return std::get<std::string>(value).c_str();
        default:
            return INCORRECT_TYPE;
    }
}
inline simdjson_result<size_t> element_d::get_string_length() const noexcept {
    switch (value_type) {
        case element_type::STRING:
            return std::get<std::string>(value).size();
        default:
            return INCORRECT_TYPE;
    }
}
inline simdjson_result<std::string_view> element_d::get_string() const noexcept {
    switch (value_type) {
        case element_type::STRING:
            return std::string_view(std::get<std::string>(value));
        default:
            return INCORRECT_TYPE;
    }
}

inline void element_d::wait_until_usable() const noexcept {}

inline bool element_d::get_pure_bool() const noexcept {
    return bool(std::get<bool>(value));
}
inline uint64_t element_d::get_pure_uint64() const noexcept {
    return uint64_t(std::get<uint64_t>(value));
}
inline int64_t element_d::get_pure_int64() const noexcept {
    return int64_t(std::get<int64_t>(value));
}
inline double element_d::get_pure_double() const noexcept {
    return double(std::get<double>(value));;
}
inline simdjson_result<array> element_d::get_pure_array() const noexcept {
    return std::get<std::shared_ptr<array>>(value);
}
inline simdjson_result<object> element_d::get_pure_object() const noexcept {
    return std::get<std::shared_ptr<object>>(value);
}

inline std::shared_ptr<element> element_d::get_instance() const noexcept {
    return std::make_shared<element_d>(this);
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
