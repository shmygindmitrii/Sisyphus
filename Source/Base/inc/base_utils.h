#pragma once

#include "base_constants.h"
#include <vector>
#include <cassert>

#define EQUAL_FLOATS(a, b) (abs(a - b) < Temple::Base::EPS)

namespace Temple
{
namespace Base
{
  std::vector<float>
  interpolate(
    float x0,
    float y0,
    float x1,
    float y1); // interpolate second value
  template <typename T>
  static void
  appendData(std::vector<uint8_t>& v, const T& data)
  {
    int oldSize = v.size();
    v.resize(oldSize + sizeof(T));
    memcpy(&v[oldSize], (void*)&data, sizeof(T));
  }
  template <typename T>
  static void
  replaceData(std::vector<uint8_t>& v, const T& data, uint32_t offset)
  {
    int oldSize = v.size();
    assert(offset + sizeof(T) <= v.size());
    memcpy(&v[offset], (void*)&data, sizeof(T));
  }
  void
  appendData(
    std::vector<uint8_t>& v,
    const uint8_t*        pData,
    const int             srcSize,
    const int             srcOffset);
  void
  replaceData(
    std::vector<uint8_t>& v,
    const uint8_t*        pData,
    const int             destOffset,
    const int             srcSize,
    const int             srcOffset);
} // namespace Base
} // namespace Temple