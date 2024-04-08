#ifndef SIMDJSON_DOM_ELEMENT_H
#define SIMDJSON_DOM_ELEMENT_H

#include <simdjson/dom/base.h>

namespace simdjson {
namespace dom {

/**
 * The actual concrete type of a JSON element
 * This is the type it is most easily cast to with get<>.
 */
enum class element_type {
  INT64 = 'l',     ///< int64_t
  UINT64 = 'u',    ///< uint64_t: any integer that fits in uint64_t but *not* int64_t
  DOUBLE = 'd',    ///< double: Any number with a "." or "e" that fits in double.
  STRING = '"',    ///< std::string_view
  BOOL = 't',      ///< bool
  NULL_VALUE = 'n' ///< null
};

/**
 * A JSON element.
 *
 * References an element in a JSON document, representing a JSON null, boolean, string, number.
 */
template<typename K>
class element {
public:
  /** Create a new, invalid element. */
  simdjson_inline element() noexcept;

  /** The type of this element. */
  simdjson_inline element_type type() const noexcept;

  /**
   * Cast this element to a null-terminated C string.
   *
   * The string is guaranteed to be valid UTF-8.
   *
   * The length of the string is given by get_string_length(). Because JSON strings
   * may contain null characters, it may be incorrect to use strlen to determine the
   * string length.
   *
   * It is possible to get a single string_view instance which represents both the string
   * content and its length: see get_string().
   *
   * @returns A pointer to a null-terminated UTF-8 string. This string is stored in the parser and will
   *          be invalidated the next time it parses a document or when it is destroyed.
   *          Returns INCORRECT_TYPE if the JSON element is not a string.
   */
  inline simdjson_result<const char *> get_c_str() const noexcept;
  /**
   * Gives the length in bytes of the string.
   *
   * It is possible to get a single string_view instance which represents both the string
   * content and its length: see get_string().
   *
   * @returns A string length in bytes.
   *          Returns INCORRECT_TYPE if the JSON element is not a string.
   */
  inline simdjson_result<size_t> get_string_length() const noexcept;
  /**
   * Cast this element to a string.
   *
   * The string is guaranteed to be valid UTF-8.
   *
   * @returns An UTF-8 string. The string is stored in the parser and will be invalidated the next time it
   *          parses a document or when it is destroyed.
   *          Returns INCORRECT_TYPE if the JSON element is not a string.
   */
  inline simdjson_result<std::string_view> get_string() const noexcept;
  /**
   * Cast this element to a signed integer.
   *
   * @returns A signed 64-bit integer.
   *          Returns INCORRECT_TYPE if the JSON element is not an integer, or NUMBER_OUT_OF_RANGE
   *          if it is negative.
   */
  inline simdjson_result<int64_t> get_int64() const noexcept;
  /**
   * Cast this element to an unsigned integer.
   *
   * @returns An unsigned 64-bit integer.
   *          Returns INCORRECT_TYPE if the JSON element is not an integer, or NUMBER_OUT_OF_RANGE
   *          if it is too large.
   */
  inline simdjson_result<uint64_t> get_uint64() const noexcept;
  /**
   * Cast this element to a double floating-point.
   *
   * @returns A double value.
   *          Returns INCORRECT_TYPE if the JSON element is not a number.
   */
  inline simdjson_result<double> get_double() const noexcept;
  /**
   * Cast this element to a bool.
   *
   * @returns A bool value.
   *          Returns INCORRECT_TYPE if the JSON element is not a boolean.
   */
  inline simdjson_result<bool> get_bool() const noexcept;

  /**
   * Whether this element is a json string.
   *
   * Equivalent to is<std::string_view>() or is<const char *>().
   */
  inline bool is_string() const noexcept;
  /**
   * Whether this element is a json number that fits in a signed 64-bit integer.
   *
   * Equivalent to is<int64_t>().
   */
  inline bool is_int64() const noexcept;
  /**
   * Whether this element is a json number that fits in an unsigned 64-bit integer.
   *
   * Equivalent to is<uint64_t>().
   */
  inline bool is_uint64() const noexcept;
  /**
   * Whether this element is a json number that fits in a double.
   *
   * Equivalent to is<double>().
   */
  inline bool is_double() const noexcept;

