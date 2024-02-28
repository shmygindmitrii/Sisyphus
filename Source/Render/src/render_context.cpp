#include "render_context.h"
#include "base_utils.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>

Sisyphus::Render::VertexFormat::VertexFormat(const std::vector<Sisyphus::Render::EVertexAttribType>& attribs)
{
  size = 0;
  for (int i = 0; i < attribs.size(); i++)
  {
    attributes.push_back(attribs[i]);
    switch (attribs[i])
    {
    case EVertexAttribType::FLOAT32:
      size += 4;
      break;
    case EVertexAttribType::INT32:
      size += 4;
      break;
    case EVertexAttribType::UINT8:
      size += 1;
      break;
    case EVertexAttribType::VEC2:
      size += 8;
      break;
    case EVertexAttribType::VEC3:
      size += 12;
      break;
    case EVertexAttribType::VEC4:
      size += 16;
      break;
    case EVertexAttribType::UV:
      size += 8;
      break;
    }
  }
}

template <typename T>
static inline void
interpolate_attribs_template(const uint8_t*& a_in, const uint8_t*& b_in, uint8_t*& c_out, float weight)
{
  *(T*)c_out = (*(T*)b_in) * weight - (*(T*)a_in) * weight + *(T*)a_in;
  a_in += sizeof(T);
  b_in += sizeof(T);
  c_out += sizeof(T);
}

void
Sisyphus::Render::interpolate_attributes(
  const uint8_t* a_in, const uint8_t* b_in, uint8_t* c_out, float weight, const VertexFormat& vf)
{
  // c_out should be enough to hold a_in or b_in (they are the same in terms of
  // size) weight is always from 0 to 1
  for (int i = 0; i < vf.attributes.size(); i++)
  {
    switch (vf.attributes[i])
    {
    case EVertexAttribType::FLOAT32:
      interpolate_attribs_template<float>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::INT32:
      interpolate_attribs_template<int32_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::UINT8:
      interpolate_attribs_template<uint8_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::VEC2:
      interpolate_attribs_template<Base::vec2_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::VEC3:
      interpolate_attribs_template<Base::vec3_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::VEC4:
      interpolate_attribs_template<Base::vec4_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::UV:
      interpolate_attribs_template<Base::vec2_t>(a_in, b_in, c_out, weight);
      break;
    }
  }
}

template <typename T>
static inline void
mult_attribs_template(const uint8_t*& a_in, uint8_t*& c_out, float mult)
{
  *(T*)c_out = (*(T*)a_in) * mult;
  a_in += sizeof(T);
  c_out += sizeof(T);
}

void
Sisyphus::Render::multiply_attributes(const uint8_t* a_in, uint8_t* c_out, float mult, const VertexFormat& vf)
{
  for (int i = 0; i < vf.attributes.size(); i++)
  {
    switch (vf.attributes[i])
    {
    case EVertexAttribType::FLOAT32:
      mult_attribs_template<float>(a_in, c_out, mult);
      break;
    case EVertexAttribType::INT32:
      mult_attribs_template<int32_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::UINT8:
      mult_attribs_template<uint8_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::VEC2:
      mult_attribs_template<Base::vec2_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::VEC3:
      mult_attribs_template<Base::vec3_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::VEC4:
      mult_attribs_template<Base::vec4_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::UV:
      mult_attribs_template<Base::vec2_t>(a_in, c_out, mult);
      break;
    }
  }
}

Sisyphus::Render::Plane::Plane()
    : normal(Base::vec3_t {0.0f, 0.0f, 1.0f})
    , offset(0.0f)
{}

Sisyphus::Render::Plane::Plane(const Base::vec3_t& _normal, float _offset)
    : normal(_normal)
    , offset(_offset)
{}

Sisyphus::Render::Context::Context(int width, int height, int bytes_per_pixel)
    : m_width(width)
    , m_height(height)
    , m_bytes_per_pixel(bytes_per_pixel)
    , m_depth_write(true)
    , m_depth_test(true)
{
  size_t full_size = m_width * m_height * (size_t)m_bytes_per_pixel;
  assert(full_size > 0);
  m_data = new uint8_t[full_size];
  m_depth = new float[m_width * m_height];
  m_builtins.resize(sizeof(Base::mat4_t) * 5);
}

const uint8_t*
Sisyphus::Render::Context::get_frame() const
{
  return m_data;
}

const unsigned int
Sisyphus::Render::Context::get_frame_size() const
{
  return m_width * m_height * m_bytes_per_pixel;
}

void
Sisyphus::Render::Context::resize(int width, int height, int bytes_per_pixel)
{
  size_t old_resolution = m_width * m_height;
  size_t old_size = old_resolution * (size_t)m_bytes_per_pixel;
  m_width = width;
  m_height = height;
  m_bytes_per_pixel = bytes_per_pixel;
  size_t cur_resolution = m_width * m_height;
  size_t full_size = cur_resolution * (size_t)m_bytes_per_pixel;
  assert(full_size >= 0);
  if (full_size != old_size)
  {
    if (m_data != nullptr)
    {
      delete[] m_data;
      m_data = nullptr;
    }
    if (full_size > 0)
    {
      m_data = new uint8_t[full_size];
    }
  }
  if (cur_resolution != old_resolution)
  {
    if (m_depth != nullptr)
    {
      delete[] m_depth;
      m_depth = nullptr;
    }
    if (cur_resolution > 0)
    {
      m_depth = new float[cur_resolution];
    }
  }
}

void
Sisyphus::Render::Context::set_viewport(float x_min, float y_min, float z_min, float x_max, float y_max, float z_max)
{
  m_viewport_min.x = x_min;
  m_viewport_min.y = y_min;
  m_viewport_min.z = z_min;

  m_viewport_max.x = x_max;
  m_viewport_max.y = y_max;
  m_viewport_max.z = z_max;
}

void
Sisyphus::Render::Context::set_descriptor_set(const std::vector<uint8_t>& descriptor_set)
{
  m_descriptor_set = descriptor_set;
}

