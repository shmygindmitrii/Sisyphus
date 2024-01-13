#include "bonfire_color.h"

Temple::Bonfire::col4u::col4u() : r(0), g(0), b(0), a(0) {

}

Temple::Bonfire::col4u::col4u(uint8_t vr, uint8_t vg, uint8_t vb, uint8_t va) : r(vr), g(vg), b(vb), a(va) {

}

Temple::Bonfire::col4u::col4u(const col4u& other) : r(other.r), g(other.g), b(other.b), a(other.a) {

}

Temple::Bonfire::col4u::col4u(col4u&& other) : r(other.r), g(other.g), b(other.b), a(other.a) {

}

Temple::Bonfire::col4u& Temple::Bonfire::col4u::operator=(col4u&& other) noexcept {
    r = other.r;
    g = other.g;
    b = other.b;
    a = other.a;
    return *this;
}

Temple::Bonfire::col4u& Temple::Bonfire::col4u::operator=(const col4u& other) {
    r = other.r;
    g = other.g;
    b = other.b;
    a = other.a;
    return *this;
}

Temple::Bonfire::col4u Temple::Bonfire::col4u::operator+(const col4u& other) {
    col4u res;
    res.r = (uint8_t)( (r + (int)other.r) % 256 );
    res.g = (uint8_t)( (g + (int)other.g) % 256 );
    res.b = (uint8_t)( (b + (int)other.b) % 256 );
    res.a = (uint8_t)( (a + (int)other.a) % 256 );
    return res;
}

Temple::Bonfire::col4u Temple::Bonfire::col4u::operator-(const col4u& other) {
    col4u res;
    res.r = (uint8_t)(r - (int)other.r >= 0 ? r - (int)other.r : 0);
    res.g = (uint8_t)(g - (int)other.g >= 0 ? g - (int)other.g : 0);
    res.b = (uint8_t)(b - (int)other.b >= 0 ? b - (int)other.b : 0);
    res.a = (uint8_t)(a - (int)other.a >= 0 ? a - (int)other.a : 0);
    return res;
}

Temple::Bonfire::col4u Temple::Bonfire::col4u::operator*(const col4u& other) const {
    col4u res;
    res.r = (uint8_t)(r * other.r >= 0 ? r * other.r : 0);
    res.g = (uint8_t)(g * other.g >= 0 ? g * other.g : 0);
    res.b = (uint8_t)(b * other.b >= 0 ? b * other.b : 0);
    res.a = (uint8_t)(a * other.a >= 0 ? a * other.a : 0);
    return res;
}

Temple::Bonfire::col4u Temple::Bonfire::col4u::operator*(float other) {
    col4u res;
    res.r = (uint8_t)( ((int)(r * other)) % 256 );
    res.g = (uint8_t)( ((int)(g * other)) % 256 );
    res.b = (uint8_t)( ((int)(b * other)) % 256 );
    res.a = (uint8_t)( ((int)(a * other)) % 256 );
    return res;
}
