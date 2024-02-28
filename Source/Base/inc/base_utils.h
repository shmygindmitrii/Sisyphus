#pragma once

#include "base_constants.h"
#include <vector>
#include <cassert>

#define EQUAL_FLOATS(a, b) (abs(a - b) < Sisyphus::Base::eps)

namespace Sisyphus
{
namespace Base
{
  std::vector<float>
  interpolate(
    float x0, float y0, float x1,
    float y1); // interpolate second value
  template <typename T>
  static void
  append_data(std::vector<uint8_t>& v, const T& data)
  {
    int oldSize = v.size();
    v.resize(oldSize + sizeof(T));
    memcpy(&v[oldSize], (void*)&data, sizeof(T));
  }
  template <typename T>
  static void
  replace_data(std::vector<uint8_t>& v, const T& data, uint32_t offset)
  {
    int oldSize = v.size();
    assert(offset + sizeof(T) <= v.size());
    memcpy(&v[offset], (void*)&data, sizeof(T));
  }
  void
  append_data(std::vector<uint8_t>& v, const uint8_t* data_ptr, const int src_size, const int src_offset);
  void
  replace_data(
    std::vector<uint8_t>& v, const uint8_t* data_ptr, const int dest_offset, const int src_size, const int src_offset);
} // namespace Base
} // namespace Sisyphus