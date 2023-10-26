#include "base_constants.h"
#include "base_vectors.h"

#include <cmath>
#include <iomanip>

// other methods tend to be inlined

// vec3 first

Temple::Base::vec3::vec3() : x(0.0f), y(0.0f), z(0.0f) {

}

Temple::Base::vec3::vec3(float a, float b, float c) : x(a), y(b), z(c) {

}

std::ostream& Temple::Base::operator <<(std::ostream& os, const Temple::Base::vec3& v) {
    os << std::fixed << std::setprecision(4) << '[' << v.x << ' ' << v.y << ' ' << v.z << ']';
    return os;
}


// vec4

Temple::Base::vec4::vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {

}

Temple::Base::vec4::vec4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) {

}

Temple::Base::vec4::vec4(float a, float b, float c) : x(a), y(b), z(c), w(0.0f) {

}

Temple::Base::vec4::vec4(const Temple::Base::vec3& v) : x(v.x), y(v.y), z(v.z), w(0.0f) {

}

std::ostream& Temple::Base::operator <<(std::ostream& os, const Temple::Base::vec4& v) {
    os << std::fixed << std::setprecision(4) << '[' << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w << ']';
    return os;
}
