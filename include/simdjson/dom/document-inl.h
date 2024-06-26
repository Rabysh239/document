#ifndef SIMDJSON_DOCUMENT_INL_H
#define SIMDJSON_DOCUMENT_INL_H

// Inline implementations go in here.

#include <simdjson/dom/base.h>
#include <simdjson/dom/document.h>
#include <simdjson/dom/element-inl.h>
#include <simdjson/internal/tape_ref-inl.h>
#include <simdjson/internal/jsonformatutils.h>

#include <cstring>

namespace simdjson {
namespace dom {

template<typename T>
array_deleter<T>::array_deleter()
        : allocator_(nullptr), n_(0) {}

template<typename T>
array_deleter<T>::array_deleter(std::pmr::memory_resource *allocator, size_t n)
        : allocator_(allocator), n_(n) {}

template<typename T>
array_deleter<T>::array_deleter(array_deleter &&other) noexcept
        : allocator_(other.allocator_),
          n_(other.n_) {
  other.allocator_ = nullptr;
}

template<typename T>
array_deleter<T> &array_deleter<T>::operator=(array_deleter &&other) noexcept {
  allocator_ = other.allocator_;
  n_ = other.n_;
  other.allocator_ = nullptr;
  return *this;
}

template<typename T>
void array_deleter<T>::operator()(T *p) {
  if (allocator_ != nullptr) {
    std::destroy_n(p, n_);
    allocator_->deallocate(p, n_ * sizeof(T));
  }
}

//
// document inline implementation
//

template<typename T>
const uint64_t &document<T>::get_tape(size_t json_index) const noexcept {
  return self()->get_tape_impl(json_index);
}

template<typename T>
const uint8_t &document<T>::get_string_buf(size_t json_index) const noexcept {
  return self()->get_string_buf_impl(json_index);
}

template<typename T>
const uint8_t *document<T>::get_string_buf_ptr() const noexcept {
  return self()->get_string_buf_ptr_impl();
}

template<typename T>
size_t document<T>::size() const noexcept {
  return self()->size_impl();
}

template<typename T>
element<T> document<T>::next_element() const noexcept {
  return {internal::tape_ref(this, size())};
}

template<typename T>
inline bool document<T>::dump_raw_tape(std::ostream &os) const noexcept {
  uint32_t string_length;
  size_t tape_idx = 0;
  size_t how_many = size();
  uint64_t payload;
  for (; tape_idx < how_many; tape_idx++) {
    os << tape_idx << " : ";
    auto tape_val = get_tape(tape_idx);
    auto type = uint8_t(tape_val >> 56);
    switch (type) {
      case '"': // we have a string
        os << "string \"";
        payload = tape_val & internal::JSON_VALUE_MASK;
        std::memcpy(&string_length, get_string_buf_ptr() + payload, sizeof(uint32_t));
        os << internal::escape_json_string(std::string_view(
                reinterpret_cast<const char *>(get_string_buf_ptr() + payload + sizeof(uint32_t)),
                string_length
        ));
        os << '"';
        os << '\n';
        break;
      case 'c':
        os << "char " << static_cast<int8_t>(get_tape(tape_idx) & internal::JSON_VALUE_MASK) << "\n";
        break;
      case 's':
        os << "short int " << static_cast<int16_t>(get_tape(tape_idx) & internal::JSON_VALUE_MASK) << "\n";
        break;
      case 'i':
        os << "int " << static_cast<int32_t>(get_tape(tape_idx) & internal::JSON_VALUE_MASK) << "\n";
        break;
      case 'l': // we have a long int
        if (tape_idx + 1 >= how_many) {
          return false;
        }
        os << "long " << static_cast<int64_t>(get_tape(++tape_idx)) << "\n";
        break;
      case 'h': // we have a long int
        if (tape_idx + 2 >= how_many) {
          return false;
        }
        os << "hugeint\n"; //TODO support value
        tape_idx += 2;
        break;
      case '8':
        os << "unsigned char " << static_cast<uint8_t>(get_tape(tape_idx) & internal::JSON_VALUE_MASK) << "\n";
        break;
      case 'G':
        os << "unsigned short int " << static_cast<uint16_t>(get_tape(tape_idx) & internal::JSON_VALUE_MASK) << "\n";
        break;
      case 'u':
        os << "unsigned int " << static_cast<uint32_t>(get_tape(tape_idx) & internal::JSON_VALUE_MASK) << "\n";
        break;
      case 'U': // we have a long uint
        if (tape_idx + 1 >= how_many) {
          return false;
        }
        os << "unsigned long " << get_tape(++tape_idx) << "\n";
        break;
      case 'f':
        os << "float ";
        float float_value;
        std::memcpy(&float_value, &get_tape(tape_idx), sizeof(float_value));
        os << float_value << '\n';
        break;
      case 'd': // we have a double
        os << "double ";
        if (tape_idx + 1 >= how_many) {
          return false;
        }
        double answer;
        std::memcpy(&answer, &get_tape(++tape_idx), sizeof(answer));
        os << answer << '\n';
        break;
      case 'n': // we have a null
        os << "null\n";
        break;
      case '1': // we have a true
        os << "true\n";
        break;
      case '0': // we have a false
        os << "false\n";
        break;
      default:
        return false;
    }
  }
  return true;
}

template<typename T>
const T *document<T>::self() const {
  return static_cast<const T*>(this);
}

inline const uint64_t &immutable_document::get_tape_impl(size_t json_index) const {
  return tape[json_index];
}

inline const uint8_t &immutable_document::get_string_buf_impl(size_t json_index) const {
  return string_buf[json_index];
}

inline const uint8_t *immutable_document::get_string_buf_ptr_impl() const noexcept {
  return string_buf.get();
}

simdjson_warn_unused
inline size_t immutable_document::capacity() const noexcept {
  return allocated_capacity;
}

simdjson_warn_unused
inline error_code immutable_document::allocate(size_t capacity) noexcept {
  if (capacity == 0) {
    string_buf.reset();
    tape.reset();
    allocated_capacity = 0;
    return SUCCESS;
  }

  // a pathological input like "[[[[..." would generate capacity tape elements, so
  // need a capacity of at least capacity + 1, but it is also possible to do
  // worse with "[7,7,7,7,6,7,7,7,6,7,7,6,[7,7,7,7,6,7,7,7,6,7,7,6,7,7,7,7,7,7,6"
  //where capacity + 1 tape elements are
  // generated, see issue https://github.com/simdjson/simdjson/issues/345
  size_t tape_capacity = SIMDJSON_ROUNDUP_N(capacity + 3, 64);
  // a document with only zero-length strings... could have capacity/3 string
  // and we would need capacity/3 * 5 bytes on the string buffer
  size_t string_capacity = SIMDJSON_ROUNDUP_N(5 * capacity / 3 + SIMDJSON_PADDING, 64);
  try {
    string_buf = allocator_make_unique_ptr<uint8_t>(allocator_, string_capacity);
    tape = allocator_make_unique_ptr<uint64_t>(allocator_, tape_capacity);
    next_tape_loc = tape.get();
    current_string_buf_loc = string_buf.get();
  } catch (std::bad_alloc &) {
    allocated_capacity = 0;
    string_buf.reset();
    tape.reset();
    return MEMALLOC;
  }
  // Technically the allocated_capacity might be larger than capacity
  // so the next line is pessimistic.
  allocated_capacity = capacity;
  return SUCCESS;
}

inline immutable_document::immutable_document() noexcept
        : allocator_(nullptr) {}

inline immutable_document::immutable_document(immutable_document::allocator_type *allocator) noexcept
        : allocator_(allocator) {}

inline immutable_document::immutable_document(immutable_document &&other) noexcept
        : allocator_(other.allocator_),
          tape(std::move(other.tape)),
          string_buf(std::move(other.string_buf)),
          next_tape_loc(other.next_tape_loc),
          current_string_buf_loc(other.current_string_buf_loc) {
  other.allocator_ = nullptr;
  other.next_tape_loc = nullptr;
  other.current_string_buf_loc = nullptr;
}

inline immutable_document &immutable_document::operator=(immutable_document &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  allocator_ = other.allocator_;
  tape = std::move(other.tape);
  string_buf = std::move(other.string_buf);
  next_tape_loc = other.next_tape_loc;
  current_string_buf_loc = other.current_string_buf_loc;
  other.allocator_ = nullptr;
  other.next_tape_loc = nullptr;
  other.current_string_buf_loc = nullptr;
  return *this;
}

inline size_t immutable_document::size_impl() const noexcept {
  return next_tape_loc - tape.get();
}

inline mutable_document::mutable_document(mutable_document::allocator_type *allocator) noexcept
        : tape(allocator),
          string_buf(allocator) {}

inline mutable_document::mutable_document(mutable_document &&other) noexcept
        : tape(std::move(other.tape)),
          string_buf(std::move(other.string_buf)) {}

inline const uint64_t &mutable_document::get_tape_impl(size_t json_index) const {
  return tape[json_index];
}

inline const uint8_t &mutable_document::get_string_buf_impl(size_t json_index) const {
  return string_buf[json_index];
}

inline const uint8_t *mutable_document::get_string_buf_ptr_impl() const noexcept {
  return string_buf.data();
}

inline size_t mutable_document::size_impl() const noexcept {
  return tape.size();
}

template<typename T>
std::unique_ptr<T[], array_deleter<T>> allocator_make_unique_ptr(std::pmr::memory_resource *allocator, size_t n) {
  T* array = new(allocator->allocate(n * sizeof(T))) T[n];
  return {array, array_deleter<T>(allocator, n)};
}

} // namespace dom
} // namespace simdjson

#endif // SIMDJSON_DOCUMENT_INL_H
