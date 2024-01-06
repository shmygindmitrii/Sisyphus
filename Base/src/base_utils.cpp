#include "base_utils.h"

std::vector<float> Temple::Base::interpolate(float x0, float y0, float x1, float y1) {
    // interpolate second value - y in this case
    std::vector<float> vals = {};
    if (EQUAL_FLOATS(x0, x1)) {
        vals.push_back(y0);
        return vals;
    }
    float a = (y1 - y0) / fabs(x1 - x0);
    float y = y0;
    float xstep = (x1 - x0) / fabs(x1 - x0);
    while (fabs(x0) < fabs(x1)) {
        vals.push_back(y);
        y += a;
        x0 += xstep;
    }
    return vals;
}
