#ifndef SIMDJSON_DOM_OBJECT_H
#define SIMDJSON_DOM_OBJECT_H

#include "../../simdjson/dom/base.h"
#include "../../simdjson/dom/element.h"
#include "../../simdjson/internal/tape_ref.h"

namespace simdjson {
namespace dom {

/**
 * JSON object.
 */
class object {
public:
//  /** Create a new, invalid object */
//  simdjson_inline object() noexcept;

  class iterator {
  public:
    using value_type = const key_value_pair;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = value_type;
    using iterator_category = std::forward_iterator_tag;

    /**
     * Get the actual key/value pair
     */
    inline reference operator*() const noexcept;
    /**
     * Get the next key/value pair.
     *
     * Part of the std::iterator interface.
     *
     */
    inline virtual iterator& operator++() noexcept = 0;
//    /**
//     * Get the next key/value pair.
//     *
//     * Part of the std::iterator interface.
//     *
//     */
//    inline iterator operator++(int) noexcept;
    /**
     * Check if these values come from the same place in the JSON.
     *
     * Part of the std::iterator interface.
     */
    inline virtual bool operator!=(const iterator& other) const noexcept = 0;
//    inline bool operator==(const iterator& other) const noexcept;
//
//    inline bool operator<(const iterator& other) const noexcept;
//    inline bool operator<=(const iterator& other) const noexcept;
//    inline bool operator>=(const iterator& other) const noexcept;
//    inline bool operator>(const iterator& other) const noexcept;
    /**
     * Get the key of this key/value pair.
     */
    inline virtual std::string_view key() const noexcept = 0;
    /**
     * Get the length (in bytes) of the key in this key/value pair.
     * You should expect this function to be faster than key().size().
     */
    inline virtual uint32_t key_length() const noexcept = 0;
    /**
     * Returns true if the key in this key/value pair is equal
     * to the provided string_view.
     */
    inline bool key_equals(std::string_view o) const noexcept;
    /**
     * Returns true if the key in this key/value pair is equal
     * to the provided string_view in a case-insensitive manner.
     * Case comparisons may only be handled correctly for ASCII strings.
     */
    inline bool key_equals_case_insensitive(std::string_view o) const noexcept;
    /**
     * Get the key of this key/value pair.
     */
    inline virtual const char *key_c_str() const noexcept = 0;
    /**
     * Get the value of this key/value pair.
     */
    inline virtual std::shared_ptr<element> value() const noexcept = 0;
//
//    iterator() noexcept = default;
//    iterator(const iterator&) noexcept = default;
//    iterator& operator=(const iterator&) noexcept = default;

    simdjson_inline virtual ~iterator() = default;
  private:
    friend class object;
  };

  /**
   * Return the first key/value pair.
   *
   * Part of the std::iterable interface.
   */
  inline virtual std::unique_ptr<object::iterator> begin() const noexcept = 0;
  /**
   * One past the last key/value pair.
   *
   * Part of the std::iterable interface.
   */
  inline virtual std::unique_ptr<object::iterator> end() const noexcept = 0;
  /**
   * Get the size of the object (number of keys).
   * It is a saturated value with a maximum of 0xFFFFFF: if the value
   * is 0xFFFFFF then the size is 0xFFFFFF or greater.
   */
//  inline size_t size() const noexcept;
  /**
   * Get the value associated with the given key.
   *
   * The key will be matched against **unescaped** JSON:
   *
   *   dom::parser parser;
   *   int64_t(parser.parse(R"({ "a\n": 1 })"_padded)["a\n"]) == 1
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\\n"].get_uint64().error() == NO_SUCH_FIELD
   *
   * This function has linear-time complexity: the keys are checked one by one.
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   *         - INCORRECT_TYPE if this is not an object
   */
  inline simdjson_result<element> operator[](std::string_view key) const noexcept;

