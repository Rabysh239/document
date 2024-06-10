#ifndef SIMDJSON_INTERNAL_TAPE_TYPE_H
#define SIMDJSON_INTERNAL_TAPE_TYPE_H

namespace simdjson {
namespace internal {

/**
 * The possible types in the tape.
 */
enum class tape_type {
  STRING = '"',
  INT8 = 'c',
  INT16 = 's',
  INT32 = 'i',
  INT64 = 'l',
  INT128 = 'h',
  UINT8 = '8',
  UINT16 = 'G',
  UINT32 = 'u',
  UINT64 = 'U',
  FLOAT = 'f',
  DOUBLE = 'd',
  TRUE_VALUE = '1',
  FALSE_VALUE = '0',
  NULL_VALUE = 'n'
}; // enum class tape_type

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_INTERNAL_TAPE_TYPE_H
