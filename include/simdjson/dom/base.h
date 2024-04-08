#ifndef SIMDJSON_DOM_BASE_H
#define SIMDJSON_DOM_BASE_H

#include <simdjson/base.h>

namespace simdjson {

/**
 * @brief A DOM API on top of the simdjson parser.
 */
namespace dom {

template<typename T>
class document;
class immutable_document;
class mutable_document;
class tape_writer_to_mutable;
class tape_writer_to_immutable;
template<typename T>
class element;

#ifdef SIMDJSON_THREADS_ENABLED
struct stage1_worker;
#endif // SIMDJSON_THREADS_ENABLED

} // namespace dom

namespace internal {

template<typename T>
class tape_ref;

} // namespace internal

} // namespace simdjson

#endif // SIMDJSON_DOM_BASE_H