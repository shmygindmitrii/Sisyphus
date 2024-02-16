#include "bonfire_render_context.h"
#include "base_utils.h"

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>

Temple::Bonfire::VertexFormat::VertexFormat(
  const std::vector<Temple::Bonfire::EVertexAttribType>& attribs)
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
interpolateAttribsTemplate(
  const uint8_t*& a_in,
  const uint8_t*& b_in,
  uint8_t*&       c_out,
  float           weight)
{
  *(T*)c_out = (*(T*)b_in) * weight - (*(T*)a_in) * weight + *(T*)a_in;
  a_in += sizeof(T);
  b_in += sizeof(T);
  c_out += sizeof(T);
}

void
Temple::Bonfire::interpolate_attributes(
  const uint8_t*      a_in,
  const uint8_t*      b_in,
  uint8_t*            c_out,
  float               weight,
  const VertexFormat& vf)
{
  // c_out should be enough to hold a_in or b_in (they are the same in terms of
  // size) weight is always from 0 to 1
  for (int i = 0; i < vf.attributes.size(); i++)
  {
    switch (vf.attributes[i])
    {
    case EVertexAttribType::FLOAT32:
      interpolateAttribsTemplate<float>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::INT32:
      interpolateAttribsTemplate<int32_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::UINT8:
      interpolateAttribsTemplate<uint8_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::VEC2:
      interpolateAttribsTemplate<Base::vec2_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::VEC3:
      interpolateAttribsTemplate<Base::vec3_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::VEC4:
      interpolateAttribsTemplate<Base::vec4_t>(a_in, b_in, c_out, weight);
      break;
    case EVertexAttribType::UV:
      interpolateAttribsTemplate<Base::vec2_t>(a_in, b_in, c_out, weight);
      break;
    }
  }
}

template <typename T>
static inline void
multAttribsTemplate(const uint8_t*& a_in, uint8_t*& c_out, float mult)
{
  *(T*)c_out = (*(T*)a_in) * mult;
  a_in += sizeof(T);
  c_out += sizeof(T);
}

void
Temple::Bonfire::multiply_attributes(
  const uint8_t*      a_in,
  uint8_t*            c_out,
  float               mult,
  const VertexFormat& vf)
{
  for (int i = 0; i < vf.attributes.size(); i++)
  {
    switch (vf.attributes[i])
    {
    case EVertexAttribType::FLOAT32:
      multAttribsTemplate<float>(a_in, c_out, mult);
      break;
    case EVertexAttribType::INT32:
      multAttribsTemplate<int32_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::UINT8:
      multAttribsTemplate<uint8_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::VEC2:
      multAttribsTemplate<Base::vec2_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::VEC3:
      multAttribsTemplate<Base::vec3_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::VEC4:
      multAttribsTemplate<Base::vec4_t>(a_in, c_out, mult);
      break;
    case EVertexAttribType::UV:
      multAttribsTemplate<Base::vec2_t>(a_in, c_out, mult);
      break;
    }
  }
}

Temple::Bonfire::Plane::Plane()
    : normal(Base::vec3_t {0.0f, 0.0f, 1.0f})
    , offset(0.0f)
{}

Temple::Bonfire::Plane::Plane(const Base::vec3_t& _normal, float _offset)
    : normal(_normal)
    , offset(_offset)
{}

Temple::Bonfire::RenderContext::RenderContext(
  int width,
  int height,
  int bytes_per_pixel)
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
Temple::Bonfire::RenderContext::get_frame() const
{
  return m_data;
}

void
Temple::Bonfire::RenderContext::resize(
  int width,
  int height,
  int bytes_per_pixel)
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
Temple::Bonfire::RenderContext::set_viewport(
  float x_min,
  float y_min,
  float z_min,
  float x_max,
  float y_max,
  float z_max)
{
  m_viewport_min.x = x_min;
  m_viewport_min.y = y_min;
  m_viewport_min.z = z_min;

  m_viewport_max.x = x_max;
  m_viewport_max.y = y_max;
  m_viewport_max.z = z_max;
}

void
Temple::Bonfire::RenderContext::set_descriptor_set(
  const std::vector<uint8_t>& descriptor_set)
{
  m_descriptor_set = descriptor_set;
}

void
Temple::Bonfire::RenderContext::set_vertex_shader(
  Temple::Bonfire::VertexShaderFunc vsf)
{
  m_vsf = vsf;
}

