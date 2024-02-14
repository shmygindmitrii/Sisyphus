#pragma once
#include <cstdint>
#include <base_vectors.h>

namespace Temple
{
namespace Bonfire
{
#pragma pack(push, 1)
  struct col4u {
    uint8_t r, g, b, a;
    col4u();
    col4u(uint8_t vr, uint8_t vg, uint8_t vb, uint8_t va);
    col4u(const col4u& other);
    col4u(col4u&& other);
    col4u&
    operator=(col4u&& other) noexcept;
    col4u&
    operator=(const col4u& other);
    col4u
    operator+(const col4u& other);
    col4u
    operator-(const col4u& other);
    col4u
    operator*(const col4u& other) const;
    col4u
    operator*(float other);
  };
#pragma pack(pop)
  Base::vec4
  getFloatColor(const col4u& c);
} // namespace Bonfire
} // namespace Temple