void
Sisyphus::Render::Context::set_vertex_shader(Sisyphus::Render::VertexShaderFunc vsf)
{
  m_vsf = vsf;
}

void
Sisyphus::Render::Context::set_pixel_shader(Sisyphus::Render::PixelShaderFunc psf)
{
  m_psf = psf;
}

void
Sisyphus::Render::Context::set_model_matrix(const Base::mat4_t& m)
{
  m_model_matrix = m;
  m_model_view_matrix = m_view_matrix * m_model_matrix;
  m_transform_matrix = m_perspective_matrix * m_model_view_matrix;
  Base::replace_data(m_builtins, m_model_matrix, 0);
  Base::replace_data(m_builtins, m_model_view_matrix, sizeof(Base::mat4_t) * 3);
  Base::replace_data(m_builtins, m_transform_matrix, sizeof(Base::mat4_t) * 4);
}

void
Sisyphus::Render::Context::set_view_matrix(const Base::mat4_t& m)
{
  m_view_matrix = m;
  m_model_view_matrix = m_view_matrix * m_model_matrix;
  m_transform_matrix = m_perspective_matrix * m_model_view_matrix;
  Base::replace_data(m_builtins, m_view_matrix, sizeof(Base::mat4_t));
  Base::replace_data(m_builtins, m_model_view_matrix, sizeof(Base::mat4_t) * 3);
  Base::replace_data(m_builtins, m_transform_matrix, sizeof(Base::mat4_t) * 4);
}

void
Sisyphus::Render::Context::set_perspective_matrix(const Base::mat4_t& m)
{
  m_perspective_matrix = m;
  m_transform_matrix = m_perspective_matrix * m_model_view_matrix;
  Base::replace_data(m_builtins, m_perspective_matrix, sizeof(Base::mat4_t) * 2);
  Base::replace_data(m_builtins, m_transform_matrix, sizeof(Base::mat4_t) * 4);
}

void
Sisyphus::Render::Context::set_perspective(float fov, float aspect, float znear, float zfar)
{
  fov = fov * Sisyphus::Base::pi / 180.0f;
  Base::mat4_t perspective_matrix = Base::mat4_t::calculate_projection_matrix(fov, aspect, znear, zfar);
  this->set_frustum(fov, aspect, znear, zfar);
  this->set_perspective_matrix(perspective_matrix);
}

void
Sisyphus::Render::Context::set_frustum(float fov, float aspect, float znear, float zfar)
{
  // create 6 planes that forms frustum in view coordinates
  // znear plane
  Base::vec3_t& znerNormal = m_frustum.bounds[0].normal;
  znerNormal.x = 0.0f;
  znerNormal.y = 0.0f;
  znerNormal.z = -1.0f;
  m_frustum.bounds[0].offset = znear; // depends on direction of normal
  // zfar plane
  Base::vec3_t& zfarNormal = m_frustum.bounds[1].normal;
  zfarNormal.x = 0.0f;
  zfarNormal.y = 0.0f;
  zfarNormal.z = 1.0f;
  m_frustum.bounds[1].offset = -zfar;
  // points on planes - here we have world coordinates where y is going from up
  // to down
  Base::vec3_t b {0.0f, znear * tanf(0.5f * fov), znear};
  Base::vec3_t r {znear * tanf(0.5f * fov) * aspect, 0.0f, znear};
  Base::vec3_t t {0.0f, -b.y, znear};
  Base::vec3_t l {-r.x, 0.0f, znear};
  // top plane
  Base::vec3_t& topNormal = m_frustum.bounds[2].normal;
  topNormal.x = 0.0f;
  topNormal.y = -cosf(0.5f * fov);
  topNormal.z = -sinf(0.5f * fov);
  m_frustum.bounds[2].offset = -topNormal.calculate_dot_product(t);
  // bottom plane
  Base::vec3_t& bottomNormal = m_frustum.bounds[3].normal;
  bottomNormal.x = 0.0f;
  bottomNormal.y = -topNormal.y;
  bottomNormal.z = topNormal.z;
  m_frustum.bounds[3].offset = -bottomNormal.calculate_dot_product(b);
  // left plane
  float         horHalfAngle = atanf(aspect * b.y / znear);
  Base::vec3_t& leftNormal = m_frustum.bounds[4].normal;
  leftNormal.x = -cosf(horHalfAngle);
  leftNormal.y = 0.0f;
  leftNormal.z = -sinf(horHalfAngle);
  m_frustum.bounds[4].offset = -leftNormal.calculate_dot_product(l);
  // right plane
  Base::vec3_t& rightNormal = m_frustum.bounds[5].normal;
  rightNormal.x = -leftNormal.x;
  rightNormal.y = 0.0f;
  rightNormal.z = leftNormal.z;
  m_frustum.bounds[5].offset = -rightNormal.calculate_dot_product(r);
}

void
Sisyphus::Render::Context::put_pixel(int x, int y, const Base::vec4_t& color)
{
  // color - from 0 to 1 each component
  if (x < 0 || x >= m_width || y < 0 || y >= m_height)
  {
    return;
  }
  int pixelIndex = y * m_width * m_bytes_per_pixel + x * m_bytes_per_pixel;
  // please, take care about color content in advance - clamp if needed
  m_data[pixelIndex + 0] = (uint8_t)(color.b * 255.0f);
  m_data[pixelIndex + 1] = (uint8_t)(color.g * 255.0f);
  m_data[pixelIndex + 2] = (uint8_t)(color.r * 255.0f);
  m_data[pixelIndex + 3] = (uint8_t)(color.a * 255.0f);
}

void
Sisyphus::Render::Context::fill(const col4u_t& color)
{
  uint32_t colors = m_width * m_height * m_bytes_per_pixel;
  for (uint32_t i = 0; i < colors; i += m_bytes_per_pixel)
  { // fake m_bytes_per_pixel - now always 4 and maybe will always be 4
    m_data[i + 0] = color.b;
    m_data[i + 1] = color.g;
    m_data[i + 2] = color.r;
    m_data[i + 3] = color.a;
  }
}

