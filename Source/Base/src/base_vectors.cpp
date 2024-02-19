#include "base_constants.h"
#include "base_vectors.h"

#include <cmath>
#include <iomanip>

// other methods tend to be inlined

// vec2_t

std::ostream&
Sisyphus::Base::operator<<(std::ostream& os, const Sisyphus::Base::vec2_t& v)
{
    os << std::fixed << std::setprecision(4) << '[' << v.x << ' ' << v.y << ']';
    return os;
}

// vec3_t

std::ostream&
Sisyphus::Base::operator<<(std::ostream& os, const Sisyphus::Base::vec3_t& v)
{
    os << std::fixed << std::setprecision(4) << '[' << v.x << ' ' << v.y << ' ' << v.z << ']';
    return os;
}


// vec4_t

std::ostream&
Sisyphus::Base::operator<<(std::ostream& os, const Sisyphus::Base::vec4_t& v)
{
    os << std::fixed << std::setprecision(4) << '[' << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w << ']';
    return os;
}