  /**
   * Whether this element is a json number.
   *
   * Both integers and floating points will return true.
   */
  inline bool is_number() const noexcept;

  /**
   * Whether this element is a json `true` or `false`.
   *
   * Equivalent to is<bool>().
   */
  inline bool is_bool() const noexcept;
  /**
   * Whether this element is a json `null`.
   */
  inline bool is_null() const noexcept;

  /**
   * Tell whether the value can be cast to provided type (T).
   *
   * Supported types:
   * - Boolean: bool
   * - Number: double, uint64_t, int64_t
   * - String: std::string_view, const char *
   *
   * @tparam T bool, double, uint64_t, int64_t, std::string_view, const char *
   */
  template<typename T>
  simdjson_inline bool is() const noexcept;

  /**
   * Get the value as the provided type (T).
   *
   * Supported types:
   * - Boolean: bool
   * - Number: double, uint64_t, int64_t
   * - String: std::string_view, const char *
   *
   * You may use get_double(), get_bool(), get_uint64(), get_int64(),
   *  or get_string() instead.
   *
   * @tparam T bool, double, uint64_t, int64_t, std::string_view, const char *
   *
   * @returns The value cast to the given type, or:
   *          INCORRECT_TYPE if the value cannot be cast to the given type.
   */

  template<typename T>
  inline typename std::enable_if<T::value, simdjson_result<T>>::type
  get() const noexcept {
    // Unless the simdjson library provides an inline implementation, calling this method should
    // immediately fail.
    static_assert(!sizeof(T), "The get method with given type is not implemented by the simdjson library. "
      "The supported types are Boolean (bool), numbers (double, uint64_t, int64_t), "
      "strings (std::string_view, const char *). "
      "We recommend you use get_double(), get_bool(), get_uint64(), get_int64(), "
      "or get_string() instead of the get template.");
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, const char *>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_c_str();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, std::string_view>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_string();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, std::int64_t>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_int64();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, std::uint64_t>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_uint64();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, double>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_double();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, bool>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_bool();
  }

  /**
   * Get the value as the provided type (T).
   *
   * Supported types:
   * - Boolean: bool
   * - Number: double, uint64_t, int64_t
   * - String: std::string_view, const char *
   *
   * @tparam T bool, double, uint64_t, int64_t, std::string_view, const char *
   *
   * @param value The variable to set to the value. May not be set if there is an error.
   *
   * @returns The error that occurred, or SUCCESS if there was no error.
   */
  template<typename T>
  simdjson_warn_unused simdjson_inline error_code get(T &value) const noexcept;

  // An element-specific version prevents recursion with simdjson_result::get<element>(value)
  template<typename T>
  inline typename std::enable_if<std::is_same<T, element<K>>::value, simdjson_result<T>>::type
  get(element &value) const noexcept {
    value = element(tape);
    return SUCCESS;
  }

  /**
   * Get the value as the provided type (T), setting error if it's not the given type.
   *
   * Supported types:
   * - Boolean: bool
   * - Number: double, uint64_t, int64_t
   * - String: std::string_view, const char *
   *
   * @tparam T bool, double, uint64_t, int64_t, std::string_view, const char *
   *
   * @param value The variable to set to the given type. value is undefined if there is an error.
   * @param error The variable to store the error. error is set to error_code::SUCCEED if there is an error.
   */
  template<typename T>
  inline void tie(T &value, error_code &error) && noexcept;

#if SIMDJSON_EXCEPTIONS
  /**
   * Read this element as a boolean.
   *
   * @return The boolean value
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not a boolean.
   */
  inline operator bool() const noexcept(false);

  /**
   * Read this element as a null-terminated UTF-8 string.
   *
   * Be mindful that JSON allows strings to contain null characters.
   *
   * Does *not* convert other types to a string; requires that the JSON type of the element was
   * an actual string.
   *
   * @return The string value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not a string.
   */
  inline explicit operator const char*() const noexcept(false);