Sisyphus::Base::vec4_t
Sisyphus::Render::Context::process_vertex(const Base::vec4_t& v)
{
  Base::vec4_t c = m_perspective_matrix * v;

  float w = c.w;
  c.w = 1.0f;
  c = c / w;

  Base::vec3_t crd {c.x, -c.y, c.z};
  crd = (crd + 1.0f) * 0.5f * (m_viewport_max - m_viewport_min) + m_viewport_min;

  return Base::vec4_t {crd.x, crd.y, crd.z, c.w};
}

void
Sisyphus::Render::Context::set_depth_test(bool flag)
{
  m_depth_test = flag;
}

void
Sisyphus::Render::Context::set_depth_write(bool flag)
{
  m_depth_write = flag;
}

void
Sisyphus::Render::Context::set_backface_culling(ECullingMode mode)
{
  m_backface_culling = mode;
}

void
Sisyphus::Render::Context::clear_depth(float val)
{
  for (int i = 0; i < m_width * m_height; i++)
  {
    m_depth[i] = val;
  }
}

void
Sisyphus::Render::Context::render_pixel_depth_wise(const Base::vec4_t& p, const uint8_t* data)
{
  int pix_flat_idx = ((int)p.y) * m_width + (int)p.x;
  if (pix_flat_idx >= 0 && pix_flat_idx < m_width * m_height)
  {
    if (m_depth_test)
    {
      if (p.z > m_depth[pix_flat_idx])
      {
        this->put_pixel(p.x, p.y, this->m_psf(p, data, this->m_builtins, this->m_descriptor_set));
      }
    }
    else
    {
      this->put_pixel(p.x, p.y, this->m_psf(p, data, this->m_builtins, this->m_descriptor_set));
    }
    if (m_depth_write)
    {
      if (p.z > m_depth[pix_flat_idx])
      {
        m_depth[pix_flat_idx] = p.z;
      }
    }
  }
}

static inline float
get_weight_between(float x, float y, float x0, float y0, float x1, float y1)
{
  float dx = x1 - x0;
  float dy = y1 - y0;
  if (fabs(dx) < fabs(dy))
  {
    return (y - y0) / (y1 - y0);
  }
  else
  {
    if (EQUAL_FLOATS(x0, x1))
    {
      return 0.0f;
    }
    return (x - x0) / (x1 - x0);
  }
}

static float
segment_plane_intersection(
  const Sisyphus::Base::vec3_t& a, const Sisyphus::Base::vec3_t& b, const Sisyphus::Render::Plane& p)
{
  const Sisyphus::Base::vec3_t ab = b - a;
  float                        k = (-p.normal.calculate_dot_product(a) - p.offset) / p.normal.calculate_dot_product(ab);
  return k;
}

static inline float
point_plane_side(const Sisyphus::Base::vec3_t& a, const Sisyphus::Render::Plane& p)
{
  // -value - under
  // 0 - on
  // +value - above
  // literally put this into the plane equation
  return p.normal.calculate_dot_product(a) + p.offset;
}

static void
cull_triangle_two_point_outside(
  const Sisyphus::Base::vec4_t& a, const Sisyphus::Base::vec4_t& b, const Sisyphus::Base::vec4_t& c,
  const uint8_t* data_a_ptr, const uint8_t* data_b_ptr, const uint8_t* data_c_ptr,
  const Sisyphus::Render::VertexFormat& vf, const Sisyphus::Render::Plane& p,
  std::vector<Sisyphus::Base::vec4_t>& passed_vertex_coords, std::vector<uint8_t>& passed_vertex_data)
{
  // a and b are outside
  float c_side = point_plane_side(c.xyz, p);
  if (fabs(c_side) <= Sisyphus::Base::eps)
  {
    // c is on the plane, discard
  }
  else
  {
    // two outside and one point is inside
    float                kA = segment_plane_intersection(c.xyz, a.xyz, p);
    float                kB = segment_plane_intersection(c.xyz, b.xyz, p);
    std::vector<uint8_t> dataAK = {};
    std::vector<uint8_t> data_bk = {};
    dataAK.resize(vf.size);
    data_bk.resize(vf.size);
    Sisyphus::Render::interpolate_attributes(data_c_ptr, data_a_ptr, dataAK.data(), kA, vf);
    Sisyphus::Render::interpolate_attributes(data_c_ptr, data_b_ptr, data_bk.data(), kB, vf);
    Sisyphus::Base::vec4_t ak = (a - c) * kA + c;
    ak.w = c.w;
    Sisyphus::Base::vec4_t bk = (b - c) * kB + c;
    bk.w = b.w;
    //
    passed_vertex_coords.emplace_back(c);
    passed_vertex_coords.emplace_back(ak);
    passed_vertex_coords.emplace_back(bk);
    Sisyphus::Base::append_data(passed_vertex_data, data_c_ptr, vf.size, 0);
    Sisyphus::Base::append_data(passed_vertex_data, dataAK.data(), vf.size, 0);
    Sisyphus::Base::append_data(passed_vertex_data, data_bk.data(), vf.size, 0);
  }
}

static void
cull_triangle_one_point_outside_one_on(
  const Sisyphus::Base::vec4_t& a, const Sisyphus::Base::vec4_t& b, const Sisyphus::Base::vec4_t& c,
  const uint8_t* data_a_ptr, const uint8_t* data_b_ptr, const uint8_t* data_c_ptr,
  const Sisyphus::Render::VertexFormat& vf, const Sisyphus::Render::Plane& p,
  std::vector<Sisyphus::Base::vec4_t>& passed_vertex_coords, std::vector<uint8_t>& passed_vertex_data)
{
  // a is outside and b is on the plane
  float c_side = point_plane_side(c.xyz, p);
  if (fabs(c_side) <= Sisyphus::Base::eps)
  {
    // only segment is on the plane, we can discard it
  }
  else
  {
    // a is outside, b is on the plane, c is inside
    float                kA = segment_plane_intersection(c.xyz, a.xyz, p);
    std::vector<uint8_t> data_ck = {};
    data_ck.resize(vf.size);
    Sisyphus::Render::interpolate_attributes(data_c_ptr, data_a_ptr, data_ck.data(), kA, vf);
    Sisyphus::Base::vec4_t ck = (a - c) * kA + c;
    ck.w = c.w;
    //
    passed_vertex_coords.emplace_back(b);
    passed_vertex_coords.emplace_back(c);
    passed_vertex_coords.emplace_back(ck);
    Sisyphus::Base::append_data(passed_vertex_data, data_b_ptr, vf.size, 0);
    Sisyphus::Base::append_data(passed_vertex_data, data_c_ptr, vf.size, 0);
    Sisyphus::Base::append_data(passed_vertex_data, data_ck.data(), vf.size, 0);
  }
}

