#ifndef SIMDJSON_OBJECT_INL_H
#define SIMDJSON_OBJECT_INL_H

#include "../../simdjson/dom/base.h"
#include "../../simdjson/dom/object.h"
#include "../../simdjson/dom/document.h"

#include "../../simdjson/dom/element-inl.h"
#include "../../simdjson/error-inl.h"

#include <cstring>
#include <utility>

namespace simdjson {

//
// simdjson_result<dom::object> inline implementation
//
simdjson_inline simdjson_result<dom::object>::simdjson_result() noexcept
    : internal::simdjson_result_base<dom::object>() {}
simdjson_inline simdjson_result<dom::object>::simdjson_result(std::shared_ptr<dom::object> value) noexcept
    : internal::simdjson_result_base<dom::object>(std::move(value)) {}
simdjson_inline simdjson_result<dom::object>::simdjson_result(error_code error) noexcept
    : internal::simdjson_result_base<dom::object>(error) {}

inline simdjson_result<dom::element> simdjson_result<dom::object>::operator[](std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first->operator[](key);
}
inline simdjson_result<dom::element> simdjson_result<dom::object>::operator[](const char *key) const noexcept {
  if (error()) { return error(); }
  return first->operator[](key);
}
inline simdjson_result<dom::element> simdjson_result<dom::object>::at_pointer(std::string_view json_pointer) const noexcept {
  if (error()) { return error(); }
  return first->at_pointer(json_pointer);
}
inline simdjson_result<dom::element> simdjson_result<dom::object>::at_key(std::string_view key) const noexcept {
  if (error()) { return error(); }
  return first->at_key(key);
}
//inline simdjson_result<dom::element> simdjson_result<dom::object>::at_key_case_insensitive(std::string_view key) const noexcept {
//  if (error()) { return error(); }
//  return first->at_key_case_insensitive(key);
//}

#if SIMDJSON_EXCEPTIONS

inline std::unique_ptr<dom::object::iterator> simdjson_result<dom::object>::begin() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return std::move(first->begin());
}
inline std::unique_ptr<dom::object::iterator> simdjson_result<dom::object>::end() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return std::move(first->end());
}

inline void simdjson_result<dom::object>::insert(std::string key, const std::shared_ptr<dom::element> &value) noexcept {
    first->insert(std::move(key), value);
}
//inline size_t simdjson_result<dom::object>::size() const noexcept(false) {
//  if (error()) { throw simdjson_error(error()); }
//  return first->size();
//}

#endif // SIMDJSON_EXCEPTIONS

