#ifndef SIMDJSON_SRC_ERROR_TABLES_CPP
#define SIMDJSON_SRC_ERROR_TABLES_CPP

#include <simdjson/error-inl.h>

namespace simdjson {
namespace internal {

  SIMDJSON_DLLIMPORTEXPORT const error_code_info error_codes[] {
    { SUCCESS, "SUCCESS: No error" },
    { MEMALLOC, "MEMALLOC: Error allocating memory, we're most likely out of memory" },
    {UNINITIALIZED,              "UNINITIALIZED: Uninitialized" },
    {INCORRECT_TYPE,             "INCORRECT_TYPE: The JSON element does not have the requested type." },
    {NUMBER_OUT_OF_RANGE,        "NUMBER_OUT_OF_RANGE: The JSON number is too large or too small to fit within the requested type." },
    {UNEXPECTED_ERROR,           "UNEXPECTED_ERROR: Unexpected error, consider reporting this problem as you may have found a bug in simdjson" },
  }; // error_messages[]

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_SRC_ERROR_TABLES_CPP