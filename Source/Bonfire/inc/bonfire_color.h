#pragma once
#include <cstdint>
#include <base_vectors.h>

namespace Temple
{
namespace Bonfire
{
#pragma pack(push, 1)
  struct col4u_t {
    uint8_t r, g, b, a;
    col4u_t
    operator+(const col4u_t& other);
    col4u_t
    operator-(const col4u_t& other);
    col4u_t
    operator*(const col4u_t& other) const;
    col4u_t
    operator*(float other);
  };
#pragma pack(pop)
  Base::vec4_t
  get_color_vec4(const col4u_t& c);
} // namespace Bonfire
} // namespace Temple
