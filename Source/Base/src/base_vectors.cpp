#include "base_constants.h"
#include "base_vectors.h"

#include <cmath>
#include <iomanip>

// other methods tend to be inlined

// vec2

std::ostream&
Temple::Base::operator<<(std::ostream& os, const Temple::Base::vec2& v)
{
  os << std::fixed << std::setprecision(4) << '[' << v.x << ' ' << v.y << ']';
  return os;
}

// vec3

std::ostream&
Temple::Base::operator<<(std::ostream& os, const Temple::Base::vec3& v)
{
  os << std::fixed << std::setprecision(4) << '[' << v.x << ' ' << v.y << ' '
     << v.z << ']';
  return os;
}


// vec4

std::ostream&
Temple::Base::operator<<(std::ostream& os, const Temple::Base::vec4& v)
{
  os << std::fixed << std::setprecision(4) << '[' << v.x << ' ' << v.y << ' '
     << v.z << ' ' << v.w << ']';
  return os;
}