void
Temple::Bonfire::RenderContext::set_pixel_shader(
  Temple::Bonfire::PixelShaderFunc psf)
{
  m_psf = psf;
}

void
Temple::Bonfire::RenderContext::set_model_matrix(const Base::mat4_t& m)
{
  m_model_matrix = m;
  m_model_view_matrix = m_view_matrix * m_model_matrix;
  m_transform_matrix = m_perspective_matrix * m_model_view_matrix;
  Base::replace_data(m_builtins, m_model_matrix, 0);
  Base::replace_data(m_builtins, m_model_view_matrix, sizeof(Base::mat4_t) * 3);
  Base::replace_data(m_builtins, m_transform_matrix, sizeof(Base::mat4_t) * 4);
}

void
Temple::Bonfire::RenderContext::set_view_matrix(const Base::mat4_t& m)
{
  m_view_matrix = m;
  m_model_view_matrix = m_view_matrix * m_model_matrix;
  m_transform_matrix = m_perspective_matrix * m_model_view_matrix;
  Base::replace_data(m_builtins, m_view_matrix, sizeof(Base::mat4_t));
  Base::replace_data(m_builtins, m_model_view_matrix, sizeof(Base::mat4_t) * 3);
  Base::replace_data(m_builtins, m_transform_matrix, sizeof(Base::mat4_t) * 4);
}

void
Temple::Bonfire::RenderContext::set_perspective_matrix(const Base::mat4_t& m)
{
  m_perspective_matrix = m;
  m_transform_matrix = m_perspective_matrix * m_model_view_matrix;
  Base::replace_data(
    m_builtins,
    m_perspective_matrix,
    sizeof(Base::mat4_t) * 2);
  Base::replace_data(m_builtins, m_transform_matrix, sizeof(Base::mat4_t) * 4);
}

void
Temple::Bonfire::RenderContext::set_perspective(
  float fov,
  float aspect,
  float znear,
  float zfar)
{
  fov = fov * Temple::Base::pi / 180.0f;
  Base::mat4_t perspective_matrix =
    Base::mat4_t::calculate_projection_matrix(fov, aspect, znear, zfar);
  this->set_frustum(fov, aspect, znear, zfar);
  this->set_perspective_matrix(perspective_matrix);
}

void
Temple::Bonfire::RenderContext::set_frustum(
  float fov,
  float aspect,
  float znear,
  float zfar)
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
Temple::Bonfire::RenderContext::put_pixel(
  int                 x,
  int                 y,
  const Base::vec4_t& color)
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
Temple::Bonfire::RenderContext::fill(const col4u_t& color)
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

Temple::Base::vec4_t
Temple::Bonfire::RenderContext::process_vertex(const Base::vec4_t& v)
{
  Base::vec4_t c = m_perspective_matrix * v;

  float w = c.w;
  c.w = 1.0f;
  c = c / w;

  Base::vec3_t crd {c.x, -c.y, c.z};
  crd =
    (crd + 1.0f) * 0.5f * (m_viewport_max - m_viewport_min) + m_viewport_min;

  return Base::vec4_t {crd.x, crd.y, crd.z, c.w};
}

void
Temple::Bonfire::RenderContext::set_depth_test(bool flag)
{
  m_depth_test = flag;
}

void
Temple::Bonfire::RenderContext::set_depth_write(bool flag)
{
  m_depth_write = flag;
}

void
Temple::Bonfire::RenderContext::set_backface_culling(ECullingMode mode)
{
  m_backface_culling = mode;
}

void
Temple::Bonfire::RenderContext::clear_depth(float val)
{
  for (int i = 0; i < m_width * m_height; i++)
  {
    m_depth[i] = val;
  }
}

