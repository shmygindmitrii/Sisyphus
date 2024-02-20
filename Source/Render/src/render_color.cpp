#include "render_color.h"

Sisyphus::Render::col4u_t
Sisyphus::Render::col4u_t::operator+(const col4u_t& other)
{
    col4u_t res;
    res.r = (uint8_t)((r + (int)other.r) % 256);
    res.g = (uint8_t)((g + (int)other.g) % 256);
    res.b = (uint8_t)((b + (int)other.b) % 256);
    res.a = (uint8_t)((a + (int)other.a) % 256);
    return res;
}

Sisyphus::Render::col4u_t
Sisyphus::Render::col4u_t::operator-(const col4u_t& other)
{
    col4u_t res;
    res.r = (uint8_t)(r - (int)other.r >= 0 ? r - (int)other.r : 0);
    res.g = (uint8_t)(g - (int)other.g >= 0 ? g - (int)other.g : 0);
    res.b = (uint8_t)(b - (int)other.b >= 0 ? b - (int)other.b : 0);
    res.a = (uint8_t)(a - (int)other.a >= 0 ? a - (int)other.a : 0);
    return res;
}

Sisyphus::Render::col4u_t
Sisyphus::Render::col4u_t::operator*(const col4u_t& other) const
{
    col4u_t res;
    res.r = (uint8_t)(r * other.r >= 0 ? r * other.r : 0);
    res.g = (uint8_t)(g * other.g >= 0 ? g * other.g : 0);
    res.b = (uint8_t)(b * other.b >= 0 ? b * other.b : 0);
    res.a = (uint8_t)(a * other.a >= 0 ? a * other.a : 0);
    return res;
}

Sisyphus::Render::col4u_t
Sisyphus::Render::col4u_t::operator*(float other)
{
    col4u_t res;
    res.r = (uint8_t)(((int)(r * other)) % 256);
    res.g = (uint8_t)(((int)(g * other)) % 256);
    res.b = (uint8_t)(((int)(b * other)) % 256);
    res.a = (uint8_t)(((int)(a * other)) % 256);
    return res;
}

Sisyphus::Base::vec4_t
Sisyphus::Render::get_color_vec4(const col4u_t& c)
{
    return Base::vec4_t {c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f};
}
