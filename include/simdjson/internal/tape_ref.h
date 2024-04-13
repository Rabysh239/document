#ifndef SIMDJSON_INTERNAL_TAPE_REF_H
#define SIMDJSON_INTERNAL_TAPE_REF_H

#include <simdjson/base.h>
#include <cstring>

namespace simdjson {
namespace dom {
template<typename T>
class document;
} // namespace dom

namespace internal {

/**
 * A reference to an element on the tape. Internal only.
 */
template<typename K>
class tape_ref {
public:
  simdjson_inline tape_ref() noexcept;
  simdjson_inline tape_ref(const dom::document<K> *doc, size_t json_index) noexcept;
  simdjson_inline tape_type tape_ref_type() const noexcept;
  simdjson_inline uint64_t tape_value() const noexcept;
  simdjson_inline bool is_float() const noexcept;
  simdjson_inline bool is_double() const noexcept;
  simdjson_inline bool is_int32() const noexcept;
  simdjson_inline bool is_int64() const noexcept;
  simdjson_inline bool is_int128() const noexcept;
  simdjson_inline bool is_uint32() const noexcept;
  simdjson_inline bool is_uint64() const noexcept;
  simdjson_inline bool is_false() const noexcept;
  simdjson_inline bool is_true() const noexcept;
  simdjson_inline bool is_null_on_tape() const noexcept;// different name to avoid clash with is_null.

  template<typename T, typename std::enable_if<(sizeof(T) < sizeof(uint64_t)), uint8_t>::type = 0>
  simdjson_inline T next_tape_value() const noexcept {
    static_assert(sizeof(T) == sizeof(uint32_t), "next_tape_value() template parameter must be 32, 64 or 128-bit");
    T x;
    std::memcpy(&x,&doc->get_tape(json_index),sizeof(T));
    return x;
  }

  template<typename T, typename std::enable_if<(sizeof(T) >= sizeof(uint64_t)), uint8_t>::type = 1>
  simdjson_inline T next_tape_value() const noexcept {
    static_assert(sizeof(T) == sizeof(uint64_t) || sizeof(T) == 2 * sizeof(uint64_t), "next_tape_value() template parameter must be 32, 64 or 128-bit");
    // Though the following is tempting...
    //  return *reinterpret_cast<const T*>(&doc_->tape[json_index + 1]);
    // It is not generally safe. It is safer, and often faster to rely
    // on memcpy. Yes, it is uglier, but it is also encapsulated.
    T x;
    std::memcpy(&x,&doc->get_tape(json_index + 1),sizeof(T));
    return x;
  }

  simdjson_inline uint32_t get_string_length() const noexcept;
  simdjson_inline const char * get_c_str() const noexcept;
  inline std::string_view get_string_view() const noexcept;
  simdjson_inline bool usable() const noexcept;

  /** The document this element references. */
  const dom::document<K> *doc;

  /** The index of this element on `doc_.tape[]` */
  size_t json_index;
};

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_INTERNAL_TAPE_REF_H