void
Temple::Bonfire::RenderContext::render_pixel_depth_wise(
  const Base::vec4_t& p,
  const uint8_t*      data)
{
  int pix_flat_idx = ((int)p.y) * m_width + (int)p.x;
  if (pix_flat_idx >= 0 && pix_flat_idx < m_width * m_height)
  {
    if (m_depth_test)
    {
      if (p.z > m_depth[pix_flat_idx])
      {
        this->put_pixel(
          p.x,
          p.y,
          this->m_psf(p, data, this->m_builtins, this->m_descriptor_set));
      }
    }
    else
    {
      this->put_pixel(
        p.x,
        p.y,
        this->m_psf(p, data, this->m_builtins, this->m_descriptor_set));
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
  const Temple::Base::vec3_t&   a,
  const Temple::Base::vec3_t&   b,
  const Temple::Bonfire::Plane& p)
{
  const Temple::Base::vec3_t ab = b - a;
  float k = (-p.normal.calculate_dot_product(a) - p.offset) /
            p.normal.calculate_dot_product(ab);
  return k;
}

static inline float
point_plane_side(const Temple::Base::vec3_t& a, const Temple::Bonfire::Plane& p)
{
  // -value - under
  // 0 - on
  // +value - above
  // literally put this into the plane equation
  return p.normal.calculate_dot_product(a) + p.offset;
}

static void
cull_triangle_two_point_outside(
  const Temple::Base::vec4_t&          a,
  const Temple::Base::vec4_t&          b,
  const Temple::Base::vec4_t&          c,
  const uint8_t*                       p_data_a,
  const uint8_t*                       p_data_b,
  const uint8_t*                       p_data_c,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passed_vertex_coords,
  std::vector<uint8_t>&                passed_vertex_data)
{
  // a and b are outside
  float c_side = point_plane_side(c.xyz, p);
  if (fabs(c_side) <= Temple::Base::eps)
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
    Temple::Bonfire::interpolate_attributes(
      p_data_c,
      p_data_a,
      dataAK.data(),
      kA,
      vf);
    Temple::Bonfire::interpolate_attributes(
      p_data_c,
      p_data_b,
      data_bk.data(),
      kB,
      vf);
    Temple::Base::vec4_t ak = (a - c) * kA + c;
    ak.w = c.w;
    Temple::Base::vec4_t bk = (b - c) * kB + c;
    bk.w = b.w;
    //
    passed_vertex_coords.emplace_back(c);
    passed_vertex_coords.emplace_back(ak);
    passed_vertex_coords.emplace_back(bk);
    Temple::Base::append_data(passed_vertex_data, p_data_c, vf.size, 0);
    Temple::Base::append_data(passed_vertex_data, dataAK.data(), vf.size, 0);
    Temple::Base::append_data(passed_vertex_data, data_bk.data(), vf.size, 0);
  }
}

static void
cull_triangle_one_point_outside_one_on(
  const Temple::Base::vec4_t&          a,
  const Temple::Base::vec4_t&          b,
  const Temple::Base::vec4_t&          c,
  const uint8_t*                       p_data_a,
  const uint8_t*                       p_data_b,
  const uint8_t*                       p_data_c,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passed_vertex_coords,
  std::vector<uint8_t>&                passed_vertex_data)
{
  // a is outside and b is on the plane
  float c_side = point_plane_side(c.xyz, p);
  if (fabs(c_side) <= Temple::Base::eps)
  {
    // only segment is on the plane, we can discard it
  }
  else
  {
    // a is outside, b is on the plane, c is inside
    float                kA = segment_plane_intersection(c.xyz, a.xyz, p);
    std::vector<uint8_t> data_ck = {};
    data_ck.resize(vf.size);
    Temple::Bonfire::interpolate_attributes(
      p_data_c,
      p_data_a,
      data_ck.data(),
      kA,
      vf);
    Temple::Base::vec4_t ck = (a - c) * kA + c;
    ck.w = c.w;
    //
    passed_vertex_coords.emplace_back(b);
    passed_vertex_coords.emplace_back(c);
    passed_vertex_coords.emplace_back(ck);
    Temple::Base::append_data(passed_vertex_data, p_data_b, vf.size, 0);
    Temple::Base::append_data(passed_vertex_data, p_data_c, vf.size, 0);
    Temple::Base::append_data(passed_vertex_data, data_ck.data(), vf.size, 0);
  }
}

static void
cull_triangle_one_point_outside(
  const Temple::Base::vec4_t&          a,
  const Temple::Base::vec4_t&          b,
  const Temple::Base::vec4_t&          c,
  const uint8_t*                       p_data_a,
  const uint8_t*                       p_data_b,
  const uint8_t*                       p_data_c,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passed_vertex_coords,
  std::vector<uint8_t>&                passed_vertex_data)
{
  // a-point is outside
  float b_side = point_plane_side(b.xyz, p);
  float c_side = point_plane_side(c.xyz, p);
  if (b_side > 0.0f)
  {
    // a and b are outside
    cull_triangle_two_point_outside(
      a,
      b,
      c,
      p_data_a,
      p_data_b,
      p_data_c,
      vf,
      p,
      passed_vertex_coords,
      passed_vertex_data);
  }
  else
  {
    if (c_side > 0.0f)
    {
      // a and c are outside
      cull_triangle_two_point_outside(
        a,
        c,
        b,
        p_data_a,
        p_data_c,
        p_data_b,
        vf,
        p,
        passed_vertex_coords,
        passed_vertex_data);
    }
    else
    {
      // only one point is really outside
      if (fabs(b_side) < Temple::Base::eps)
      {
        // b is on the plane
        cull_triangle_one_point_outside_one_on(
          a,
          b,
          c,
          p_data_a,
          p_data_b,
          p_data_c,
          vf,
          p,
          passed_vertex_coords,
          passed_vertex_data);
      }
      else if (fabs(c_side) < Temple::Base::eps)
      {
        // c is on the plane
        cull_triangle_one_point_outside_one_on(
          a,
          c,
          b,
          p_data_a,
          p_data_c,
          p_data_b,
          vf,
          p,
          passed_vertex_coords,
          passed_vertex_data);
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
        Temple::Bonfire::interpolate_attributes(
          p_data_b,
          p_data_a,
          data_bk.data(),
          kB,
          vf);
        Temple::Bonfire::interpolate_attributes(
          p_data_c,
          p_data_a,
          data_ck.data(),
          kC,
          vf);
        Temple::Base::vec4_t bk = (a - b) * kB + b;
        bk.w = b.w;
        Temple::Base::vec4_t ck = (a - c) * kC + c;
        ck.w = c.w;
        //
        passed_vertex_coords.emplace_back(b);
        passed_vertex_coords.emplace_back(c);
        passed_vertex_coords.emplace_back(bk);
        Temple::Base::append_data(passed_vertex_data, p_data_b, vf.size, 0);
        Temple::Base::append_data(passed_vertex_data, p_data_c, vf.size, 0);
        Temple::Base::append_data(
          passed_vertex_data,
          data_bk.data(),
          vf.size,
          0);
        //
        passed_vertex_coords.emplace_back(c);
        passed_vertex_coords.emplace_back(ck);
        passed_vertex_coords.emplace_back(bk);
        Temple::Base::append_data(passed_vertex_data, p_data_c, vf.size, 0);
        Temple::Base::append_data(
          passed_vertex_data,
          data_ck.data(),
          vf.size,
          0);
        Temple::Base::append_data(
          passed_vertex_data,
          data_bk.data(),
          vf.size,
          0);
      }
    }
  }
}

static void
cull_triangle_by_plane(
  const Temple::Base::vec4_t&          a,
  const Temple::Base::vec4_t&          b,
  const Temple::Base::vec4_t&          c,
  const uint8_t*                       p_data_a,
  const uint8_t*                       p_data_b,
  const uint8_t*                       p_data_c,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passed_vertex_coords,
  std::vector<uint8_t>&                passed_vertex_data)
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
    Temple::Base::append_data(passed_vertex_data, p_data_a, vf.size, 0);
    Temple::Base::append_data(passed_vertex_data, p_data_b, vf.size, 0);
    Temple::Base::append_data(passed_vertex_data, p_data_c, vf.size, 0);
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
        a,
        b,
        c,
        p_data_a,
        p_data_b,
        p_data_c,
        vf,
        p,
        passed_vertex_coords,
        passed_vertex_data);
    }
    else if (b_side > 0.0f)
    {
      cull_triangle_one_point_outside(
        b,
        a,
        c,
        p_data_b,
        p_data_a,
        p_data_c,
        vf,
        p,
        passed_vertex_coords,
        passed_vertex_data);
    }
    else if (c_side > 0.0f)
    {
      cull_triangle_one_point_outside(
        c,
        a,
        b,
        p_data_c,
        p_data_a,
        p_data_b,
        vf,
        p,
        passed_vertex_coords,
        passed_vertex_data);
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
  std::vector<Temple::Base::vec4_t>&   input_vertex_coords,
  const uint8_t*                       p_data,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passed_vertex_coords,
  std::vector<uint8_t>&                passed_vertex_data)
{
  for (int i = 0; i < input_vertex_coords.size(); i += 3)
  {
    const Temple::Base::vec4_t& a = input_vertex_coords[i];
    const Temple::Base::vec4_t& b = input_vertex_coords[i + 1];
    const Temple::Base::vec4_t& c = input_vertex_coords[i + 2];
    const uint8_t*              p_data_a = p_data + i * vf.size;
    const uint8_t*              p_data_b = p_data + (i + 1) * vf.size;
    const uint8_t*              p_data_c = p_data + (i + 2) * vf.size;
    cull_triangle_by_plane(
      a,
      b,
      c,
      p_data_a,
      p_data_b,
      p_data_c,
      vf,
      p,
      passed_vertex_coords,
      passed_vertex_data);
  }
}