static void
cull_triangle_one_point_outside(
  const Sisyphus::Base::vec4_t& a, const Sisyphus::Base::vec4_t& b, const Sisyphus::Base::vec4_t& c,
  const uint8_t* data_a_ptr, const uint8_t* data_b_ptr, const uint8_t* data_c_ptr,
  const Sisyphus::Render::VertexFormat& vf, const Sisyphus::Render::Plane& p,
  std::vector<Sisyphus::Base::vec4_t>& passed_vertex_coords, std::vector<uint8_t>& passed_vertex_data)
{
  // a-point is outside
  float b_side = point_plane_side(b.xyz, p);
  float c_side = point_plane_side(c.xyz, p);
  if (b_side > 0.0f)
  {
    // a and b are outside
    cull_triangle_two_point_outside(
      a, b, c, data_a_ptr, data_b_ptr, data_c_ptr, vf, p, passed_vertex_coords, passed_vertex_data);
  }
  else
  {
    if (c_side > 0.0f)
    {
      // a and c are outside
      cull_triangle_two_point_outside(
        a, c, b, data_a_ptr, data_c_ptr, data_b_ptr, vf, p, passed_vertex_coords, passed_vertex_data);
    }
    else
    {
      // only one point is really outside
      if (fabs(b_side) < Sisyphus::Base::eps)
      {
        // b is on the plane
        cull_triangle_one_point_outside_one_on(
          a, b, c, data_a_ptr, data_b_ptr, data_c_ptr, vf, p, passed_vertex_coords, passed_vertex_data);
      }
      else if (fabs(c_side) < Sisyphus::Base::eps)
      {
        // c is on the plane
        cull_triangle_one_point_outside_one_on(
          a, c, b, data_a_ptr, data_c_ptr, data_b_ptr, vf, p, passed_vertex_coords, passed_vertex_data);
      }
      else
      {
        // no point is on the plane, just one outside and two inside
        float                kB = segment_plane_intersection(b.xyz, a.xyz, p);
        float                kC = segment_plane_intersection(c.xyz, a.xyz, p);
        std::vector<uint8_t> data_bk = {};
        std::vector<uint8_t> data_ck = {};
        data_bk.resize(vf.size);
        data_ck.resize(vf.size);
        Sisyphus::Render::interpolate_attributes(data_b_ptr, data_a_ptr, data_bk.data(), kB, vf);
        Sisyphus::Render::interpolate_attributes(data_c_ptr, data_a_ptr, data_ck.data(), kC, vf);
        Sisyphus::Base::vec4_t bk = (a - b) * kB + b;
        bk.w = b.w;
        Sisyphus::Base::vec4_t ck = (a - c) * kC + c;
        ck.w = c.w;
        //
        passed_vertex_coords.emplace_back(b);
        passed_vertex_coords.emplace_back(c);
        passed_vertex_coords.emplace_back(bk);
        Sisyphus::Base::append_data(passed_vertex_data, data_b_ptr, vf.size, 0);
        Sisyphus::Base::append_data(passed_vertex_data, data_c_ptr, vf.size, 0);
        Sisyphus::Base::append_data(passed_vertex_data, data_bk.data(), vf.size, 0);
        //
        passed_vertex_coords.emplace_back(c);
        passed_vertex_coords.emplace_back(ck);
        passed_vertex_coords.emplace_back(bk);
        Sisyphus::Base::append_data(passed_vertex_data, data_c_ptr, vf.size, 0);
        Sisyphus::Base::append_data(passed_vertex_data, data_ck.data(), vf.size, 0);
        Sisyphus::Base::append_data(passed_vertex_data, data_bk.data(), vf.size, 0);
      }
    }
  }
}

static void
cull_triangle_by_plane(
  const Sisyphus::Base::vec4_t& a, const Sisyphus::Base::vec4_t& b, const Sisyphus::Base::vec4_t& c,
  const uint8_t* data_a_ptr, const uint8_t* data_b_ptr, const uint8_t* data_c_ptr,
  const Sisyphus::Render::VertexFormat& vf, const Sisyphus::Render::Plane& p,
  std::vector<Sisyphus::Base::vec4_t>& passed_vertex_coords, std::vector<uint8_t>& passed_vertex_data)
{
  float a_side = point_plane_side(a.xyz, p);
  float b_side = point_plane_side(b.xyz, p);
  float c_side = point_plane_side(c.xyz, p);
  // here we have many possible cases
  if (a_side <= 0.0f && b_side <= 0.0f && c_side <= 0.0f)
  {
    // everything is under the plane, add whole triangle
    passed_vertex_coords.emplace_back(a);
    passed_vertex_coords.emplace_back(b);
    passed_vertex_coords.emplace_back(c);
    Sisyphus::Base::append_data(passed_vertex_data, data_a_ptr, vf.size, 0);
    Sisyphus::Base::append_data(passed_vertex_data, data_b_ptr, vf.size, 0);
    Sisyphus::Base::append_data(passed_vertex_data, data_c_ptr, vf.size, 0);
  }
  else if (a_side > 0.0f && b_side > 0.0f && c_side > 0.0f)
  {
    // skip, whole triangle is above
  }
  else
  {
    // here is intersection
    if (a_side > 0.0f)
    {
      cull_triangle_one_point_outside(
        a, b, c, data_a_ptr, data_b_ptr, data_c_ptr, vf, p, passed_vertex_coords, passed_vertex_data);
    }
    else if (b_side > 0.0f)
    {
      cull_triangle_one_point_outside(
        b, a, c, data_b_ptr, data_a_ptr, data_c_ptr, vf, p, passed_vertex_coords, passed_vertex_data);
    }
    else if (c_side > 0.0f)
    {
      cull_triangle_one_point_outside(
        c, a, b, data_c_ptr, data_a_ptr, data_b_ptr, vf, p, passed_vertex_coords, passed_vertex_data);
    }
    else
    {
      // impossible to reach, at least one point should be outside
      assert(false);
    }
  }
}

