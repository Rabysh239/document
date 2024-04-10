#ifndef SIMDJSON_TAPE_REF_INL_H
#define SIMDJSON_TAPE_REF_INL_H

#include <simdjson/dom/document.h>
#include <simdjson/internal/tape_ref.h>
#include <simdjson/internal/tape_type.h>

#include <cstring>

namespace simdjson {
namespace internal {

constexpr const uint64_t JSON_VALUE_MASK = 0x00FFFFFFFFFFFFFF;

//
// tape_ref inline implementation
//
template<typename K>
simdjson_inline tape_ref<K>::tape_ref() noexcept : doc{nullptr}, json_index{0} {}
template<typename K>
simdjson_inline tape_ref<K>::tape_ref(const dom::document<K> *_doc, size_t _json_index) noexcept : doc{_doc}, json_index{_json_index} {}

template<typename K>
simdjson_inline bool tape_ref<K>::usable() const noexcept {
  return doc != nullptr; // when the document pointer is null, this tape_ref is uninitialized (should not be accessed).
}
// Some value types have a specific on-tape word value. It can be faster
// to check the type by doing a word-to-word comparison instead of extracting the
// most significant 8 bits.

template<typename K>
simdjson_inline bool tape_ref<K>::is_float() const noexcept {
  constexpr auto tape_float = uint8_t(tape_type::FLOAT);
  return reinterpret_cast<const uint8_t *>(&doc->get_tape(json_index))[7] == tape_float;
}
template<typename K>
simdjson_inline bool tape_ref<K>::is_double() const noexcept {
  constexpr uint64_t tape_double = uint64_t(tape_type::DOUBLE)<<56;
  return doc->get_tape(json_index) == tape_double;
}
template<typename K>
simdjson_inline bool tape_ref<K>::is_int32() const noexcept {
  constexpr auto tape_int32 = uint8_t(tape_type::INT32);
  return reinterpret_cast<const uint8_t *>(&doc->get_tape(json_index))[7] == tape_int32;
}
template<typename K>
simdjson_inline bool tape_ref<K>::is_int64() const noexcept {
  constexpr uint64_t tape_int64 = uint64_t(tape_type::INT64)<<56;
  return doc->get_tape(json_index) == tape_int64;
}
template<typename K>
simdjson_inline bool tape_ref<K>::is_uint64() const noexcept {
  constexpr uint64_t tape_uint64 = uint64_t(tape_type::UINT64)<<56;
  return doc->get_tape(json_index) == tape_uint64;
}
template<typename K>
simdjson_inline bool tape_ref<K>::is_false() const noexcept {
  constexpr uint64_t tape_false = uint64_t(tape_type::FALSE_VALUE)<<56;
  return doc->get_tape(json_index) == tape_false;
}
template<typename K>
simdjson_inline bool tape_ref<K>::is_true() const noexcept {
  constexpr uint64_t tape_true = uint64_t(tape_type::TRUE_VALUE)<<56;
  return doc->get_tape(json_index) == tape_true;
}
template<typename K>
simdjson_inline bool tape_ref<K>::is_null_on_tape() const noexcept {
  constexpr uint64_t tape_null = uint64_t(tape_type::NULL_VALUE)<<56;
  return doc->get_tape(json_index) == tape_null;
}

template<typename K>
simdjson_inline tape_type tape_ref<K>::tape_ref_type() const noexcept {
  return static_cast<tape_type>(doc->get_tape(json_index) >> 56);
}
template<typename K>
simdjson_inline uint64_t internal::tape_ref<K>::tape_value() const noexcept {
  return doc->get_tape(json_index) & internal::JSON_VALUE_MASK;
}

template<typename K>
simdjson_inline uint32_t internal::tape_ref<K>::get_string_length() const noexcept {
  size_t string_buf_index = size_t(tape_value());
  uint32_t len;
  std::memcpy(&len, &doc->get_string_buf(string_buf_index), sizeof(len));
  return len;
}

template<typename K>
simdjson_inline const char * internal::tape_ref<K>::get_c_str() const noexcept {
  size_t string_buf_index = size_t(tape_value());
  return reinterpret_cast<const char *>(&doc->get_string_buf(string_buf_index + sizeof(uint32_t)));
}

template<typename K>
inline std::string_view internal::tape_ref<K>::get_string_view() const noexcept {
  return std::string_view(
      get_c_str(),
      get_string_length()
  );
}

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_TAPE_REF_INL_H
