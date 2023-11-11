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

Temple::Base::vec3::vec3(const vec3& a) : x(a.x), y(a.y), z(a.z) {

}

Temple::Base::vec3& Temple::Base::vec3::operator=(const vec3& a) {
    this->x = a.x;
    this->y = a.y;
    this->z = a.z;
    return *this;
}

Temple::Base::vec3& Temple::Base::vec3::operator=(vec3&& a) noexcept {
    this->x = a.x;
    this->y = a.y;
    this->z = a.z;
    return *this;
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

Temple::Base::vec4::vec4(const vec3& v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) {

}

std::ostream& Temple::Base::operator <<(std::ostream& os, const Temple::Base::vec4& v) {
    os << std::fixed << std::setprecision(4) << '[' << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w << ']';
    return os;
}