static void
cull_triangles_by_plane(
  std::vector<Sisyphus::Base::vec4_t>& input_vertex_coords, const uint8_t* data_ptr,
  const Sisyphus::Render::VertexFormat& vf, const Sisyphus::Render::Plane& p,
  std::vector<Sisyphus::Base::vec4_t>& passed_vertex_coords, std::vector<uint8_t>& passed_vertex_data)
{
  for (int i = 0; i < input_vertex_coords.size(); i += 3)
  {
    const Sisyphus::Base::vec4_t& a = input_vertex_coords[i];
    const Sisyphus::Base::vec4_t& b = input_vertex_coords[i + 1];
    const Sisyphus::Base::vec4_t& c = input_vertex_coords[i + 2];
    const uint8_t*                data_a_ptr = data_ptr + i * vf.size;
    const uint8_t*                data_b_ptr = data_ptr + (i + 1) * vf.size;
    const uint8_t*                data_c_ptr = data_ptr + (i + 2) * vf.size;
    cull_triangle_by_plane(
      a, b, c, data_a_ptr, data_b_ptr, data_c_ptr, vf, p, passed_vertex_coords, passed_vertex_data);
  }
}

void
Sisyphus::Render::Context::cull_triangle_by_frustum(
  const Base::vec4_t& a, const Base::vec4_t& b, const Base::vec4_t& c, const uint8_t* data_a_ptr,
  const uint8_t* data_b_ptr, const uint8_t* data_c_ptr, const VertexFormat& vf,
  std::vector<Base::vec4_t>& passed_vertex_coords, std::vector<uint8_t>& passed_vertex_data)
{
  std::vector<Base::vec4_t> vertex_passed_front = {a, b, c};
  std::vector<uint8_t>      data_passed_front = {};
  Base::append_data(data_passed_front, data_a_ptr, vf.size, 0);
  Base::append_data(data_passed_front, data_b_ptr, vf.size, 0);
  Base::append_data(data_passed_front, data_c_ptr, vf.size, 0);
  std::vector<Base::vec4_t> vertex_passed_back = {};
  std::vector<uint8_t>      data_passed_back = {};

  std::vector<Base::vec4_t>* vertex_passed_front_ptr = &vertex_passed_front;
  std::vector<Base::vec4_t>* vertex_passed_back_ptr = &vertex_passed_back;
  std::vector<uint8_t>*      data_passed_front_ptr = &data_passed_front;
  std::vector<uint8_t>*      data_passed_back_ptr = &data_passed_back;

  for (int i = 0; i < 6; i++)
  {
    const Render::Plane& p = this->m_frustum.bounds[i];
    vertex_passed_back_ptr->clear();
    data_passed_back_ptr->clear();
    cull_triangles_by_plane(
      *vertex_passed_front_ptr, data_passed_front_ptr->data(), vf, p, *vertex_passed_back_ptr, *data_passed_back_ptr);
    std::swap(vertex_passed_front_ptr, vertex_passed_back_ptr);
    std::swap(data_passed_front_ptr, data_passed_back_ptr);
  }
  for (int i = 0; i < vertex_passed_front_ptr->size(); i++)
  {
    passed_vertex_coords.emplace_back((*vertex_passed_front_ptr)[i]);
    Sisyphus::Base::append_data(passed_vertex_data, data_passed_front_ptr->data() + (vf.size * i), vf.size, 0);
  }
}

static bool
cull_segment_by_plane(
  Sisyphus::Base::vec4_t& a_world, Sisyphus::Base::vec4_t& b_world, std::vector<uint8_t>& a_data,
  std::vector<uint8_t>& b_data, const Sisyphus::Render::VertexFormat& v_out_format, const Sisyphus::Render::Plane& p)
{
  float a_side = point_plane_side(a_world.xyz, p);
  float b_side = point_plane_side(b_world.xyz, p);
  if (
    (a_side > 0.0f && b_side > 0.0f) || (fabs(a_side) < Sisyphus::Base::eps && b_side > 0.0f) ||
    (fabs(b_side) < Sisyphus::Base::eps && a_side > 0.0f))
  {
    // outside of plane, totally clipped
    return false;
  }
  else
  {
    if (a_side > 0.0f || b_side > 0.0f)
    {
      std::vector<uint8_t> c_data(v_out_format.size);
      if (a_side > 0.0f)
      {
        // a is outside
        float                  k_ba = segment_plane_intersection(b_world.xyz, a_world.xyz, p);
        Sisyphus::Base::vec3_t c = b_world.xyz + (a_world.xyz - b_world.xyz) * k_ba;
        std::swap(a_world, b_world);
        b_world = {c.x, c.y, c.z, b_world.w};
        Sisyphus::Render::interpolate_attributes(b_data.data(), a_data.data(), c_data.data(), k_ba, v_out_format);
        a_data = c_data;
        std::swap(a_data, b_data);
      }
      else
      {
        // b is outside
        float                  k_ab = segment_plane_intersection(a_world.xyz, b_world.xyz, p);
        Sisyphus::Base::vec3_t c = a_world.xyz + (b_world.xyz - a_world.xyz) * k_ab;
        b_world = {c.x, c.y, c.z, b_world.w};
        Sisyphus::Render::interpolate_attributes(a_data.data(), b_data.data(), c_data.data(), k_ab, v_out_format);
        b_data = c_data;
      }
    }
  }
  return true;
}

