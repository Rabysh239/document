#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

namespace components::document {

    static inline bool is_equals(double x, double y) {
        return std::fabs(x - y) < std::numeric_limits<double>::epsilon();
    }

} // namespace document