  /**
   * Get the value associated with the given key.
   *
   * The key will be matched against **unescaped** JSON:
   *
   *   dom::parser parser;
   *   int64_t(parser.parse(R"({ "a\n": 1 })"_padded)["a\n"]) == 1
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\\n"].get_uint64().error() == NO_SUCH_FIELD
   *
   * This function has linear-time complexity: the keys are checked one by one.
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   *         - INCORRECT_TYPE if this is not an object
   */
  inline simdjson_result<element> operator[](const char *key) const noexcept;

  /**
   * Get the value associated with the given JSON pointer. We use the RFC 6901
   * https://tools.ietf.org/html/rfc6901 standard, interpreting the current node
   * as the root of its own JSON document.
   *
   *   dom::parser parser;
   *   object obj = parser.parse(R"({ "foo": { "a": [ 10, 20, 30 ] }})"_padded);
   *   obj.at_pointer("/foo/a/1") == 20
   *   obj.at_pointer("/foo")["a"].at(1) == 20
   *
   * It is allowed for a key to be the empty string:
   *
   *   dom::parser parser;
   *   object obj = parser.parse(R"({ "": { "a": [ 10, 20, 30 ] }})"_padded);
   *   obj.at_pointer("//a/1") == 20
   *   obj.at_pointer("/")["a"].at(1) == 20
   *
   * @return The value associated with the given JSON pointer, or:
   *         - NO_SUCH_FIELD if a field does not exist in an object
   *         - INDEX_OUT_OF_BOUNDS if an array index is larger than an array length
   *         - INCORRECT_TYPE if a non-integer is used to access an array
   *         - INVALID_JSON_POINTER if the JSON pointer is invalid and cannot be parsed
   */
  inline simdjson_result<element> at_pointer(std::string_view json_pointer) const noexcept;

  /**
   * Get the value associated with the given key.
   *
   * The key will be matched against **unescaped** JSON:
   *
   *   dom::parser parser;
   *   int64_t(parser.parse(R"({ "a\n": 1 })"_padded)["a\n"]) == 1
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\\n"].get_uint64().error() == NO_SUCH_FIELD
   *
   * This function has linear-time complexity: the keys are checked one by one.
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   */
  inline virtual simdjson_result<element> at_key(std::string_view key) const noexcept = 0;
//
//  /**
//   * Get the value associated with the given key in a case-insensitive manner.
//   * It is only guaranteed to work over ASCII inputs.
//   *
//   * Note: The key will be matched against **unescaped** JSON.
//   *
//   * This function has linear-time complexity: the keys are checked one by one.
//   *
//   * @return The value associated with this field, or:
//   *         - NO_SUCH_FIELD if the field does not exist in the object
//   */
//  inline simdjson_result<element> at_key_case_insensitive(std::string_view key) const noexcept;

    inline virtual void insert(std::string key, const std::shared_ptr<element> &value) noexcept = 0;

  simdjson_inline virtual ~object() = default;

private:

  friend class element;
  friend struct simdjson_result<element>;
  template<typename T>
  friend class simdjson::internal::string_builder;

  inline virtual void wait_until_usable() const noexcept = 0;

  inline virtual std::shared_ptr<element> get_wrapped_instance() const noexcept = 0;
};

class object_d: public object {
public:
    using data_type = std::unordered_map<std::string, std::shared_ptr<element>>;
    using iterator_type = data_type::const_iterator;

    simdjson_inline object_d() noexcept = default;

    class iterator_d : public iterator {
    public:
        simdjson_inline iterator_d(iterator_type _iter) noexcept;

        inline iterator& operator++() noexcept override;

        inline bool operator!=(const iterator& other) const noexcept override;

        inline std::string_view key() const noexcept override;
        inline uint32_t key_length() const noexcept override;
        inline const char *key_c_str() const noexcept override;
        inline std::shared_ptr<element> value() const noexcept override;

    private:
        iterator_type iter;
    };

    inline std::unique_ptr<object::iterator> begin() const noexcept override;
    inline std::unique_ptr<object::iterator> end() const noexcept override;

    inline simdjson_result<element> at_key(std::string_view key) const noexcept override;

