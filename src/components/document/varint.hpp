#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

namespace components::document {

template<typename T>
static inline bool is_equals(T x, T y) {
  static_assert(std::is_floating_point<T>());
  return std::fabs(x - y) < std::numeric_limits<T>::epsilon();
}

} // namespace components::document