void
Temple::Bonfire::RenderContext::cull_triangle_by_frustum(
  const Base::vec4_t&        a,
  const Base::vec4_t&        b,
  const Base::vec4_t&        c,
  const uint8_t*             p_data_a,
  const uint8_t*             p_data_b,
  const uint8_t*             p_data_c,
  const VertexFormat&        vf,
  std::vector<Base::vec4_t>& passed_vertex_coords,
  std::vector<uint8_t>&      passed_vertex_data)
{
  std::vector<Base::vec4_t> vertex_passed_front = {a, b, c};
  std::vector<uint8_t>      data_passed_front = {};
  Base::append_data(data_passed_front, p_data_a, vf.size, 0);
  Base::append_data(data_passed_front, p_data_b, vf.size, 0);
  Base::append_data(data_passed_front, p_data_c, vf.size, 0);
  std::vector<Base::vec4_t> vertex_passed_back = {};
  std::vector<uint8_t>      data_passed_back = {};

  std::vector<Base::vec4_t>* p_vertex_passed_front = &vertex_passed_front;
  std::vector<Base::vec4_t>* p_vertex_passed_back = &vertex_passed_back;
  std::vector<uint8_t>*      p_data_passed_front = &data_passed_front;
  std::vector<uint8_t>*      p_data_passed_back = &data_passed_back;

  for (int i = 0; i < 6; i++)
  {
    const Bonfire::Plane& p = this->m_frustum.bounds[i];
    p_vertex_passed_back->clear();
    p_data_passed_back->clear();
    cull_triangles_by_plane(
      *p_vertex_passed_front,
      p_data_passed_front->data(),
      vf,
      p,
      *p_vertex_passed_back,
      *p_data_passed_back);
    std::swap(p_vertex_passed_front, p_vertex_passed_back);
    std::swap(p_data_passed_front, p_data_passed_back);
  }
  for (int i = 0; i < p_vertex_passed_front->size(); i++)
  {
    passed_vertex_coords.emplace_back((*p_vertex_passed_front)[i]);
    Temple::Base::append_data(
      passed_vertex_data,
      p_data_passed_front->data() + (vf.size * i),
      vf.size,
      0);
  }
}

