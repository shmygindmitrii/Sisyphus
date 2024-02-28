#include "base_utils.h"

std::vector<float>
Sisyphus::Base::interpolate(float x0, float y0, float x1, float y1)
{
  // interpolate second value - y in this case
  std::vector<float> vals = {};
  if (EQUAL_FLOATS(x0, x1))
  {
    vals.push_back(y0);
    return vals;
  }
  float a = (y1 - y0) / fabs(x1 - x0);
  float y = y0;
  float xstep = (x1 - x0) / fabs(x1 - x0);
  while (fabs(x0) < fabs(x1))
  {
    vals.push_back(y);
    y += a;
    x0 += xstep;
  }
  return vals;
}

void
Sisyphus::Base::append_data(std::vector<uint8_t>& v, const uint8_t* data_ptr, const int src_size, const int src_offset)
{
  int oldSize = v.size();
  v.resize(oldSize + src_size);
  memcpy(&v[oldSize], data_ptr + src_offset, src_size);
}

void
Sisyphus::Base::replace_data(
  std::vector<uint8_t>& v, const uint8_t* data_ptr, const int dest_offset, const int src_size, const int src_offset)
{
  assert(dest_offset + src_size <= v.size());
  memcpy(&v[dest_offset], data_ptr + src_offset, src_size);
}
