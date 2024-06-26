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
inline simdjson_result<uint8_t> element<K>::get_uint8() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_uint8())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::INT8: {
        return cast_from<K, int8_t, uint8_t>(tape);
      }
      case internal::tape_type::UINT16: {
        return cast_from<K, uint16_t, uint8_t>(tape);
      }
      case internal::tape_type::INT16: {
        return cast_from<K, int16_t, uint8_t>(tape);
      }
      case internal::tape_type::UINT32: {
        return cast_from<K, uint32_t, uint8_t>(tape);
      }
      case internal::tape_type::INT32: {
        return cast_from<K, int32_t, uint8_t>(tape);
      }
      case internal::tape_type::UINT64: {
        return cast_from<K, uint64_t, uint8_t>(tape);
      }
      case internal::tape_type::INT64: {
        return cast_from<K, int64_t, uint8_t>(tape);
      }
      case internal::tape_type::INT128: {
        return cast_from<K, __int128_t, uint8_t>(tape);
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  return tape.template next_tape_value<uint8_t>();
}
template<typename K>
inline simdjson_result<uint16_t> element<K>::get_uint16() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_uint16())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::UINT8: {
        return uint16_t(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::INT8: {
        return uint16_t(tape.template next_tape_value<int8_t>());
      }
      case internal::tape_type::INT16: {
        return cast_from<K, int16_t, uint16_t>(tape);
      }
      case internal::tape_type::UINT32: {
        return cast_from<K, uint32_t, uint16_t>(tape);
      }
      case internal::tape_type::INT32: {
        return cast_from<K, int32_t, uint16_t>(tape);
      }
      case internal::tape_type::UINT64: {
        return cast_from<K, uint64_t, uint16_t>(tape);
      }
      case internal::tape_type::INT64: {
        return cast_from<K, int64_t, uint16_t>(tape);
      }
      case internal::tape_type::INT128: {
        return cast_from<K, __int128_t, uint16_t>(tape);
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  return tape.template next_tape_value<uint16_t>();
}
template<typename K>
inline simdjson_result<uint32_t> element<K>::get_uint32() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_uint32())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::UINT8: {
        return uint32_t(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::INT8: {
        return uint32_t(tape.template next_tape_value<int8_t>());
      }
      case internal::tape_type::UINT16: {
        return uint32_t(tape.template next_tape_value<uint16_t>());
      }
      case internal::tape_type::INT16: {
        return cast_from<K, int16_t, uint32_t>(tape);
      }
      case internal::tape_type::INT32: {
        return cast_from<K, int32_t, uint32_t>(tape);
      }
      case internal::tape_type::UINT64: {
        return cast_from<K, uint64_t, uint32_t>(tape);
      }
      case internal::tape_type::INT64: {
        return cast_from<K, int64_t, uint32_t>(tape);
      }
      case internal::tape_type::INT128: {
        return cast_from<K, __int128_t, uint32_t>(tape);
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  return tape.template next_tape_value<uint32_t>();
}
template<typename K>
inline simdjson_result<uint64_t> element<K>::get_uint64() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_uint64())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::UINT8: {
        return uint64_t(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::INT8: {
        return uint64_t(tape.template next_tape_value<int8_t>());
      }
      case internal::tape_type::UINT16: {
        return uint64_t(tape.template next_tape_value<uint16_t>());
      }
      case internal::tape_type::UINT32: {
        return uint64_t(tape.template next_tape_value<uint32_t>());
      }
      case internal::tape_type::INT16: {
        return cast_from<K, int16_t, uint64_t>(tape);
      }
      case internal::tape_type::INT32: {
        return cast_from<K, int32_t, uint64_t>(tape);
      }
      case internal::tape_type::INT64: {
        return cast_from<K, int64_t, uint64_t>(tape);
      }
      case internal::tape_type::INT128: {
        return cast_from<K, __int128_t, uint64_t>(tape);
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  return tape.template next_tape_value<uint64_t>();
}
template<typename K>
inline simdjson_result<int8_t> element<K>::get_int8() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_int8())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::UINT8: {
        return int8_t(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::INT16: {
        return cast_from<K, int16_t, int8_t>(tape);
      }
      case internal::tape_type::UINT16: {
        return cast_from<K, uint16_t, int8_t>(tape);
      }
      case internal::tape_type::INT32: {
        return cast_from<K, int32_t, int8_t>(tape);
      }
      case internal::tape_type::UINT32: {
        return cast_from<K, uint32_t, int8_t>(tape);
      }
      case internal::tape_type::INT64: {
        return cast_from<K, int64_t, int8_t>(tape);
      }
      case internal::tape_type::UINT64: {
        return cast_from<K, uint64_t, int8_t>(tape);
      }
      case internal::tape_type::INT128: {
        return cast_from<K, __int128_t, int8_t>(tape);
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  return tape.template next_tape_value<int8_t>();
}
template<typename K>
inline simdjson_result<int16_t> element<K>::get_int16() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_int16())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::INT8: {
        return int16_t(tape.template next_tape_value<int8_t>());
      }
      case internal::tape_type::UINT8: {
        return int16_t(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::UINT16: {
        return cast_from<K, uint16_t, int16_t>(tape);
      }
      case internal::tape_type::INT32: {
        return cast_from<K, int32_t, int16_t>(tape);
      }
      case internal::tape_type::UINT32: {
        return cast_from<K, uint32_t, int16_t>(tape);
      }
      case internal::tape_type::INT64: {
        return cast_from<K, int64_t, int16_t>(tape);
      }
      case internal::tape_type::UINT64: {
        return cast_from<K, uint64_t, int16_t>(tape);
      }
      case internal::tape_type::INT128: {
        return cast_from<K, __int128_t, int16_t>(tape);
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  return tape.template next_tape_value<int16_t>();
}
template<typename K>
inline simdjson_result<int32_t> element<K>::get_int32() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_int32())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::INT8: {
        return int32_t(tape.template next_tape_value<int8_t>());
      }
      case internal::tape_type::UINT8: {
        return int32_t(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::INT16: {
        return int32_t(tape.template next_tape_value<int16_t>());
      }
      case internal::tape_type::UINT16: {
        return int32_t(tape.template next_tape_value<uint16_t>());
      }
      case internal::tape_type::UINT32: {
        return cast_from<K, uint32_t, int32_t>(tape);
      }
      case internal::tape_type::INT64: {
        return cast_from<K, int64_t, int32_t>(tape);
      }
      case internal::tape_type::UINT64: {
        return cast_from<K, uint64_t, int32_t>(tape);
      }
      case internal::tape_type::INT128: {
        return cast_from<K, __int128_t, int32_t>(tape);
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  return tape.template next_tape_value<int32_t>();
}
template<typename K>
inline simdjson_result<int64_t> element<K>::get_int64() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_int64())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::INT8: {
        return int64_t(tape.template next_tape_value<int8_t>());
      }
      case internal::tape_type::UINT8: {
        return int64_t(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::INT16: {
        return int32_t(tape.template next_tape_value<int16_t>());
      }
      case internal::tape_type::UINT16: {
        return int64_t(tape.template next_tape_value<uint16_t>());
      }
      case internal::tape_type::INT32: {
        return int64_t(tape.template next_tape_value<int32_t>());
      }
      case internal::tape_type::UINT32: {
        return int64_t(tape.template next_tape_value<uint32_t>());
      }
      case internal::tape_type::UINT64: {
        return cast_from<K, uint64_t, int64_t>(tape);
      }
      case internal::tape_type::INT128: {
        return cast_from<K, __int128_t, int64_t>(tape);
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  return tape.template next_tape_value<int64_t>();
}

template<typename K>
inline simdjson_result<__int128_t> element<K>::get_int128() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_int128())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::INT8: {
        return __int128_t(tape.template next_tape_value<int8_t>());
      }
      case internal::tape_type::UINT8: {
        return __int128_t(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::INT16: {
        return __int128_t(tape.template next_tape_value<int16_t>());
      }
      case internal::tape_type::UINT16: {
        return __int128_t(tape.template next_tape_value<uint16_t>());
      }
      case internal::tape_type::INT32: {
        return __int128_t(tape.template next_tape_value<int32_t>());
      }
      case internal::tape_type::UINT32: {
        return __int128_t(tape.template next_tape_value<uint32_t>());
      }
      case internal::tape_type::INT64: {
        return cast_from<K, int64_t, __int128_t>(tape);
      }
      case internal::tape_type::UINT64: {
        return cast_from<K, uint64_t, __int128_t>(tape);
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  return tape.template next_tape_value<__int128_t>();
}

template<typename K>
inline simdjson_result<float> element<K>::get_float() const noexcept {
  SIMDJSON_DEVELOPMENT_ASSERT(tape.usable()); // https://github.com/simdjson/simdjson/issues/1914
  if(simdjson_unlikely(!tape.is_float())) { // branch rarely taken
    switch (tape.tape_ref_type()) {
      case internal::tape_type::DOUBLE: {
        return float(tape.template next_tape_value<double>());
      }
      case internal::tape_type::UINT8: {
        return float(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::INT8: {
        return float(tape.template next_tape_value<int8_t>());
      }
      case internal::tape_type::UINT16: {
        return float(tape.template next_tape_value<uint16_t>());
      }
      case internal::tape_type::INT16: {
        return float(tape.template next_tape_value<int16_t>());
      }
      case internal::tape_type::UINT32: {
        return float(tape.template next_tape_value<uint32_t>());
      }
      case internal::tape_type::INT32: {
        return float(tape.template next_tape_value<int32_t>());
      }
      case internal::tape_type::UINT64: {
        return float(tape.template next_tape_value<uint64_t>());
      }
      case internal::tape_type::INT64: {
        return float(tape.template next_tape_value<int64_t>());
      }
      case internal::tape_type::INT128: {
        return float(tape.template next_tape_value<__int128_t>());
      }
      default:
        return INCORRECT_TYPE;
    }
  }
  // this is common:
  return tape.template next_tape_value<float>();
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
  if(simdjson_unlikely(!tape.is_double())) { // branch rarely taken    switch (tape.tape_ref_type()) {
    switch (tape.tape_ref_type()) {
      case internal::tape_type::FLOAT: {
        return double(tape.template next_tape_value<float>());
      }
      case internal::tape_type::UINT8: {
        return double(tape.template next_tape_value<uint8_t>());
      }
      case internal::tape_type::INT8: {
        return double(tape.template next_tape_value<int8_t>());
      }
      case internal::tape_type::UINT16: {
        return double(tape.template next_tape_value<uint16_t>());
      }
      case internal::tape_type::INT16: {
        return double(tape.template next_tape_value<int16_t>());
      }
      case internal::tape_type::UINT32: {
        return double(tape.template next_tape_value<uint32_t>());
      }
      case internal::tape_type::INT32: {
        return double(tape.template next_tape_value<int32_t>());
      }
      case internal::tape_type::UINT64: {
        return double(tape.template next_tape_value<uint64_t>());
      }
      case internal::tape_type::INT64: {
        return double(tape.template next_tape_value<int64_t>());
      }
      case internal::tape_type::INT128: {
        return double(tape.template next_tape_value<__int128_t>());
      }
      default:
        return INCORRECT_TYPE;
    }
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
template<typename K> inline bool element<K>::is_int32() const noexcept { return is<int32_t>(); }
template<typename K> inline bool element<K>::is_int64() const noexcept { return is<int64_t>(); }
template<typename K> inline bool element<K>::is_int128() const noexcept { return is<__int128_t>(); }
template<typename K> inline bool element<K>::is_uint32() const noexcept { return is<uint32_t>(); }
template<typename K> inline bool element<K>::is_uint64() const noexcept { return is<uint64_t>(); }
template<typename K> inline bool element<K>::is_float() const noexcept { return is<float>(); }
template<typename K> inline bool element<K>::is_double() const noexcept { return is<double>(); }
template<typename K> inline bool element<K>::is_bool() const noexcept { return is<bool>(); }
template<typename K> inline bool element<K>::is_number() const noexcept { return is_int64() || is_uint64() || is_double(); }

template<typename K>
inline bool element<K>::is_null() const noexcept {
  return tape.is_null_on_tape();
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