    inline void insert(std::string key, const std::shared_ptr<element> &value) noexcept override;

private:
    data_type data;

    inline void wait_until_usable() const noexcept override;

    inline std::shared_ptr<element> get_wrapped_instance() const noexcept override;
};

class object_impl: public object {
public:
    /** Create a new, invalid object */
    simdjson_inline object_impl() noexcept;
    simdjson_inline object_impl(const internal::tape_ref &tape) noexcept;

    class iterator_impl : public iterator {
    public:
        simdjson_inline iterator_impl(const internal::tape_ref &_tape) noexcept;
        simdjson_inline iterator_impl(const internal::tape_ref &_tape, size_t _end_json_index, std::unique_ptr<iterator> _dynamic_data_iter) noexcept;

        inline iterator& operator++() noexcept override;

        inline bool operator!=(const iterator& other) const noexcept override;

        inline std::string_view key() const noexcept override;
        inline uint32_t key_length() const noexcept override;
        inline const char *key_c_str() const noexcept override;
        inline std::shared_ptr<element> value() const noexcept override;

    private:
        internal::tape_ref tape;
        size_t end_json_index;
        std::unique_ptr<iterator> dynamic_data_iter;
        bool is_const;
    };

    inline std::unique_ptr<object::iterator> begin() const noexcept override;
    inline std::unique_ptr<object::iterator> end() const noexcept override;

    inline simdjson_result<element> at_key(std::string_view key) const noexcept override;

    inline void insert(std::string key, const std::shared_ptr<element> &value) noexcept override;

private:
    internal::tape_ref tape;
    std::shared_ptr<object_d> dynamic_data;

    inline void wait_until_usable() const noexcept override;

    inline std::shared_ptr<element> get_wrapped_instance() const noexcept override;
};

/**
 * Key/value pair in an object.
 */
class key_value_pair {
public:
  /** key in the key-value pair **/
  std::string_view key;
  /** value in the key-value pair **/
  std::shared_ptr<element> value;

private:
  simdjson_inline key_value_pair(std::string_view _key, std::shared_ptr<element> _value) noexcept;
  friend class object;
};

} // namespace dom

/** The result of a JSON conversion that may fail. */
template<>
struct simdjson_result<dom::object> : public internal::simdjson_result_base<dom::object> {
public:
  simdjson_inline simdjson_result() noexcept; ///< @private
  simdjson_inline simdjson_result(std::shared_ptr<dom::object> value) noexcept; ///< @private
  simdjson_inline simdjson_result(error_code error) noexcept; ///< @private

  inline simdjson_result<dom::element> operator[](std::string_view key) const noexcept;
  inline simdjson_result<dom::element> operator[](const char *key) const noexcept;
  inline simdjson_result<dom::element> at_pointer(std::string_view json_pointer) const noexcept;
  inline simdjson_result<dom::element> at_key(std::string_view key) const noexcept;
//  inline simdjson_result<dom::element> at_key_case_insensitive(std::string_view key) const noexcept;

  inline void insert(std::string key, const std::shared_ptr<dom::element> &value) noexcept;

#if SIMDJSON_EXCEPTIONS
  inline std::unique_ptr<dom::object::iterator> begin() const noexcept(false);
  inline std::unique_ptr<dom::object::iterator> end() const noexcept(false);
//  inline size_t size() const noexcept(false);
#endif // SIMDJSON_EXCEPTIONS
};

} // namespace simdjson

#if defined(__cpp_lib_ranges)
#include <ranges>

namespace std {
namespace ranges {
template<>
inline constexpr bool enable_view<simdjson::dom::object> = true;
#if SIMDJSON_EXCEPTIONS
template<>
inline constexpr bool enable_view<simdjson::simdjson_result<simdjson::dom::object>> = true;
#endif // SIMDJSON_EXCEPTIONS
} // namespace ranges
} // namespace std
#endif // defined(__cpp_lib_ranges)

#endif // SIMDJSON_DOM_OBJECT_H