namespace dom {

//
// object inline implementation
//
inline simdjson_result<element> object::operator[](std::string_view key) const noexcept {
    return at_key(key);
}
inline simdjson_result<element> object::operator[](const char *key) const noexcept {
    return at_key(key);
}

inline simdjson_result<element> object::at_pointer(std::string_view json_pointer) const noexcept {
    wait_until_usable();
    if(json_pointer.empty()) { // an empty string means that we return the current node
        return get_wrapped_instance();
    } else if(json_pointer[0] != '/') { // otherwise there is an error
        return INVALID_JSON_POINTER;
    }
    json_pointer = json_pointer.substr(1);
    size_t slash = json_pointer.find('/');
    std::string_view key = json_pointer.substr(0, slash);
    // Grab the child with the given key
    simdjson_result<element> child;

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
//inline size_t object::size() const noexcept {
//  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
//  return tape.scope_count();
//}

//// In case you wonder why we need this, please see
//// https://github.com/simdjson/simdjson/issues/323
//// People do seek keys in a case-insensitive manner.
//inline simdjson_result<element> object::at_key_case_insensitive(std::string_view key) const noexcept {
//    std::unique_ptr<iterator> end_field = end();
//    for (auto field = begin(); *field != *end_field; ++*field) {
//        if (field->key_equals_case_insensitive(key)) {
//            return field->value();
//        }
//    }
//    return NO_SUCH_FIELD;
//}

inline std::unique_ptr<object::iterator> object_d::begin() const noexcept {
    return std::make_unique<object_d::iterator_d>(data.begin());
}
inline std::unique_ptr<object::iterator> object_d::end() const noexcept {
    return std::make_unique<object_d::iterator_d>(data.end());
}

void object_d::wait_until_usable() const noexcept {}

std::shared_ptr<element> object_d::get_wrapped_instance() const noexcept {
    return std::make_shared<element_d>(std::make_shared<object_d>(*this));
}

void object_d::insert(std::string key, const std::shared_ptr<element>& value) noexcept {
    data.insert({std::move(key), value});
}

simdjson_result<element> object_d::at_key(std::string_view key) const noexcept {
    auto key_str = std::string(key);
    if (data.find(key_str) == data.end()) {
        return NO_SUCH_FIELD;
    }
    return data.at(std::string(key));
}

simdjson_inline object_impl::object_impl() noexcept : tape{} {}
simdjson_inline object_impl::object_impl(const internal::tape_ref &_tape) noexcept : tape{_tape} {
    auto &addition_map = tape.doc->dynamic_addition;
    if (addition_map.find(tape.json_index) == addition_map.end()) {
        addition_map.insert(std::make_pair(tape.json_index, std::make_shared<object_d>()));
    }
    dynamic_data = addition_map[tape.json_index];
}
inline std::unique_ptr<object::iterator> object_impl::begin() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
    return std::make_unique<object_impl::iterator_impl>(
            object_impl::iterator_impl(internal::tape_ref(tape.doc, tape.json_index + 1), tape.after_element() - 1,
                                       dynamic_data->begin()));
}
inline std::unique_ptr<object::iterator> object_impl::end() const noexcept {
    return dynamic_data->end();
}

inline simdjson_result<element> object_impl::at_key(std::string_view key) const noexcept {
    auto value_from_dynamic_data = dynamic_data->at_key(std::string(key));
    if (value_from_dynamic_data.error() != NO_SUCH_FIELD) {
        return value_from_dynamic_data;
    }
    std::unique_ptr<iterator> end_field = dynamic_data->begin();
    for (auto field = begin(); *field != *end_field; ++*field) {
        if (field->key_equals(key)) {
            return field->value();
        }
    }
    return NO_SUCH_FIELD;
}

void object_impl::insert(std::string key, const std::shared_ptr<element> &value) noexcept {
    dynamic_data->insert(std::move(key), value);
}

void object_impl::wait_until_usable() const noexcept {
    SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
}

std::shared_ptr<element> object_impl::get_wrapped_instance() const noexcept {
    return {std::make_shared<element_impl>(element_impl(this->tape))}; // copy the current node
}

//
// object::iterator inline implementation
//
inline const key_value_pair object::iterator::operator*() const noexcept {
    return key_value_pair(key(), value());
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

inline bool object::iterator::key_equals(std::string_view o) const noexcept {
    // We use the fact that the key length can be computed quickly
    // without access to the string buffer.
    const uint32_t len = key_length();
    if(o.size() == len) {
        // We avoid construction of a temporary string_view instance.
        return (memcmp(o.data(), key_c_str(), len) == 0);
    }
    return false;
}

inline bool object::iterator::key_equals_case_insensitive(std::string_view o) const noexcept {
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

simdjson_inline object_d::iterator_d::iterator_d(object_d::iterator_type _iter) noexcept : iter(_iter) {}

inline object::iterator &object_d::iterator_d::operator++() noexcept {
    ++iter;
    return *this;
}

inline bool object_d::iterator_d::operator!=(const object::iterator &other) const noexcept {
    const auto* other_d = dynamic_cast<const object_d::iterator_d*>(&other);
    return other_d == nullptr || iter != other_d->iter;
}
inline std::string_view object_d::iterator_d::key() const noexcept {
    return iter->first;
}
inline uint32_t object_d::iterator_d::key_length() const noexcept {
    return iter->first.size();
}
inline const char* object_d::iterator_d::key_c_str() const noexcept {
    return iter->first.c_str();
}
inline std::shared_ptr<element> object_d::iterator_d::value() const noexcept {
    return iter->second;
}

simdjson_inline object_impl::iterator_impl::iterator_impl(const internal::tape_ref &_tape) noexcept
    : tape{_tape}, is_const(false) {}
simdjson_inline object_impl::iterator_impl::iterator_impl(const internal::tape_ref &_tape, size_t _end_json_index, std::unique_ptr<iterator> _dynamic_data_iter) noexcept
    : tape{_tape}, end_json_index(_end_json_index), dynamic_data_iter(std::move(_dynamic_data_iter)), is_const(true) {}
inline bool object_impl::iterator_impl::operator!=(const object::iterator& other) const noexcept {
    if (!is_const) {
        return *dynamic_data_iter != other;
    }
    const auto* other_impl = dynamic_cast<const object_impl::iterator_impl*>(&other);
    return other_impl == nullptr || tape.json_index != other_impl->tape.json_index;
}
//inline bool object_impl::iterator_impl::operator==(const object::iterator& other) const noexcept {
//    return tape.json_index == other.tape.json_index;
//}
//inline bool object_impl::iterator_impl::operator<(const object::iterator& other) const noexcept {
//  return tape.json_index < other.tape.json_index;
//}
//inline bool object_impl::iterator_impl::operator<=(const object::iterator& other) const noexcept {
//  return tape.json_index <= other.tape.json_index;
//}
//inline bool object_impl::iterator_impl::operator>=(const object::iterator& other) const noexcept {
//  return tape.json_index >= other.tape.json_index;
//}
//inline bool object_impl::iterator_impl::operator>(const object::iterator& other) const noexcept {
//  return tape.json_index > other.tape.json_index;
//}
inline object::iterator& object_impl::iterator_impl::operator++() noexcept {
    if (is_const) {
        tape.json_index++;
        tape.json_index = tape.after_element();
        if (tape.json_index == end_json_index) {
            is_const = false;
        }
    } else {
        ++*dynamic_data_iter;
    }
  return *this;
}
//inline object::iterator object::iterator::operator++(int) noexcept {
//  object::iterator out = *this;
//  ++*this;
//  return out;
//}
inline std::string_view object_impl::iterator_impl::key() const noexcept {
  return is_const ? tape.get_string_view() : dynamic_data_iter->key();
}
inline uint32_t object_impl::iterator_impl::key_length() const noexcept {
  return is_const ? tape.get_string_length() : dynamic_data_iter->key_length();
}
inline const char* object_impl::iterator_impl::key_c_str() const noexcept {
    return is_const ? reinterpret_cast<const char *>(&tape.doc->string_buf[size_t(tape.tape_value()) + sizeof(uint32_t)])
                    : dynamic_data_iter->key_c_str();
}
inline std::shared_ptr<element> object_impl::iterator_impl::value() const noexcept {
    return is_const ? std::make_shared<element_impl>(element_impl(internal::tape_ref(tape.doc, tape.json_index + 1)))
                    : dynamic_data_iter->value();
}

//
// key_value_pair inline implementation
//
inline key_value_pair::key_value_pair(std::string_view _key, std::shared_ptr<element> _value) noexcept :
  key(_key), value(std::move(_value)) {}

} // namespace dom

} // namespace simdjson

#if defined(__cpp_lib_ranges)
static_assert(std::ranges::view<simdjson::dom::object>);
static_assert(std::ranges::sized_range<simdjson::dom::object>);
#if SIMDJSON_EXCEPTIONS
static_assert(std::ranges::view<simdjson::simdjson_result<simdjson::dom::object>>);
static_assert(std::ranges::sized_range<simdjson::simdjson_result<simdjson::dom::object>>);
#endif // SIMDJSON_EXCEPTIONS
#endif // defined(__cpp_lib_ranges)

#endif // SIMDJSON_OBJECT_INL_H