void
Temple::Bonfire::RenderContext::draw_lines(
  const std::vector<Base::vec4_t>& coords,
  const std::vector<int>&          indices,
  const uint8_t*                   p_vertex_data,
  const VertexFormat&              v_in_format,
  const VertexFormat&              v_out_format)
{
  if (indices.size() == 0 || indices.size() % 2 != 0)
  {
    return;
  }
  for (int i = 0; i < indices.size(); i += 2)
  {
    const Base::vec4_t& va = coords[indices[i]];
    const Base::vec4_t& vb = coords[indices[i + 1]];
    const uint8_t* p_data_a = &p_vertex_data[indices[i] * v_in_format.size];
    const uint8_t* p_data_b = &p_vertex_data[indices[i + 1] * v_in_format.size];
    // draw single line here
    Base::vec4_t         a, b;
    std::vector<uint8_t> a_vertex_out(v_out_format.size),
      b_vertex_out(v_out_format.size);
    this->m_vsf(
      va,
      a,
      a_vertex_out,
      p_data_a,
      this->m_builtins,
      this->m_descriptor_set);
    this->m_vsf(
      vb,
      b,
      b_vertex_out,
      p_data_b,
      this->m_builtins,
      this->m_descriptor_set);
    //
    a = process_vertex(a);
    b = process_vertex(b);
    // obtained vertex shader results and go to the pixel stage
    Base::vec4_t a0(a);
    Base::vec4_t b0(b);
    // perspective correct interpolation part - normalize by z first
    std::vector<uint8_t> v_depthed_a(v_out_format.size),
      v_depthed_b(v_out_format.size);
    uint8_t *p_depthed_a = v_depthed_a.data(),
            *p_depthed_b = v_depthed_b.data();
    multiply_attributes(a_vertex_out.data(), p_depthed_a, a0.w, v_out_format);
    multiply_attributes(b_vertex_out.data(), p_depthed_b, b0.w, v_out_format);
    //
    const uint8_t *p_data_a0 = p_depthed_a, *p_data_b0 = p_depthed_b;
    if (a.y > b.y)
    {
      std::swap(a0, b0);
      std::swap(p_data_a0, p_data_b0);
    }
    // a is bottom vertex and b is top vertex
    float y_dif = b0.y - a0.y;
    float x_dif = b0.x - a0.x;
    if (fabs(y_dif) < 0.001f && fabs(x_dif) < 0.001f)
    {
      // point
      render_pixel_depth_wise(a0, p_data_a0);
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
          interpolate_attributes(
            p_data_a0,
            p_data_b0,
            c_out.data(),
            weight,
            v_out_format);
          multiply_attributes(
            c_out.data(),
            pixel_out.data(),
            pzo,
            v_out_format);
          render_pixel_depth_wise(c, pixel_out.data());
        }
      }
      else
      {
        if (a0.x > b0.x)
        {
          std::swap(a0, b0);
          std::swap(p_data_a0, p_data_b0);
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
          interpolate_attributes(
            p_data_a0,
            p_data_b0,
            c_out.data(),
            weight,
            v_out_format);
          multiply_attributes(
            c_out.data(),
            pixel_out.data(),
            pzo,
            v_out_format);
          render_pixel_depth_wise(c, pixel_out.data());
        }
      }
    }
  }
}

