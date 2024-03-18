#ifndef SIMDJSON_SRC_GENERIC_STAGE2_TAPE_BUILDER_H

#ifndef SIMDJSON_CONDITIONAL_INCLUDE
#define SIMDJSON_SRC_GENERIC_STAGE2_TAPE_BUILDER_H
#include <utility>

#include "../../generic/stage2/base.h"
#include "../../../include/simdjson/dom/document.h"
#include "../../../include/simdjson/dom/tape_writer.h"

#endif // SIMDJSON_CONDITIONAL_INCLUDE


namespace simdjson {
namespace SIMDJSON_IMPLEMENTATION {
namespace stage2 {

template<typename K>
struct tape_builder {
  simdjson_inline explicit tape_builder(dom::immutable_document &doc) noexcept;
  simdjson_inline explicit tape_builder(dom::mutable_document &doc) noexcept;

  simdjson_inline uint32_t start_container() noexcept;

  simdjson_inline void visit_document_end() noexcept;

  simdjson_inline void visit_array_end(uint32_t start_tape_index, uint32_t count) noexcept;
  simdjson_inline void visit_empty_array() noexcept;

  simdjson_inline void visit_object_end(uint32_t start_tape_index, uint32_t count) noexcept;
  simdjson_inline void visit_empty_object() noexcept;

  simdjson_inline void build(char const* c_str, size_t size) noexcept;
  simdjson_inline void build(std::string_view value) noexcept;

  /** Write a signed 64-bit value to  */
  simdjson_inline void build(int64_t value) noexcept;

  /** Write an unsigned 64-bit value to  */
  simdjson_inline void build(uint64_t value) noexcept;

  /** Write a double value to  */
  simdjson_inline void build(double value) noexcept;
  simdjson_inline void build(bool value) noexcept;
  simdjson_inline void visit_null_atom() noexcept;
private:
  std::unique_ptr<dom::tape_writer<K>> tape_;

  simdjson_inline void end_container(uint32_t start_tape_index, uint32_t count, internal::tape_type start, internal::tape_type end) noexcept;
  simdjson_inline void empty_container(internal::tape_type start, internal::tape_type end) noexcept;

  /**
   * Append a tape entry (an 8-bit type,and 56 bits worth of value).
   */
  simdjson_inline void append(uint64_t val, internal::tape_type t) noexcept;
  /**
   * Write a value to a known location on 
   *
   * Used to go back and write out the start of a container after the container ends.
   */
  simdjson_inline void write(uint32_t tape_index, uint64_t val, internal::tape_type t) noexcept;

private:
  /**
   * Append both the tape entry, and a supplementary value following it. Used for types that need
   * all 64 bits, such as double and uint64_t.
   */
  template<typename T>
  simdjson_inline void append2(uint64_t val, T val2, internal::tape_type t) noexcept;
}; // struct tape_builder

template<typename K>
simdjson_inline void tape_builder<K>::visit_empty_object() noexcept {
  empty_container(internal::tape_type::START_OBJECT, internal::tape_type::END_OBJECT);
}
template<typename K>
simdjson_inline void tape_builder<K>::visit_empty_array() noexcept {
  empty_container(internal::tape_type::START_ARRAY, internal::tape_type::END_ARRAY);
}

template<typename K>
simdjson_inline void tape_builder<K>::visit_object_end(uint32_t start_tape_index, uint32_t count) noexcept {
  end_container(start_tape_index, count, internal::tape_type::START_OBJECT, internal::tape_type::END_OBJECT);
}
template<typename K>
simdjson_inline void tape_builder<K>::visit_array_end(uint32_t start_tape_index, uint32_t count) noexcept {
  end_container(start_tape_index, count, internal::tape_type::START_ARRAY, internal::tape_type::END_ARRAY);
}
template<typename K>
simdjson_inline void tape_builder<K>::visit_document_end() noexcept {
  constexpr uint32_t start_tape_index = 0;
  append(start_tape_index, internal::tape_type::ROOT);
  write(start_tape_index, tape_->next_tape_index(), internal::tape_type::ROOT);
}

template<typename K>
simdjson_inline tape_builder<K>::tape_builder(dom::immutable_document &doc) noexcept
: tape_(new simdjson::dom::tape_writer_to_immutable(doc)) {}

template<typename K>
simdjson_inline tape_builder<K>::tape_builder(dom::mutable_document &doc) noexcept
        : tape_(new simdjson::dom::tape_writer_to_mutable(doc)) {}

template<typename K>
simdjson_inline void tape_builder<K>::build(char const* c_str, size_t size) noexcept {
  // we advance the point, accounting for the fact that we have a NULL termination
  append(tape_->next_string_buf_index(), internal::tape_type::STRING);
  tape_->append_string(c_str, size);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(std::string_view value) noexcept {
  build(std::string(value).c_str(), value.size());
}

template<typename K>
simdjson_inline void tape_builder<K>::build(int64_t value) noexcept {
  append2(0, value, internal::tape_type::INT64);
}

template<typename K>
simdjson_inline void tape_builder<K>::build(uint64_t value) noexcept {
  append(0, internal::tape_type::UINT64);
  tape_->append(value);
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
simdjson_inline void tape_builder<K>::visit_null_atom() noexcept {
  append(0, internal::tape_type::NULL_VALUE);
}

template<typename K>
simdjson_inline void tape_builder<K>::empty_container(internal::tape_type start, internal::tape_type end) noexcept {
  auto start_index = tape_->next_tape_index();
  append(start_index+2, start);
  append(start_index, end);
}

template<typename K>
simdjson_inline uint32_t tape_builder<K>::start_container() noexcept {
  auto start_index = tape_->next_tape_index();
  tape_->skip(); // We don't actually *write* the start element until the end.
  return start_index;
}

template<typename K>
simdjson_inline void tape_builder<K>::end_container(uint32_t start_tape_index, uint32_t count, internal::tape_type start, internal::tape_type end) noexcept {
  // Write the ending tape element, pointing at the start location
  append(start_tape_index, end);
  // Write the start tape element, pointing at the end location (and including count)
  // count can overflow if it exceeds 24 bits... so we saturate
  // the convention being that a cnt of 0xffffff or more is undetermined in value (>=  0xffffff).
  const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
  write(start_tape_index, tape_->next_tape_index() | (uint64_t(cntsat) << 32), start);
}

template<typename K>
simdjson_inline void tape_builder<K>::append(uint64_t val, internal::tape_type t) noexcept {
  tape_->append(val | ((uint64_t(char(t))) << 56));
}

template<typename K>
template<typename T>
simdjson_inline void tape_builder<K>::append2(uint64_t val, T val2, internal::tape_type t) noexcept {
  append(val, t);
  static_assert(sizeof(val2) == sizeof(int64_t), "Type is not 64 bits!");
  tape_->copy(&val2);
}

template<typename K>
simdjson_inline void tape_builder<K>::write(uint32_t tape_index, uint64_t val, internal::tape_type t) noexcept {
  tape_->write(tape_index, val | ((uint64_t(char(t))) << 56));
}

} // namespace stage2
} // namespace SIMDJSON_IMPLEMENTATION
} // namespace simdjson

#endif // SIMDJSON_SRC_GENERIC_STAGE2_TAPE_BUILDER_H