bool
Sisyphus::Render::Context::cull_segment_by_frustum(
  Base::vec4_t& a_world, Base::vec4_t& b_world, std::vector<uint8_t>& a_data, std::vector<uint8_t>& b_data,
  const VertexFormat& v_out_format)
{
  for (int i = 0; i < 6; i++)
  {
    const Render::Plane& p = this->m_frustum.bounds[i];
    bool                 visible = cull_segment_by_plane(a_world, b_world, a_data, b_data, v_out_format, p);
    if (!visible)
    {
      return false;
    }
  }
  return true;
}

void
Sisyphus::Render::Context::draw_lines(
  const std::vector<Base::vec4_t>& coords, const std::vector<int>& indices, const uint8_t* vertex_data_ptr,
  const VertexFormat& v_in_format, const VertexFormat& v_out_format)
{
  if (indices.size() == 0 || indices.size() % 2 != 0)
  {
    return;
  }
  for (int i = 0; i < indices.size(); i += 2)
  {
    const Base::vec4_t& va = coords[indices[i]];
    const Base::vec4_t& vb = coords[indices[i + 1]];
    const uint8_t*      data_a_ptr = &vertex_data_ptr[indices[i] * v_in_format.size];
    const uint8_t*      data_b_ptr = &vertex_data_ptr[indices[i + 1] * v_in_format.size];
    // draw single line here
    Base::vec4_t         a_world, b_world;
    std::vector<uint8_t> a_vertex_out(v_out_format.size), b_vertex_out(v_out_format.size);
    this->m_vsf(va, a_world, a_vertex_out, data_a_ptr, this->m_builtins, this->m_descriptor_set);
    this->m_vsf(vb, b_world, b_vertex_out, data_b_ptr, this->m_builtins, this->m_descriptor_set);
    // frustum culling
    // only two possible separation cases - a outside or b outside
    // rewrite a_world, b_world, a_vertex_out, b_vertex_out
    bool visible = this->cull_segment_by_frustum(a_world, b_world, a_vertex_out, b_vertex_out, v_out_format);
    if (!visible)
    {
      continue;
    }
    //
    Base::vec4_t a = this->process_vertex(a_world);
    Base::vec4_t b = this->process_vertex(b_world);
    // obtained vertex shader results and go to the pixel stage
    Base::vec4_t a0(a);
    Base::vec4_t b0(b);
    // perspective correct interpolation part - normalize by z first
    std::vector<uint8_t> v_depthed_a(v_out_format.size), v_depthed_b(v_out_format.size);
    uint8_t *            depthed_a_ptr = v_depthed_a.data(), *depthed_b_ptr = v_depthed_b.data();
    multiply_attributes(a_vertex_out.data(), depthed_a_ptr, a0.w, v_out_format);
    multiply_attributes(b_vertex_out.data(), depthed_b_ptr, b0.w, v_out_format);
    //
    const uint8_t *data_a0_ptr = depthed_a_ptr, *data_b0_ptr = depthed_b_ptr;
    if (a.y > b.y)
    {
      std::swap(a0, b0);
      std::swap(data_a0_ptr, data_b0_ptr);
    }
    // a is bottom vertex and b is top vertex
    float y_dif = b0.y - a0.y;
    float x_dif = b0.x - a0.x;
    if (fabs(y_dif) < 0.001f && fabs(x_dif) < 0.001f)
    {
      // point
      this->render_pixel_depth_wise(a0, data_a0_ptr);
    }
    else
    {
      std::vector<uint8_t> pixel_out(v_out_format.size);
      if (fabs(y_dif) > fabs(x_dif))
      {
        float                slope = x_dif / y_dif;
        std::vector<uint8_t> c_out(v_out_format.size);
        Base::vec4_t         c;
        for (c.y = a0.y; c.y < b0.y; c.y += 1.0f)
        {
          c.x = a0.x + (c.y - a0.y) * slope;
          float weight = (c.y - a0.y) / (b0.y - a0.y);
          c.w = (b0.w - a0.w) * weight + a0.w;
          float pzo = 1.0f / c.w;
          c.z = (b0.z - a0.z) * weight + a0.z;
          interpolate_attributes(data_a0_ptr, data_b0_ptr, c_out.data(), weight, v_out_format);
          multiply_attributes(c_out.data(), pixel_out.data(), pzo, v_out_format);
          this->render_pixel_depth_wise(c, pixel_out.data());
        }
      }
      else
      {
        if (a0.x > b0.x)
        {
          std::swap(a0, b0);
          std::swap(data_a0_ptr, data_b0_ptr);
        }
        float                slope = y_dif / x_dif;
        std::vector<uint8_t> c_out(v_out_format.size);
        Base::vec4_t         c;
        for (c.x = a0.x; c.x < b0.x; c.x += 1.0f)
        {
          c.y = a0.y + (c.x - a0.x) * slope;
          float weight = (c.x - a0.x) / (b0.x - a0.x);
          c.w = (b0.w - a0.w) * weight + a0.w;
          float pzo = 1.0f / c.w;
          c.z = (b0.z - a0.z) * weight + a0.z;
          interpolate_attributes(data_a0_ptr, data_b0_ptr, c_out.data(), weight, v_out_format);
          multiply_attributes(c_out.data(), pixel_out.data(), pzo, v_out_format);
          this->render_pixel_depth_wise(c, pixel_out.data());
        }
      }
    }
  }
}