void
Temple::Bonfire::RenderContext::draw_triangles(
  const std::vector<Base::vec4_t>& coords,
  const std::vector<int>&          indices,
  const uint8_t*                   p_vertex_data,
  const VertexFormat&              v_in_format,
  const VertexFormat&              v_out_format)
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
    const uint8_t* p_data_a = &p_vertex_data[indices[i] * v_in_format.size];
    const uint8_t* p_data_b = &p_vertex_data[indices[i + 1] * v_in_format.size];
    const uint8_t* p_data_c = &p_vertex_data[indices[i + 2] * v_in_format.size];
    //
    Base::vec4_t         a_world, b_world, c_world;
    std::vector<uint8_t> a_vertex_out(v_out_format.size),
      b_vertex_out(v_out_format.size), c_vertex_out(v_out_format.size);

    this->m_vsf(
      va,
      a_world,
      a_vertex_out,
      p_data_a,
      this->m_builtins,
      this->m_descriptor_set);
    this->m_vsf(
      vb,
      b_world,
      b_vertex_out,
      p_data_b,
      this->m_builtins,
      this->m_descriptor_set);
    this->m_vsf(
      vc,
      c_world,
      c_vertex_out,
      p_data_c,
      this->m_builtins,
      this->m_descriptor_set);

    // backface culling
    if (m_backface_culling != ECullingMode::None)
    {
      Temple::Base::vec4_t side0, side1, outside_normal;
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
      float        zlength = z.calculate_magnitude();
      if (zlength < Temple::Base::eps)
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
      a_world,
      b_world,
      c_world,
      a_vertex_out.data(),
      b_vertex_out.data(),
      c_vertex_out.data(),
      v_out_format,
      view_passed_vertex_coords,
      view_passed_vertex_data);
    // rasterization
    for (int j = 0; j < view_passed_vertex_coords.size(); j += 3)
    {
      const Base::vec4_t& a_visible(view_passed_vertex_coords[j]);
      const Base::vec4_t& b_visible(view_passed_vertex_coords[j + 1]);
      const Base::vec4_t& c_visible(view_passed_vertex_coords[j + 2]);
      //
      uint8_t* p_vertex_out_a = &view_passed_vertex_data[j * v_out_format.size];
      uint8_t* p_vertex_out_b =
        &view_passed_vertex_data[(j + 1) * v_out_format.size];
      uint8_t* p_vertex_out_c =
        &view_passed_vertex_data[(j + 2) * v_out_format.size];
      //
      Base::vec4_t a, b, c;
      a = process_vertex(a_visible);
      b = process_vertex(b_visible);
      c = process_vertex(c_visible);
      //
      Base::vec4_t sa = a, sb = b, sc = c;
      if (sa.y > sc.y)
      {
        std::swap(sa, sc);
        std::swap(p_vertex_out_a, p_vertex_out_c);
      }
      if (sa.y > sb.y)
      {
        std::swap(sa, sb);
        std::swap(p_vertex_out_a, p_vertex_out_b);
      }
      if (sb.y > sc.y)
      {
        std::swap(sb, sc);
        std::swap(p_vertex_out_b, p_vertex_out_c);
      }
      // get interpolated values - line coordinates, only one component
      std::vector<float> xab = Base::interpolate(sa.y, sa.x, sb.y, sb.x);
      std::vector<float> xbc = Base::interpolate(sb.y, sb.x, sc.y, sc.x);
      std::vector<float> xac =
        Base::interpolate(sa.y, sa.x, sc.y, sc.x); // long side x
      int nzeros = (int)(xab.size() == 0) + (int)(xbc.size() == 0) +
                   (int)(xac.size() == 0);
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
      std::vector<uint8_t> v_interpolated_ac(v_out_format.size),
        v_interpolated_ab(v_out_format.size),
        v_interpolated_bc(v_out_format.size),
        v_interpolated_lr(v_out_format.size);
      std::vector<uint8_t> v_depthed_a(v_out_format.size),
        v_depthed_b(v_out_format.size), v_depthed_c(v_out_format.size),
        v_depthed_p(v_out_format.size);
      // divide attributes by original z - lesser attributes, that are located
      // further
      multiply_attributes(
        p_vertex_out_a,
        v_depthed_a.data(),
        sa.w,
        v_out_format);
      multiply_attributes(
        p_vertex_out_b,
        v_depthed_b.data(),
        sb.w,
        v_out_format);
      multiply_attributes(
        p_vertex_out_c,
        v_depthed_c.data(),
        sc.w,
        v_out_format);
      //
      if (leftToRight)
      {
        Base::vec4_t c;
        for (idx = 0; idx < xab.size() % (n + 1); idx++)
        {
          float leftx = xab[idx];
          float rightx = xac[idx];
          float v_weight_ab =
            get_weight_between(leftx, bottomy, sa.x, sa.y, sb.x, sb.y);
          float v_weight_ac =
            get_weight_between(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
          interpolate_attributes(
            v_depthed_a.data(),
            v_depthed_b.data(),
            v_interpolated_ab.data(),
            v_weight_ab,
            v_out_format);
          interpolate_attributes(
            v_depthed_a.data(),
            v_depthed_c.data(),
            v_interpolated_ac.data(),
            v_weight_ac,
            v_out_format);
          float lz = sa.z + (sb.z - sa.z) * v_weight_ab;
          float rz = sa.z + (sc.z - sa.z) * v_weight_ac;
          float lwo = sa.w + (sb.w - sa.w) * v_weight_ab;
          float rwo = sa.w + (sc.w - sa.w) * v_weight_ac;
          while (leftx < rightx)
          {
            float h_weight = (leftx - xab[idx]) / (xac[idx] - xab[idx]);
            interpolate_attributes(
              v_interpolated_ab.data(),
              v_interpolated_ac.data(),
              v_interpolated_lr.data(),
              h_weight,
              v_out_format);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * h_weight;
            float pwo = lwo + (rwo - lwo) * h_weight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiply_attributes(
              v_interpolated_lr.data(),
              v_depthed_p.data(),
              pzo,
              v_out_format);
            render_pixel_depth_wise(c, v_depthed_p.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
        for (; idx < n; idx++)
        {
          float leftx = xbc[idx - xab.size()];
          float rightx = xac[idx];
          float v_weight_bc =
            get_weight_between(leftx, bottomy, sb.x, sb.y, sc.x, sc.y);
          float v_weight_ac =
            get_weight_between(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
          interpolate_attributes(
            v_depthed_b.data(),
            v_depthed_c.data(),
            v_interpolated_bc.data(),
            v_weight_bc,
            v_out_format);
          interpolate_attributes(
            v_depthed_a.data(),
            v_depthed_c.data(),
            v_interpolated_ac.data(),
            v_weight_ac,
            v_out_format);
          float lz = sb.z + (sc.z - sb.z) * v_weight_bc;
          float rz = sa.z + (sc.z - sa.z) * v_weight_ac;
          float lwo = sb.w + (sc.w - sb.w) * v_weight_bc;
          float rwo = sa.w + (sc.w - sa.w) * v_weight_ac;
          while (leftx < rightx)
          {
            float h_weight = (leftx - xbc[idx - xab.size()]) /
                             (xac[idx] - xbc[idx - xab.size()]);
            interpolate_attributes(
              v_interpolated_bc.data(),
              v_interpolated_ac.data(),
              v_interpolated_lr.data(),
              h_weight,
              v_out_format);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * h_weight;
            float pwo = lwo + (rwo - lwo) * h_weight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiply_attributes(
              v_interpolated_lr.data(),
              v_depthed_p.data(),
              pzo,
              v_out_format);
            render_pixel_depth_wise(c, v_depthed_p.data());
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
          float v_weight_ac =
            get_weight_between(leftx, bottomy, sa.x, sa.y, sc.x, sc.y);
          float v_weight_ab =
            get_weight_between(rightx, bottomy, sa.x, sa.y, sb.x, sb.y);
          interpolate_attributes(
            v_depthed_a.data(),
            v_depthed_b.data(),
            v_interpolated_ab.data(),
            v_weight_ab,
            v_out_format);
          interpolate_attributes(
            v_depthed_a.data(),
            v_depthed_c.data(),
            v_interpolated_ac.data(),
            v_weight_ac,
            v_out_format);
          float lz = sa.z + (sc.z - sa.z) * v_weight_ac;
          float rz = sa.z + (sb.z - sa.z) * v_weight_ab;
          float lwo = sa.w + (sc.w - sa.w) * v_weight_ac;
          float rwo = sa.w + (sb.w - sa.w) * v_weight_ab;
          while (leftx < rightx)
          {
            float h_weight = (leftx - xac[idx]) / (xab[idx] - xac[idx]);
            interpolate_attributes(
              v_interpolated_ac.data(),
              v_interpolated_ab.data(),
              v_interpolated_lr.data(),
              h_weight,
              v_out_format);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * h_weight;
            float pwo = lwo + (rwo - lwo) * h_weight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiply_attributes(
              v_interpolated_lr.data(),
              v_depthed_p.data(),
              pzo,
              v_out_format);
            render_pixel_depth_wise(c, v_depthed_p.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
        for (; idx < n; idx++)
        {
          float leftx = xac[idx];
          float rightx = xbc[idx - xab.size()];
          float v_weight_ac =
            get_weight_between(leftx, bottomy, sa.x, sa.y, sc.x, sc.y);
          float v_weight_bc =
            get_weight_between(rightx, bottomy, sb.x, sb.y, sc.x, sc.y);
          interpolate_attributes(
            v_depthed_b.data(),
            v_depthed_c.data(),
            v_interpolated_bc.data(),
            v_weight_bc,
            v_out_format);
          interpolate_attributes(
            v_depthed_a.data(),
            v_depthed_c.data(),
            v_interpolated_ac.data(),
            v_weight_ac,
            v_out_format);
          float lz = sa.z + (sc.z - sa.z) * v_weight_ac;
          float rz = sb.z + (sc.z - sb.z) * v_weight_bc;
          float lwo = sa.w + (sc.w - sa.w) * v_weight_ac;
          float rwo = sb.w + (sc.w - sb.w) * v_weight_bc;
          while (leftx < rightx)
          {
            float h_weight =
              (leftx - xac[idx]) / (xbc[idx - xab.size()] - xac[idx]);
            interpolate_attributes(
              v_interpolated_ac.data(),
              v_interpolated_bc.data(),
              v_interpolated_lr.data(),
              h_weight,
              v_out_format);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * h_weight;
            float pwo = lwo + (rwo - lwo) * h_weight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiply_attributes(
              v_interpolated_lr.data(),
              v_depthed_p.data(),
              pzo,
              v_out_format);
            render_pixel_depth_wise(c, v_depthed_p.data());
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
Temple::Bonfire::RenderContext::set_log_func(LogFunc log)
{
  m_log = log;
}

Temple::Bonfire::RenderContext::~RenderContext()
{
  delete[] m_data;
  m_data = nullptr;
}
