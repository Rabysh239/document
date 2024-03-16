#pragma once

#include <cstring>
#include <vector>
#include "../../simdjson/dom/base.h"
#include "../../simdjson/dom/document.h"

namespace simdjson {
namespace dom {

class tape_writer {
public:
  virtual simdjson_inline uint32_t next_tape_index() noexcept = 0;

  /**
   * Skip the current tape entry without writing.
   *
   * Used to skip the start of the container, since we'll come back later to fill it in when the
   * container ends.
   */
  virtual simdjson_inline void skip() noexcept = 0;
  virtual simdjson_inline void append(uint64_t val) noexcept = 0;
  virtual simdjson_inline void copy(void *val_ptr) noexcept = 0;
  virtual simdjson_inline void write(uint32_t tape_index, uint64_t val) noexcept = 0;

  virtual simdjson_inline uint64_t next_string_buf_index() noexcept = 0;
  virtual simdjson_inline void append_string(const char *c_str, uint32_t str_length) noexcept = 0;
}; // class tape_writer

class tape_writer_to_immutable : public tape_writer {
public:
  simdjson_inline explicit tape_writer_to_immutable(dom::immutable_document &doc);
  simdjson_inline uint32_t next_tape_index() noexcept override;
  simdjson_inline void skip() noexcept override;
  simdjson_inline void append(uint64_t val) noexcept override;
  simdjson_inline void copy(void *val_ptr) noexcept override;
  simdjson_inline void write(uint32_t tape_index, uint64_t val) noexcept override;

  simdjson_inline uint64_t next_string_buf_index() noexcept override;
  simdjson_inline void append_string(const char *c_str, uint32_t str_length) noexcept override;
private:
  dom::immutable_document *doc_{};
  uint8_t *current_string_buf_loc;
  /** The next place to write to tape */
  uint64_t *next_tape_loc;
}; // class tape_writer_to_immutable

class tape_writer_to_mutable : public tape_writer {
public:
  simdjson_inline explicit tape_writer_to_mutable(dom::mutable_document &doc);
  simdjson_inline uint32_t next_tape_index() noexcept override;
  simdjson_inline void skip() noexcept override;
  simdjson_inline void append(uint64_t val) noexcept override;
  simdjson_inline void copy(void *val_ptr) noexcept override;
  simdjson_inline void write(uint32_t tape_index, uint64_t val) noexcept override;

  simdjson_inline uint64_t next_string_buf_index() noexcept override;
  simdjson_inline void append_string(const char *c_str, uint32_t str_length) noexcept override;
private:
  std::vector<uint64_t> *tape_ptr;
  std::vector<uint8_t> *current_string_buf;
}; // class tape_writer_to_mutable

simdjson_inline tape_writer_to_immutable::tape_writer_to_immutable(dom::immutable_document &doc)
        : doc_(&doc),
          next_tape_loc{doc.tape.get()},
          current_string_buf_loc{doc.string_buf.get()} {}

uint32_t tape_writer_to_immutable::next_tape_index() noexcept {
  return next_tape_loc - doc_->tape.get();
}

simdjson_inline void tape_writer_to_immutable::skip() noexcept {
  next_tape_loc++;
}

simdjson_inline void tape_writer_to_immutable::append(uint64_t val) noexcept {
  *next_tape_loc = val;
  next_tape_loc++;
}

simdjson_inline void tape_writer_to_immutable::copy(void *val_ptr) noexcept {
  memcpy(next_tape_loc, val_ptr, sizeof(uint64_t));
  next_tape_loc++;
}

simdjson_inline void tape_writer_to_immutable::write(uint32_t tape_index, uint64_t val) noexcept {
  doc_->tape[tape_index] = val;
}

simdjson_inline uint64_t tape_writer_to_immutable::next_string_buf_index() noexcept {
  return current_string_buf_loc - doc_->string_buf.get();
}

void tape_writer_to_immutable::append_string(const char *c_str, uint32_t str_length) noexcept {
  memcpy(current_string_buf_loc, &str_length, sizeof(uint32_t));
  current_string_buf_loc += sizeof(uint32_t);
  memcpy(current_string_buf_loc, c_str, str_length + 1);
  current_string_buf_loc += str_length + 1;
}

tape_writer_to_mutable::tape_writer_to_mutable(dom::mutable_document &doc)
        : tape_ptr(&doc.tape),
          current_string_buf(&doc.string_buf) {}

uint32_t tape_writer_to_mutable::next_tape_index() noexcept {
  return tape_ptr->size();
}

void tape_writer_to_mutable::skip() noexcept {
  tape_ptr->push_back({});
}

void tape_writer_to_mutable::append(uint64_t val) noexcept {
  tape_ptr->push_back(val);
}

void tape_writer_to_mutable::copy(void *val_ptr) noexcept {
  tape_ptr->push_back({});
  memcpy(&tape_ptr->back(), val_ptr, sizeof(uint64_t));
}

void tape_writer_to_mutable::write(uint32_t tape_index, uint64_t val) noexcept {
  tape_ptr->operator[](tape_index) = val;
}

uint64_t tape_writer_to_mutable::next_string_buf_index() noexcept {
  return current_string_buf->size();
}

void tape_writer_to_mutable::append_string(const char *c_str, uint32_t str_length) noexcept {
  auto buf_size = current_string_buf->size();
  current_string_buf->resize(buf_size + sizeof(uint32_t));
  memcpy(current_string_buf->data() + buf_size, &str_length, sizeof(uint32_t));
  current_string_buf->insert(current_string_buf->end(), c_str, c_str + str_length + 1);
}

} // namespace dom
} // namespace simdjson