  /**
   * Read this element as a null-terminated UTF-8 string.
   *
   * Does *not* convert other types to a string; requires that the JSON type of the element was
   * an actual string.
   *
   * @return The string value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not a string.
   */
  inline operator std::string_view() const noexcept(false);

  /**
   * Read this element as an unsigned integer.
   *
   * @return The integer value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not an integer
   * @exception simdjson_error(NUMBER_OUT_OF_RANGE) if the integer does not fit in 64 bits or is negative
   */
  inline operator uint64_t() const noexcept(false);
  /**
   * Read this element as an signed integer.
   *
   * @return The integer value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not an integer
   * @exception simdjson_error(NUMBER_OUT_OF_RANGE) if the integer does not fit in 64 bits
   */
  inline operator int64_t() const noexcept(false);
  /**
   * Read this element as an double.
   *
   * @return The double value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not a number
   */
  inline operator double() const noexcept(false);
#endif // SIMDJSON_EXCEPTIONS

  /**
   * operator< defines a total order for element allowing to use them in
   * ordered C++ STL containers
   *
   * @return TRUE if the key appears before the other one in the tape
   */
  inline bool operator<(const element &other) const noexcept;

  /**
   * operator== allows to verify if two element values reference the
   * same JSON item
   *
   * @return TRUE if the two values references the same JSON element
   */
  inline bool operator==(const element &other) const noexcept;

  /** @private for debugging. Prints out the root element. */
  inline bool dump_raw_tape(std::ostream &out) const noexcept;

private:
  simdjson_inline element(const internal::tape_ref<K> &tape) noexcept;
  internal::tape_ref<K> tape;
  friend class immutable_document;
  friend class mutable_document;
  friend struct simdjson_result<element>;

};

} // namespace dom

/** The result of a JSON navigation that may fail. */
template<typename K>
struct simdjson_result<dom::element<K>> : public internal::simdjson_result_base<dom::element<K>>  {
  using base = internal::simdjson_result_base<dom::element<K>>;
  using base::error;
  using base::first;
public:
  simdjson_inline simdjson_result() noexcept; ///< @private
  simdjson_inline simdjson_result(dom::element<K> &&value) noexcept; ///< @private
  simdjson_inline simdjson_result(error_code error) noexcept; ///< @private

  simdjson_inline simdjson_result<dom::element_type> type() const noexcept;
  template<typename T>
  simdjson_inline bool is() const noexcept;
  template<typename T>
  simdjson_inline simdjson_result<T> get() const noexcept;
  template<typename T>
  simdjson_warn_unused simdjson_inline error_code get(T &value) const noexcept;

  simdjson_inline simdjson_result<const char *> get_c_str() const noexcept;
  simdjson_inline simdjson_result<size_t> get_string_length() const noexcept;
  simdjson_inline simdjson_result<std::string_view> get_string() const noexcept;
  simdjson_inline simdjson_result<int64_t> get_int64() const noexcept;
  simdjson_inline simdjson_result<uint64_t> get_uint64() const noexcept;
  simdjson_inline simdjson_result<double> get_double() const noexcept;
  simdjson_inline simdjson_result<bool> get_bool() const noexcept;

  simdjson_inline bool is_string() const noexcept;
  simdjson_inline bool is_int64() const noexcept;
  simdjson_inline bool is_uint64() const noexcept;
  simdjson_inline bool is_double() const noexcept;
  simdjson_inline bool is_number() const noexcept;
  simdjson_inline bool is_bool() const noexcept;
  simdjson_inline bool is_null() const noexcept;

#if SIMDJSON_EXCEPTIONS
  simdjson_inline operator bool() const noexcept(false);
  simdjson_inline explicit operator const char*() const noexcept(false);
  simdjson_inline operator std::string_view() const noexcept(false);
  simdjson_inline operator uint64_t() const noexcept(false);
  simdjson_inline operator int64_t() const noexcept(false);
  simdjson_inline operator double() const noexcept(false);
#endif // SIMDJSON_EXCEPTIONS
};

} // namespace simdjson

#endif // SIMDJSON_DOM_DOCUMENT_H
