#ifndef SIMDJSON_SRC_GENERIC_STAGE2_TAPE_BUILDER_H

#ifndef SIMDJSON_CONDITIONAL_INCLUDE
#define SIMDJSON_SRC_GENERIC_STAGE2_TAPE_BUILDER_H
#include "../../generic/stage2/base.h"
#include "../../generic/stage2/json_iterator.h"
#include "../../generic/stage2/tape_writer.h"
#include "../../../include/simdjson/dom/document.h"
#endif // SIMDJSON_CONDITIONAL_INCLUDE


namespace simdjson {
namespace SIMDJSON_IMPLEMENTATION {
namespace stage2 {

struct tape_builder {
  static simdjson_inline void parse_document(
    dom::document &doc,
    boost::json::value &value) noexcept;

  simdjson_inline uint32_t start_container() noexcept;

  simdjson_inline void visit_document_end() noexcept;

  simdjson_inline void visit_array_end(uint32_t start_tape_index, uint32_t count) noexcept;
  simdjson_inline void visit_empty_array() noexcept;

  simdjson_inline void visit_object_end(uint32_t start_tape_index, uint32_t count) noexcept;
  simdjson_inline void visit_empty_object() noexcept;

  simdjson_inline void visit_string(const boost::json::value &value) noexcept;
  simdjson_inline void visit_number(const boost::json::value &value) noexcept;
  simdjson_inline void visit_bool_atom(const boost::json::value &value) noexcept;
  simdjson_inline void visit_null_atom() noexcept;

  tape_writer tape;
private:
  dom::document *doc_{};
  uint8_t *current_string_buf_loc;

  simdjson_inline explicit tape_builder(dom::document &doc) noexcept;

  simdjson_warn_unused simdjson_inline uint32_t next_tape_index() const noexcept;
  simdjson_inline void end_container(uint32_t start_tape_index, uint32_t count, internal::tape_type start, internal::tape_type end) noexcept;
  simdjson_inline void empty_container(internal::tape_type start, internal::tape_type end) noexcept;
}; // struct tape_builder

simdjson_inline void tape_builder::parse_document(
    dom::document &doc,
    boost::json::value &value) noexcept {
  tape_builder builder(doc);
  walk_document(builder, value);
}

simdjson_inline void tape_builder::visit_empty_object() noexcept {
  empty_container(internal::tape_type::START_OBJECT, internal::tape_type::END_OBJECT);
}
simdjson_inline void tape_builder::visit_empty_array() noexcept {
  empty_container(internal::tape_type::START_ARRAY, internal::tape_type::END_ARRAY);
}

simdjson_inline void tape_builder::visit_object_end(uint32_t start_tape_index, uint32_t count) noexcept {
  end_container(start_tape_index, count, internal::tape_type::START_OBJECT, internal::tape_type::END_OBJECT);
}
simdjson_inline void tape_builder::visit_array_end(uint32_t start_tape_index, uint32_t count) noexcept {
  end_container(start_tape_index, count, internal::tape_type::START_ARRAY, internal::tape_type::END_ARRAY);
}
simdjson_inline void tape_builder::visit_document_end() noexcept {
  constexpr uint32_t start_tape_index = 0;
  tape.append(start_tape_index, internal::tape_type::ROOT);
  tape_writer::write(doc_->tape[start_tape_index], next_tape_index(), internal::tape_type::ROOT);
}

simdjson_inline tape_builder::tape_builder(dom::document &doc) noexcept : tape{doc.tape.get()}, doc_(&doc), current_string_buf_loc{doc.string_buf.get()} {}

simdjson_inline void tape_builder::visit_string(const boost::json::value &value) noexcept {
    // we advance the point, accounting for the fact that we have a NULL termination
    tape.append(current_string_buf_loc - doc_->string_buf.get(), internal::tape_type::STRING);
    auto &str = value.get_string();
    auto str_length = uint32_t(str.size());
    // But only add the overflow check when the document itself exceeds 4GB
    memcpy(current_string_buf_loc, &str_length, sizeof(uint32_t));
    current_string_buf_loc += sizeof(uint32_t);
    memcpy(current_string_buf_loc, str.c_str(), str_length + 1);
    current_string_buf_loc += str_length + 1;
}

simdjson_inline void tape_builder::visit_number(const boost::json::value &value) noexcept {
    if (value.is_double()) {
        tape.append_double(value.get_double());
    } else if (value.is_int64()) {
        tape.append_s64(value.get_int64());
    } else if (value.is_uint64()) {
        tape.append_u64(value.get_uint64());
    }
}

simdjson_inline void tape_builder::visit_bool_atom(const boost::json::value &value) noexcept {
    tape.append(0, value.get_bool() ? internal::tape_type::TRUE_VALUE : internal::tape_type::FALSE_VALUE);
}

simdjson_inline void tape_builder::visit_null_atom() noexcept {
  tape.append(0, internal::tape_type::NULL_VALUE);
}

simdjson_inline uint32_t tape_builder::next_tape_index() const noexcept {
  return uint32_t(tape.next_tape_loc - doc_->tape.get());
}

simdjson_inline void tape_builder::empty_container(internal::tape_type start, internal::tape_type end) noexcept {
  auto start_index = next_tape_index();
  tape.append(start_index+2, start);
  tape.append(start_index, end);
}

simdjson_inline uint32_t tape_builder::start_container() noexcept {
  auto start_index = next_tape_index();
  tape.skip(); // We don't actually *write* the start element until the end.
  return start_index;
}

simdjson_inline void tape_builder::end_container(uint32_t start_tape_index, uint32_t count, internal::tape_type start, internal::tape_type end) noexcept {
  // Write the ending tape element, pointing at the start location
  tape.append(start_tape_index, end);
  // Write the start tape element, pointing at the end location (and including count)
  // count can overflow if it exceeds 24 bits... so we saturate
  // the convention being that a cnt of 0xffffff or more is undetermined in value (>=  0xffffff).
  const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
  tape_writer::write(doc_->tape[start_tape_index], next_tape_index() | (uint64_t(cntsat) << 32), start);
}

} // namespace stage2
} // namespace SIMDJSON_IMPLEMENTATION
} // namespace simdjson

#endif // SIMDJSON_SRC_GENERIC_STAGE2_TAPE_BUILDER_H