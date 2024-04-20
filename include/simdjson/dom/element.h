#ifndef SIMDJSON_DOM_ELEMENT_H
#define SIMDJSON_DOM_ELEMENT_H

#include <simdjson/dom/base.h>

namespace simdjson {
namespace dom {

/**
 * The actual concrete type of a JSON element
 * This is the type it is most easily cast_from to with get<>.
 */
enum class element_type {
  INT32 = 'i',
  INT64 = 'l',     ///< int64_t
  INT128 = 'h',
  UINT32 = 'u',
  UINT64 = 'U',    ///< uint64_t: any integer that fits in uint64_t but *not* int64_t
  FLOAT = 'f',
  DOUBLE = 'd',    ///< double: Any number with a "." or "e" that fits in double.
  STRING = '"',    ///< std::string_view
  BOOL = '1',      ///< bool
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

  inline simdjson_result<int32_t> get_int32() const noexcept;
  /**
   * Cast this element to a signed integer.
   *
   * @returns A signed 64-bit integer.
   *          Returns INCORRECT_TYPE if the JSON element is not an integer, or NUMBER_OUT_OF_RANGE
   *          if it is negative.
   */
  inline simdjson_result<int64_t> get_int64() const noexcept;

  inline simdjson_result<__int128_t> get_int128() const noexcept;

  inline simdjson_result<uint32_t> get_uint32() const noexcept;
  /**
   * Cast this element to an unsigned integer.
   *
   * @returns An unsigned 64-bit integer.
   *          Returns INCORRECT_TYPE if the JSON element is not an integer, or NUMBER_OUT_OF_RANGE
   *          if it is too large.
   */
  inline simdjson_result<uint64_t> get_uint64() const noexcept;

  inline simdjson_result<float> get_float() const noexcept;
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

  inline bool is_int32() const noexcept;
  /**
   * Whether this element is a json number that fits in a signed 64-bit integer.
   *
   * Equivalent to is<int64_t>().
   */
  inline bool is_int64() const noexcept;

  inline bool is_int128() const noexcept;

  inline bool is_uint32() const noexcept;
  /**
   * Whether this element is a json number that fits in an unsigned 64-bit integer.
   *
   * Equivalent to is<uint64_t>().
   */
  inline bool is_uint64() const noexcept;

  inline bool is_float() const noexcept;
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
   * Tell whether the value can be cast_from to provided type (T).
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
   * @returns The value cast_from to the given type, or:
   *          INCORRECT_TYPE if the value cannot be cast_from to the given type.
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
  inline typename std::enable_if<std::is_same<T, int32_t>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_int32();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, int64_t>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_int64();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, __int128_t>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_int128();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, uint32_t>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_uint32();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, uint64_t>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_uint64();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, float>::value, simdjson_result<T>>::type
  get() const noexcept {
    return get_float();
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

  template<typename T>
  friend class document;
  friend struct simdjson_result<element>;

};

template<typename K, typename From, typename To, typename std::enable_if<std::is_signed<From>::value && std::is_signed<To>::value, uint8_t>::type = 0>
simdjson_result<To> cast_from(internal::tape_ref<K> tape) noexcept {
  From result = tape.template next_tape_value<From>();
  if (result > From((std::numeric_limits<To>::max)()) || result < From((std::numeric_limits<To>::min)() < result)) {
    return NUMBER_OUT_OF_RANGE;
  }
  return static_cast<To>(result);
}

template<typename K, typename From, typename To, typename std::enable_if<std::is_signed<From>::value && std::is_unsigned<To>::value, uint8_t>::type = 1>
simdjson_result<To> cast_from(internal::tape_ref<K> tape) noexcept {
  From result = tape.template next_tape_value<From>();
  if (result < 0) {
    return NUMBER_OUT_OF_RANGE;
  }
  return static_cast<To>(result);
}

template<typename K, typename From, typename To, typename std::enable_if<std::is_unsigned<From>::value, uint8_t>::type = 2>
simdjson_result<To> cast_from(internal::tape_ref<K> tape) noexcept {
  From result = tape.template next_tape_value<From>();
  // Wrapping max in parens to handle Windows issue: https://stackoverflow.com/questions/11544073/how-do-i-deal-with-the-max-macro-in-windows-h-colliding-with-max-in-std
  if (result > From((std::numeric_limits<To>::max)())) {
    return NUMBER_OUT_OF_RANGE;
  }
  return static_cast<To>(result);
}

} // namespace dom
} // namespace simdjson

#endif // SIMDJSON_DOM_ELEMENT_H