void
Sisyphus::Render::Context::draw_triangles(
  const std::vector<Base::vec4_t>& coords, const std::vector<int>& indices, const uint8_t* vertex_data_ptr,
  const VertexFormat& v_in_format, const VertexFormat& v_out_format)
{
  if (indices.size() == 0 || indices.size() % 3 != 0)
  {
    return;
  }
  int fragments = 0;
  for (int i = 0; i < indices.size(); i += 3)
  {
    const Base::vec4_t& va(coords[indices[i]]);
    const Base::vec4_t& vb(coords[indices[i + 1]]);
    const Base::vec4_t& vc(coords[indices[i + 2]]);
    const uint8_t*      data_a_ptr = &vertex_data_ptr[indices[i] * v_in_format.size];
    const uint8_t*      data_b_ptr = &vertex_data_ptr[indices[i + 1] * v_in_format.size];
    const uint8_t*      data_c_ptr = &vertex_data_ptr[indices[i + 2] * v_in_format.size];
    //
    Base::vec4_t         a_world, b_world, c_world;
    std::vector<uint8_t> a_vertex_out(v_out_format.size), b_vertex_out(v_out_format.size),
      c_vertex_out(v_out_format.size);
    // obtain output coordinates in view space and output vertex attributes
    this->m_vsf(va, a_world, a_vertex_out, data_a_ptr, this->m_builtins, this->m_descriptor_set);
    this->m_vsf(vb, b_world, b_vertex_out, data_b_ptr, this->m_builtins, this->m_descriptor_set);
    this->m_vsf(vc, c_world, c_vertex_out, data_c_ptr, this->m_builtins, this->m_descriptor_set);

    // backface culling
    if (m_backface_culling != ECullingMode::None)
    {
      Sisyphus::Base::vec4_t side0, side1, outside_normal;
      switch (m_backface_culling)
      {
      case ECullingMode::ClockWise:
        side0 = c_world - a_world;
        side1 = b_world - c_world;
        outside_normal = side0.calculate_cross_product(side1);
        break;
      case ECullingMode::CounterClockWise:
      default:
        side0 = b_world - a_world;
        side1 = c_world - b_world;
        outside_normal = side0.calculate_cross_product(side1);
        break;
      }
      Base::vec4_t z = a_world + b_world + c_world;
      float        zlength = a_world.calculate_magnitude();
      if (zlength < Sisyphus::Base::eps)
      {
        continue;
      }
      z = z * (1.0f / zlength);
      Base::vec3_t n {outside_normal.x, outside_normal.y, outside_normal.z};
      n = n.calculate_normalized();
      if (z.xyz.calculate_dot_product(n) > 0.0f)
      {
        continue;
      }
    }
    // frustum culling
    std::vector<uint8_t>      view_passed_vertex_data = {};
    std::vector<Base::vec4_t> view_passed_vertex_coords = {};
    this->cull_triangle_by_frustum(
      a_world, b_world, c_world, a_vertex_out.data(), b_vertex_out.data(), c_vertex_out.data(), v_out_format,
      view_passed_vertex_coords, view_passed_vertex_data);
    // rasterization
    for (int j = 0; j < view_passed_vertex_coords.size(); j += 3)
    {
      const Base::vec4_t& a_visible(view_passed_vertex_coords[j]);
      const Base::vec4_t& b_visible(view_passed_vertex_coords[j + 1]);
      const Base::vec4_t& c_visible(view_passed_vertex_coords[j + 2]);
      //
      uint8_t* vertex_out_a_ptr = &view_passed_vertex_data[j * v_out_format.size];
      uint8_t* vertex_out_b_ptr = &view_passed_vertex_data[(j + 1) * v_out_format.size];
      uint8_t* vertex_out_c_ptr = &view_passed_vertex_data[(j + 2) * v_out_format.size];
      //
      Base::vec4_t a, b, c;
      a = this->process_vertex(a_visible);
      b = this->process_vertex(b_visible);
      c = this->process_vertex(c_visible);
      //
      Base::vec4_t sa = a, sb = b, sc = c;
      if (sa.y > sc.y)
      {
        std::swap(sa, sc);
        std::swap(vertex_out_a_ptr, vertex_out_c_ptr);
      }
      if (sa.y > sb.y)
      {
        std::swap(sa, sb);
        std::swap(vertex_out_a_ptr, vertex_out_b_ptr);
      }
      if (sb.y > sc.y)
      {
        std::swap(sb, sc);
        std::swap(vertex_out_b_ptr, vertex_out_c_ptr);
      }
      // get interpolated values - line coordinates, only one component
      std::vector<float> xab = Base::interpolate(sa.y, sa.x, sb.y, sb.x);
      std::vector<float> xbc = Base::interpolate(sb.y, sb.x, sc.y, sc.x);
      std::vector<float> xac = Base::interpolate(sa.y, sa.x, sc.y, sc.x); // long side x
      int                nzeros = (int)(xab.size() == 0) + (int)(xbc.size() == 0) + (int)(xac.size() == 0);
      if (nzeros > 1)
      {
        return;
      }
      // here is the diffrence - we don't want to merge xab and xbc
      int  n = std::min(xac.size(), xab.size() + xbc.size());
      int  middle = n / 2;
      bool leftToRight = true;
      if (middle >= xab.size())
      {
        if (xac[middle] < xbc[middle - xab.size()])
        {
          leftToRight = false;
        }
      }
      else
      {
        if (xac[middle] < xab[middle])
        {
          leftToRight = false;
        }
      }
      float                bottomy = sa.y;
      int                  idx = 0;
      std::vector<uint8_t> v_interpolated_ac(v_out_format.size), v_interpolated_ab(v_out_format.size),
        v_interpolated_bc(v_out_format.size), v_interpolated_lr(v_out_format.size);
      std::vector<uint8_t> v_depthed_a(v_out_format.size), v_depthed_b(v_out_format.size),
        v_depthed_c(v_out_format.size), v_depthed_p(v_out_format.size);
      // divide attributes by original z - lesser attributes, that are located
      // further
      multiply_attributes(vertex_out_a_ptr, v_depthed_a.data(), sa.w, v_out_format);
      multiply_attributes(vertex_out_b_ptr, v_depthed_b.data(), sb.w, v_out_format);
      multiply_attributes(vertex_out_c_ptr, v_depthed_c.data(), sc.w, v_out_format);
      //
      if (leftToRight)
      {
        Base::vec4_t c;
        for (idx = 0; idx < xab.size() % (n + 1); idx++)
        {
          float leftx = xab[idx];
          float rightx = xac[idx];
          float v_weight_ab = get_weight_between(leftx, bottomy, sa.x, sa.y, sb.x, sb.y);
          float v_weight_ac = get_weight_between(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
          interpolate_attributes(
            v_depthed_a.data(), v_depthed_b.data(), v_interpolated_ab.data(), v_weight_ab, v_out_format);
          interpolate_attributes(
            v_depthed_a.data(), v_depthed_c.data(), v_interpolated_ac.data(), v_weight_ac, v_out_format);
          float lz = sa.z + (sb.z - sa.z) * v_weight_ab;
          float rz = sa.z + (sc.z - sa.z) * v_weight_ac;
          float lwo = sa.w + (sb.w - sa.w) * v_weight_ab;
          float rwo = sa.w + (sc.w - sa.w) * v_weight_ac;
          while (leftx < rightx)
          {
            float h_weight = (leftx - xab[idx]) / (xac[idx] - xab[idx]);
            interpolate_attributes(
              v_interpolated_ab.data(), v_interpolated_ac.data(), v_interpolated_lr.data(), h_weight, v_out_format);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * h_weight;
            float pwo = lwo + (rwo - lwo) * h_weight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiply_attributes(v_interpolated_lr.data(), v_depthed_p.data(), pzo, v_out_format);
            this->render_pixel_depth_wise(c, v_depthed_p.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
        for (; idx < n; idx++)
        {
          float leftx = xbc[idx - xab.size()];
          float rightx = xac[idx];
          float v_weight_bc = get_weight_between(leftx, bottomy, sb.x, sb.y, sc.x, sc.y);
          float v_weight_ac = get_weight_between(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
          interpolate_attributes(
            v_depthed_b.data(), v_depthed_c.data(), v_interpolated_bc.data(), v_weight_bc, v_out_format);
          interpolate_attributes(
            v_depthed_a.data(), v_depthed_c.data(), v_interpolated_ac.data(), v_weight_ac, v_out_format);
          float lz = sb.z + (sc.z - sb.z) * v_weight_bc;
          float rz = sa.z + (sc.z - sa.z) * v_weight_ac;
          float lwo = sb.w + (sc.w - sb.w) * v_weight_bc;
          float rwo = sa.w + (sc.w - sa.w) * v_weight_ac;
          while (leftx < rightx)
          {
            float h_weight = (leftx - xbc[idx - xab.size()]) / (xac[idx] - xbc[idx - xab.size()]);
            interpolate_attributes(
              v_interpolated_bc.data(), v_interpolated_ac.data(), v_interpolated_lr.data(), h_weight, v_out_format);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * h_weight;
            float pwo = lwo + (rwo - lwo) * h_weight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiply_attributes(v_interpolated_lr.data(), v_depthed_p.data(), pzo, v_out_format);
            this->render_pixel_depth_wise(c, v_depthed_p.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
      }
      else
      {
        Base::vec4_t c;
        for (idx = 0; idx < xab.size() % (n + 1); idx++)
        {
          float leftx = xac[idx];
          float rightx = xab[idx];
          float v_weight_ac = get_weight_between(leftx, bottomy, sa.x, sa.y, sc.x, sc.y);
          float v_weight_ab = get_weight_between(rightx, bottomy, sa.x, sa.y, sb.x, sb.y);
          interpolate_attributes(
            v_depthed_a.data(), v_depthed_b.data(), v_interpolated_ab.data(), v_weight_ab, v_out_format);
          interpolate_attributes(
            v_depthed_a.data(), v_depthed_c.data(), v_interpolated_ac.data(), v_weight_ac, v_out_format);
          float lz = sa.z + (sc.z - sa.z) * v_weight_ac;
          float rz = sa.z + (sb.z - sa.z) * v_weight_ab;
          float lwo = sa.w + (sc.w - sa.w) * v_weight_ac;
          float rwo = sa.w + (sb.w - sa.w) * v_weight_ab;
          while (leftx < rightx)
          {
            float h_weight = (leftx - xac[idx]) / (xab[idx] - xac[idx]);
            interpolate_attributes(
              v_interpolated_ac.data(), v_interpolated_ab.data(), v_interpolated_lr.data(), h_weight, v_out_format);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * h_weight;
            float pwo = lwo + (rwo - lwo) * h_weight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiply_attributes(v_interpolated_lr.data(), v_depthed_p.data(), pzo, v_out_format);
            this->render_pixel_depth_wise(c, v_depthed_p.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
        for (; idx < n; idx++)
        {
          float leftx = xac[idx];
          float rightx = xbc[idx - xab.size()];
          float v_weight_ac = get_weight_between(leftx, bottomy, sa.x, sa.y, sc.x, sc.y);
          float v_weight_bc = get_weight_between(rightx, bottomy, sb.x, sb.y, sc.x, sc.y);
          interpolate_attributes(
            v_depthed_b.data(), v_depthed_c.data(), v_interpolated_bc.data(), v_weight_bc, v_out_format);
          interpolate_attributes(
            v_depthed_a.data(), v_depthed_c.data(), v_interpolated_ac.data(), v_weight_ac, v_out_format);
          float lz = sa.z + (sc.z - sa.z) * v_weight_ac;
          float rz = sb.z + (sc.z - sb.z) * v_weight_bc;
          float lwo = sa.w + (sc.w - sa.w) * v_weight_ac;
          float rwo = sb.w + (sc.w - sb.w) * v_weight_bc;
          while (leftx < rightx)
          {
            float h_weight = (leftx - xac[idx]) / (xbc[idx - xab.size()] - xac[idx]);
            interpolate_attributes(
              v_interpolated_ac.data(), v_interpolated_bc.data(), v_interpolated_lr.data(), h_weight, v_out_format);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * h_weight;
            float pwo = lwo + (rwo - lwo) * h_weight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiply_attributes(v_interpolated_lr.data(), v_depthed_p.data(), pzo, v_out_format);
            this->render_pixel_depth_wise(c, v_depthed_p.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
      }
      fragments++;
    }
  }
  if (m_log != nullptr)
  {
    static char msg[128];
    int         iwr = snprintf(msg, 128, "triangles drawn: %d \n", fragments);
    m_log(msg, iwr);
  }
}

void
Sisyphus::Render::Context::set_log_func(LogFunc log)
{
  m_log = log;
}

Sisyphus::Render::Context::~Context()
{
  delete[] m_data;
  m_data = nullptr;
}
