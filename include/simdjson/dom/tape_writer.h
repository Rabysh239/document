#pragma once

#include <cstring>
#include <vector>
#include <simdjson/dom/base.h>
#include <simdjson/dom/document.h>

namespace simdjson {
namespace dom {

template<typename T>
class tape_writer {
public:
  virtual ~tape_writer() = default;

  /**
   * Skip the current tape entry without writing.
   *
   * Used to skip the start of the container, since we'll come back later to fill it in when the
   * container ends.
   */
  simdjson_inline void append(uint64_t val) noexcept;
  simdjson_inline void copy(void *val_ptr) noexcept;

  simdjson_inline uint64_t next_string_buf_index() noexcept;
  simdjson_inline void append_string(const char *c_str, uint32_t str_length) noexcept;
private:
  T* self();
};

template<typename T>
void tape_writer<T>::append(uint64_t val) noexcept {
  return self()->append_impl(val);
}

template<typename T>
void tape_writer<T>::copy(void *val_ptr) noexcept {
  return self()->copy_impl(val_ptr);
}

template<typename T>
uint64_t tape_writer<T>::next_string_buf_index() noexcept {
  return self()->next_string_buf_index_impl();
}

template<typename T>
void tape_writer<T>::append_string(const char *c_str, uint32_t str_length) noexcept {
  return self()->append_string_impl(c_str, str_length);
}

template<typename T>
T *tape_writer<T>::self() {
  return static_cast<T*>(this);
}
// class tape_writer

class tape_writer_to_immutable : public tape_writer<tape_writer_to_immutable> {
public:
  simdjson_inline explicit tape_writer_to_immutable(dom::immutable_document &doc);
  simdjson_inline void append_impl(uint64_t val) noexcept;
  simdjson_inline void copy_impl(void *val_ptr) noexcept;

  simdjson_inline uint64_t next_string_buf_index_impl() noexcept;
  simdjson_inline void append_string_impl(const char *c_str, uint32_t str_length) noexcept;
private:
  dom::immutable_document *doc_{};
  uint8_t *&current_string_buf_loc;
  /** The next place to write to tape */
  uint64_t *&next_tape_loc;
}; // class tape_writer_to_immutable

class tape_writer_to_mutable : public tape_writer<tape_writer_to_mutable> {
public:
  simdjson_inline explicit tape_writer_to_mutable(dom::mutable_document &doc);
  simdjson_inline void append_impl(uint64_t val) noexcept;
  simdjson_inline void copy_impl(void *val_ptr) noexcept;

  simdjson_inline uint64_t next_string_buf_index_impl() noexcept;
  simdjson_inline void append_string_impl(const char *c_str, uint32_t str_length) noexcept;
private:
  std::pmr::vector<uint64_t> *tape_ptr;
  std::pmr::vector<uint8_t> *current_string_buf;
}; // class tape_writer_to_mutable

simdjson_inline tape_writer_to_immutable::tape_writer_to_immutable(dom::immutable_document &doc)
        : doc_(&doc),
          current_string_buf_loc{doc.current_string_buf_loc},
          next_tape_loc{doc.next_tape_loc} {}

simdjson_inline void tape_writer_to_immutable::append_impl(uint64_t val) noexcept {
  *next_tape_loc = val;
  next_tape_loc++;
}

simdjson_inline void tape_writer_to_immutable::copy_impl(void *val_ptr) noexcept {
  memcpy(next_tape_loc, val_ptr, sizeof(uint64_t));
  next_tape_loc++;
}

simdjson_inline uint64_t tape_writer_to_immutable::next_string_buf_index_impl() noexcept {
  return current_string_buf_loc - doc_->string_buf.get();
}

void tape_writer_to_immutable::append_string_impl(const char *c_str, uint32_t str_length) noexcept {
  memcpy(current_string_buf_loc, &str_length, sizeof(uint32_t));
  current_string_buf_loc += sizeof(uint32_t);
  memcpy(current_string_buf_loc, c_str, str_length + 1);
  current_string_buf_loc += str_length + 1;
}

tape_writer_to_mutable::tape_writer_to_mutable(dom::mutable_document &doc)
        : tape_ptr(&doc.tape),
          current_string_buf(&doc.string_buf) {}

void tape_writer_to_mutable::append_impl(uint64_t val) noexcept {
  tape_ptr->push_back(val);
}

void tape_writer_to_mutable::copy_impl(void *val_ptr) noexcept {
  tape_ptr->push_back({});
  memcpy(&tape_ptr->back(), val_ptr, sizeof(uint64_t));
}

uint64_t tape_writer_to_mutable::next_string_buf_index_impl() noexcept {
  return current_string_buf->size();
}

void tape_writer_to_mutable::append_string_impl(const char *c_str, uint32_t str_length) noexcept {
  auto buf_size = current_string_buf->size();
  current_string_buf->resize(buf_size + sizeof(uint32_t));
  memcpy(current_string_buf->data() + buf_size, &str_length, sizeof(uint32_t));
  current_string_buf->insert(current_string_buf->end(), c_str, c_str + str_length + 1);
}

} // namespace dom
} // namespace simdjson