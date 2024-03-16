#ifndef SIMDJSON_DOM_DOCUMENT_H
#define SIMDJSON_DOM_DOCUMENT_H

#include "../../simdjson/dom/base.h"
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <memory>

namespace simdjson {
namespace dom {

/**
 * A parsed JSON document.
 *
 * This class cannot be copied, only moved, to avoid unintended allocations.
 */
class document : public boost::intrusive_ref_counter<document> {
public:
  virtual uint64_t &get_tape(size_t json_index) const = 0;
  virtual uint8_t &get_string_buf(size_t json_index) const = 0;
  virtual uint8_t *get_string_buf_ptr() const = 0;
  /**
 * @private Dump the raw tape for debugging.
 *
 * @param os the stream to output to.
 * @return false if the tape is likely wrong (e.g., you did not parse a valid JSON).
 */
  bool dump_raw_tape(std::ostream &os) const noexcept;
}; // class document

class immutable_document : public document {
public:
  /**
   * Create a document container with zero capacity.
   *
   * The parser will allocate capacity as needed.
   */
  immutable_document() noexcept = default;
  ~immutable_document() noexcept = default;

  /**
   * Take another document's buffers.
   *
   * @param other The document to take. Its capacity is zeroed and it is invalidated.
   */
  immutable_document(immutable_document &&other) noexcept = default;
  /** @private */
  immutable_document(const immutable_document &) = delete; // Disallow copying
  /**
   * Take another document's buffers.
   *
   * @param other The document to take. Its capacity is zeroed.
   */
  immutable_document &operator=(immutable_document &&other) noexcept = default;
  /** @private */
  immutable_document &operator=(const immutable_document &) = delete; // Disallow copying

  uint64_t &get_tape(size_t json_index) const override;
  uint8_t &get_string_buf(size_t json_index) const override;
  uint8_t *get_string_buf_ptr() const override;

  /**
   * Get the root element of this document as a JSON array.
   */
  element root() const noexcept;

  /** @private Allocate memory to support
   * input JSON documents of up to len bytes.
   *
   * When calling this function, you lose
   * all the data.
   *
   * The memory allocation is strict: you
   * can you use this function to increase
   * or lower the amount of allocated memory.
   * Passsing zero clears the memory.
   */
  error_code allocate(size_t len) noexcept;
  /** @private Capacity in bytes, in terms
   * of how many bytes of input JSON we can
   * support.
   */
  size_t capacity() const noexcept;


private:
  /** @private Structural values. */
  std::unique_ptr<uint64_t[]> tape{};

  /** @private String values.
   *
   * Should be at least byte_capacity.
   */
  std::unique_ptr<uint8_t[]> string_buf{};
  size_t allocated_capacity{0};
  friend class parser;
  friend class tape_writer_to_immutable;
}; // class immutable_document

class mutable_document : public document {
public:
  uint64_t &get_tape(size_t json_index) const override;
  uint8_t &get_string_buf(size_t json_index) const override;
  uint8_t *get_string_buf_ptr() const override;
  element next_element() const noexcept;
private:
  std::unique_ptr<std::vector<uint64_t>> tape{new std::vector<uint64_t>};
  std::unique_ptr<std::vector<uint8_t>> string_buf{new std::vector<uint8_t>};
  friend class tape_writer_to_mutable;
}; // class mutable_document

} // namespace dom
} // namespace simdjson

#endif // SIMDJSON_DOM_DOCUMENT_H
