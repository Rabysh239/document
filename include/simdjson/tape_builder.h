#ifndef SIMDJSON_TAPE_BUILDER_H

#ifndef SIMDJSON_CONDITIONAL_INCLUDE
#define SIMDJSON_TAPE_BUILDER_H
#include <utility>

#include <simdjson/base.h>
#include <simdjson/dom/document.h>
#include <simdjson/dom/tape_writer.h>

#endif // SIMDJSON_CONDITIONAL_INCLUDE


namespace simdjson {

template<typename K>
struct tape_builder {
  using allocator_type = std::pmr::memory_resource;

  simdjson_inline tape_builder() noexcept;
  simdjson_inline tape_builder(allocator_type *allocator, dom::immutable_document &doc) noexcept;
  simdjson_inline tape_builder(allocator_type *allocator, dom::mutable_document &doc) noexcept;

  simdjson_inline ~tape_builder();

  simdjson_inline tape_builder(tape_builder &&) noexcept;

  simdjson_inline tape_builder(const tape_builder &) = delete;

  simdjson_inline tape_builder &operator=(tape_builder &&) noexcept;

  simdjson_inline tape_builder &operator=(const tape_builder &) = delete;

  simdjson_inline void build(std::string_view value) noexcept;

  simdjson_inline void build(int8_t value) noexcept;
  simdjson_inline void build(int16_t value) noexcept;
  simdjson_inline void build(int32_t value) noexcept;
  /** Write a signed 64-bit value to  */
  simdjson_inline void build(int64_t value) noexcept;

  simdjson_inline void build(__int128_t value) noexcept;

  simdjson_inline void build(uint8_t value) noexcept;
  simdjson_inline void build(uint16_t value) noexcept;
  simdjson_inline void build(uint32_t value) noexcept;
  /** Write an unsigned 64-bit value to  */
  simdjson_inline void build(uint64_t value) noexcept;

  simdjson_inline void build(float value) noexcept;
  /** Write a double value to  */
  simdjson_inline void build(double value) noexcept;
  simdjson_inline void build(bool value) noexcept;
  simdjson_inline void build(nullptr_t) noexcept;
  simdjson_inline void visit_null_atom() noexcept;
private:
  allocator_type *allocator_;
  dom::tape_writer<K> *tape_;

  /**
   * Append a tape entry (an 8-bit type,and 56 bits worth of value).
   */
  simdjson_inline void append(uint64_t val, internal::tape_type t) noexcept;

private:
  /**
   * Append both the tape entry, and a supplementary value following it. Used for types that need
   * all 64 bits, such as double and uint64_t.
   */
  template<typename T>
  simdjson_inline void append2(uint64_t val, T val2, internal::tape_type t) noexcept;

  template<typename T>
  simdjson_inline void append3(T val2, internal::tape_type t) noexcept;
};

template<typename K>
tape_builder<K>::tape_builder() noexcept
        : tape_(nullptr),
          allocator_(nullptr) {}

template<typename K>
tape_builder<K>::~tape_builder() {
  mr_delete(allocator_, static_cast<K *>(tape_));
}

template<typename K>
tape_builder<K>::tape_builder(tape_builder &&other) noexcept
        : allocator_(other.allocator_),
          tape_(other.tape_) {
  other.allocator_ = nullptr;
  other.tape_ = nullptr;
}

template<typename K>
tape_builder<K> &tape_builder<K>::operator=(tape_builder &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  allocator_ = other.allocator_;
  tape_ = other.tape_;
  other.allocator_ = nullptr;
  other.tape_ = nullptr;
  return *this;
}
// struct tape_builder

template<typename K>
simdjson_inline tape_builder<K>::tape_builder(allocator_type *allocator, dom::immutable_document &doc) noexcept
        : allocator_(allocator),
          tape_(new(allocator_->allocate(sizeof(simdjson::dom::tape_writer_to_immutable)))
                        simdjson::dom::tape_writer_to_immutable(doc)) {}

template<typename K>
simdjson_inline tape_builder<K>::tape_builder(allocator_type *allocator, dom::mutable_document &doc) noexcept
        : allocator_(allocator),
          tape_(new(allocator_->allocate(sizeof(simdjson::dom::tape_writer_to_mutable)))
                        simdjson::dom::tape_writer_to_mutable(doc)) {}

template<typename K>
simdjson_inline void tape_builder<K>::build(std::string_view value) noexcept {
  // we advance the point, accounting for the fact that we have a NULL termination
  append(tape_->next_string_buf_index(), internal::tape_type::STRING);
  tape_->append_string(value);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(int8_t value) noexcept {
  append(value, internal::tape_type::INT8);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(int16_t value) noexcept {
  append(value, internal::tape_type::INT16);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(int32_t value) noexcept {
  append(value, internal::tape_type::INT32);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(int64_t value) noexcept {
  append2(0, value, internal::tape_type::INT64);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(__int128_t value) noexcept {
  append3(value, internal::tape_type::INT128);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(uint8_t value) noexcept {
  append(value, internal::tape_type::UINT8);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(uint16_t value) noexcept {
  append(value, internal::tape_type::UINT16);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(uint32_t value) noexcept {
  append(value, internal::tape_type::UINT32);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(uint64_t value) noexcept {
  append(0, internal::tape_type::UINT64);
  tape_->append(value);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(float value) noexcept {
  uint64_t tape_data;
  std::memcpy(&tape_data, &value, sizeof(value));
  append(tape_data, internal::tape_type::FLOAT);
}

/** Write a double value to  */
template<typename K>
simdjson_inline void tape_builder<K>::build(double value) noexcept {
  append2(0, value, internal::tape_type::DOUBLE);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(bool value) noexcept {
  append(0, value ? internal::tape_type::TRUE_VALUE : internal::tape_type::FALSE_VALUE);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(nullptr_t) noexcept {
  visit_null_atom();
}

template<typename K>
simdjson_inline void tape_builder<K>::visit_null_atom() noexcept {
  append(0, internal::tape_type::NULL_VALUE);
}

template<typename K>
simdjson_inline void tape_builder<K>::append(uint64_t val, internal::tape_type t) noexcept {
  tape_->append(val | ((uint64_t(char(t))) << 56));
}

template<typename K>
template<typename T>
simdjson_inline void tape_builder<K>::append2(uint64_t val, T val2, internal::tape_type t) noexcept {
  append(val, t);
  static_assert(sizeof(val2) == sizeof(uint64_t), "Type is not 64 bits!");
  tape_->copy(&val2);
}

template<typename K>
template<typename T>
simdjson_inline void tape_builder<K>::append3(T val2, internal::tape_type t) noexcept {
  append(0, t);
  static_assert(sizeof(val2) == 2 * sizeof(uint64_t), "Type is not 128 bits!");
  auto data = reinterpret_cast<uint64_t *>(&val2);
  tape_->copy(data);
  tape_->copy(data + 1);
}

} // namespace simdjson

#endif // SIMDJSON_TAPE_BUILDER_H