#pragma once

#include <cstdint>
#include "bonfire_color.h"
#include "base_vectors.h"
#include "base_matrices.h"

namespace Temple
{
namespace Bonfire
{
  enum class EPrimitiveType {
    LINE,
    TRIANGLE,
  };
  enum class EVertexAttribType {
    FLOAT32,
    INT32,
    UINT8,
    VEC2,
    VEC3,
    VEC4,
    UV,
  };
  enum class ECullingMode {
    None,
    ClockWise,
    CounterClockWise
  };
  struct VertexFormat {
    size_t size;
    std::vector<EVertexAttribType>
      attributes; // position is always in the beginning
    VertexFormat(const std::vector<EVertexAttribType>& attribs);
  };
  void
  interpolate_attributes(
    const uint8_t*      aIn,
    const uint8_t*      bIn,
    uint8_t*            cOut,
    float               weight,
    const VertexFormat& vf);
  void
  multiply_attributes(
    const uint8_t*      aIn,
    uint8_t*            cOut,
    float               mult,
    const VertexFormat& vf);
  // perVertexData use only inside vertex shader; do not pass further and no
  // need to interpolate everything needed for pixel shader put to perVertexOut
  // - this data will be interpolated
  using VertexShaderFunc = void (*)(
    const Base::vec4_t&         input,
    Base::vec4_t&               output,
    std::vector<uint8_t>&       per_vertex_out,
    const uint8_t*              per_vertex_data,
    const std::vector<uint8_t>& builtins,
    const std::vector<uint8_t>& descriptor_set); // over single vertex
  using PixelShaderFunc = Base::vec4_t (*)(
    const Base::vec4_t&         input,
    const uint8_t*              per_pixel_np,
    const std::vector<uint8_t>& builtins,
    const std::vector<uint8_t>& descriptor_set); // over single pixel
  //
  using LogFunc = void (*)(const char* msg, unsigned int msg_length);
  //
  struct Plane {
    Base::vec3_t normal;
    float        offset;
    Plane();
    Plane(const Base::vec3_t& _normal, float _offset);
  };
  struct Frustum {
    Plane bounds[6];
  };
  //
  class RenderContext {
  private:
    uint8_t*             m_data = nullptr;
    float*               m_depth = nullptr;
    int                  m_width = 0;
    int                  m_height = 0;
    int                  m_bytes_per_pixel = 0;
    Base::vec3_t         m_viewport_min = {0.0f, 0.0f, 0.0f};
    Base::vec3_t         m_viewport_max = {0.0f, 0.0f, 0.0f};
    std::vector<uint8_t> m_descriptor_set;
    VertexShaderFunc     m_vsf = nullptr;
    PixelShaderFunc      m_psf = nullptr;
    bool                 m_depth_write = true;
    bool                 m_depth_test = true;
    ECullingMode         m_backface_culling = ECullingMode::None;
    Base::mat4_t         m_model_matrix = Base::mat4_t::get_identity_matrix();
    Base::mat4_t         m_view_matrix = Base::mat4_t::get_identity_matrix();
    Base::mat4_t m_perspective_matrix = Base::mat4_t::get_identity_matrix();
    Base::mat4_t m_model_view_matrix = Base::mat4_t::get_identity_matrix();
    Base::mat4_t m_transform_matrix = Base::mat4_t::get_identity_matrix();
    std::vector<uint8_t> m_builtins; // default matrices - immediate mode
    //
    Frustum m_frustum;
    //
    LogFunc m_log = nullptr;

  public:
    RenderContext(int width, int height, int bytes_per_pixel);
    const uint8_t*
    get_frame() const;
    const unsigned int
    get_frame_size() const;
    void
    resize(int width, int height, int bytes_per_pixel);
    void
    set_viewport(
      float x_min,
      float y_min,
      float z_min,
      float x_max,
      float y_max,
      float z_max);
    void
    set_descriptor_set(const std::vector<uint8_t>& descriptor_set);
    void
    set_vertex_shader(VertexShaderFunc vsf);
    void
    set_pixel_shader(PixelShaderFunc psf);
    void
    set_model_matrix(const Base::mat4_t& m);
    void
    set_view_matrix(const Base::mat4_t& m);
    void
    set_perspective_matrix(const Base::mat4_t& m);
    void
    set_perspective(float fov, float aspect, float znear, float zfar);
    void
    set_frustum(float fov, float aspect, float znear, float zfar);
    void
    put_pixel(int x, int y, const Base::vec4_t& color);
    void
    render_pixel_depth_wise(const Base::vec4_t& p, const uint8_t* data);
    Base::vec4_t
    process_vertex(const Base::vec4_t& v);
    void
    fill(const col4u_t& color);
    void
    cull_triangle_by_frustum(
      const Base::vec4_t&        a,
      const Base::vec4_t&        b,
      const Base::vec4_t&        c,
      const uint8_t*             a_data,
      const uint8_t*             b_data,
      const uint8_t*             c_data,
      const VertexFormat&        vf,
      std::vector<Base::vec4_t>& passed_vertex_coords,
      std::vector<uint8_t>&      passed_vertex_data);
    void
    set_depth_test(bool flag);
    void
    set_depth_write(bool flag);
    void
    set_backface_culling(ECullingMode mode);
    void
    clear_depth(float val);
    void
    draw_lines(
      const std::vector<Base::vec4_t>& coords,
      const std::vector<int>&          indices,
      const uint8_t*                   vertex_data,
      const VertexFormat&              v_in_format,
      const VertexFormat&              v_out_Format);
    void
    draw_triangles(
      const std::vector<Base::vec4_t>& coords,
      const std::vector<int>&          indices,
      const uint8_t*                   vertex_data,
      const VertexFormat&              v_in_format,
      const VertexFormat&              v_out_format);
    //
    void
    set_log_func(LogFunc log);
    //
    ~RenderContext();
  };
} // namespace Bonfire
} // namespace Temple
