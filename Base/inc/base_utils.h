#pragma once

#include "base_constants.h"
#include <vector>

#define EQUAL_FLOATS(a, b) (abs(a - b) < Temple::Base::EPS)

namespace Temple {
    namespace Base {
        std::vector<float> interpolate(float x0, float y0, float x1, float y1); // interpolate second value
    }
}