#include <vector>

#include "app.h"
#include "render_color.h"
#include "render_context.h"
#include "render_texture_holder.h"
#include "win_tex_loader.h"
#include "obj_file.h"

using namespace Sisyphus;

static Render::col4u_t              s_red {255, 0, 0, 255};
static Render::col4u_t              s_green {0, 255, 0, 255};
static Render::col4u_t              s_blue {0, 0, 255, 255};
static Render::col4u_t              s_yellow {200, 200, 0, 255};
static Render::col4u_t              s_pink {200, 154, 165, 255};
static Render::col4u_t              s_turquoise {0, 154, 154, 255};
static Render::col4u_t              s_orange {205, 88, 0, 255};
static Render::col4u_t              s_violet {107, 0, 215, 255};
static std::vector<Render::col4u_t> s_default_colors_u = {s_red,  s_green,     s_blue,   s_yellow,
                                                          s_pink, s_turquoise, s_orange, s_violet};
static std::vector<Base::vec4_t>    s_default_colors = {
    Render::get_color_vec4(s_red),    Render::get_color_vec4(s_green), Render::get_color_vec4(s_blue),
    Render::get_color_vec4(s_yellow), Render::get_color_vec4(s_pink),  Render::get_color_vec4(s_turquoise),
    Render::get_color_vec4(s_orange), Render::get_color_vec4(s_violet)};
static Render::col4u_t s_bg_color {15, 15, 35, 255};
static Render::col4u_t s_line_color {0, 150, 0, 255};

static Render::Context s_render_context(1, 1, 4);

static std::vector<Base::vec4_t> s_abc = {};
static std::vector<int>          s_abc_line_indices = {0, 1, 1, 2, 2, 0};
static std::vector<int>          s_abc_triangle_indices = {2, 1, 0};
static std::vector<uint8_t>      s_abc_triangle_attribs = {};

std::vector<Base::vec4_t> s_model_verts;
std::vector<int>          s_model_inds;
std::vector<uint8_t>      s_model_vertex_attribs;

static Render::VertexFormat s_vertex_output_format({
    Render::EVertexAttribType::VEC4, // position
    Render::EVertexAttribType::VEC4, // color
    Render::EVertexAttribType::VEC2, // tex
    Render::EVertexAttribType::VEC3, // normal
});

static Render::VertexFormat s_vertex_input_format({
    Render::EVertexAttribType::VEC4, // color
    Render::EVertexAttribType::VEC2, // tex
    Render::EVertexAttribType::VEC3, // normal
});

extern "C"
{
void
sisyphus_application_init(void* data)
{
    // sample triangle for debuggin purposes
    Base::vec4_t a {-0.0f, -0.0f, +0.2f, +1.0f};
    Base::vec4_t b {-0.7f, -0.7f, +0.2f, +1.0f};
    Base::vec4_t c {+0.7f, -0.7f, +0.2f, +1.0f};
    Base::vec2_t uv0 {0.0f, 0.0f};
    Base::vec2_t uv1 {1.0f, 0.0f};
    Base::vec2_t uv2 {0.0f, 1.0f};
    Base::vec3_t normal {0.0f, 0.0f, -1.0f};
    Base::vec4_t fred = Render::get_color_vec4(s_red);
    Base::vec4_t fgreen = Render::get_color_vec4(s_green);
    Base::vec4_t fblue = Render::get_color_vec4(s_blue);
    s_abc.push_back(a);
    s_abc.push_back(b);
    s_abc.push_back(c);
    Base::append_data(s_abc_triangle_attribs, fred);
    Base::append_data(s_abc_triangle_attribs, uv0);
    Base::append_data(s_abc_triangle_attribs, normal);
    Base::append_data(s_abc_triangle_attribs, fgreen);
    Base::append_data(s_abc_triangle_attribs, uv1);
    Base::append_data(s_abc_triangle_attribs, normal);
    Base::append_data(s_abc_triangle_attribs, fblue);
    Base::append_data(s_abc_triangle_attribs, uv2);
    Base::append_data(s_abc_triangle_attribs, normal);
    // we prepared sample triangle to draw in both modes - line and solid
    s_render_context.set_vertex_shader(
        [](const Base::vec4_t& inp, Base::vec4_t& out, std::vector<uint8_t>& per_vertex_out,
           const uint8_t* per_vertex_data, const std::vector<uint8_t>& builtins,
           const std::vector<uint8_t>& descriptor_set)
        {
            const Base::mat4_t* model_view_matrix_ptr = reinterpret_cast<const Base::mat4_t*>(builtins.data());
            const Base::mat4_t* transform_ptr = model_view_matrix_ptr + 4;
            out = (*model_view_matrix_ptr) * inp;
            // almost rasterized coord is found - need to divide by w and make x,y fit
            // to viewport in another function
            const Base::vec4_t* color_ptr = reinterpret_cast<const Base::vec4_t*>(per_vertex_data);
            const Base::vec2_t* tex_ptr = reinterpret_cast<const Base::vec2_t*>(color_ptr + 1);
            const Base::vec3_t* normal_ptr = reinterpret_cast<const Base::vec3_t*>(tex_ptr + 1);
            int                 offset = 0;
            // rotate position
            Base::vec4_t pos = out;
            memcpy(per_vertex_out.data() + offset, &pos, sizeof(Base::vec4_t));
            offset += sizeof(Base::vec4_t);
            //
            memcpy(per_vertex_out.data() + offset, color_ptr, sizeof(Base::vec4_t));
            offset += sizeof(Base::vec4_t);
            memcpy(per_vertex_out.data() + offset, tex_ptr, sizeof(Base::vec2_t));
            offset += sizeof(Base::vec2_t);
            // rotate normal
            Base::vec4_t normal {normal_ptr->x, normal_ptr->y, normal_ptr->z, 0.0f};
            normal = (*model_view_matrix_ptr) * normal;
            normal = normal.calculate_normalized();
            memcpy(per_vertex_out.data() + offset, &normal, sizeof(Base::vec3_t));
        });
    s_render_context.set_pixel_shader(
        [](const Base::vec4_t& inp, const uint8_t* per_pixel_data, const std::vector<uint8_t>& builtins,
           const std::vector<uint8_t>& descriptor_set) -> Base::vec4_t
        {
            const Base::vec4_t* pos_ptr = reinterpret_cast<const Base::vec4_t*>(per_pixel_data);
            const Base::vec4_t* pixel_color_ptr = reinterpret_cast<const Base::vec4_t*>(pos_ptr + 1);
            const Base::vec2_t* tex_ptr = reinterpret_cast<const Base::vec2_t*>(pixel_color_ptr + 1);
            const Base::vec3_t* normal_ptr = reinterpret_cast<const Base::vec3_t*>(tex_ptr + 1);
            // per-pixel
            Base::vec4_t tex_color = Render::TextureHolder::instance()->get_pixel(0, tex_ptr->u, tex_ptr->v);
            // illumination
            const Base::vec3_t* camera_position_ptr = reinterpret_cast<const Base::vec3_t*>(descriptor_set.data());
            const int*          light_count_ptr = reinterpret_cast<const int*>(camera_position_ptr + 1);
            const int*          light_type_ptr = light_count_ptr + 1;
            Base::vec3_t        pixel_position {pos_ptr->x, pos_ptr->y, pos_ptr->z};
            float               ambient_illumination = 0.0f, diffuse_illumination = 0.0f, specular_illumination = 0.0f;
            for (int i = 0; i < *light_count_ptr; i++)
            {
                const float* illumination_ptr = reinterpret_cast<const float*>(light_type_ptr + 1);
                if ((*light_type_ptr) == 0)
                {
                    // ambient
                    ambient_illumination += *illumination_ptr;
                    light_type_ptr = reinterpret_cast<const int*>(illumination_ptr + 1);
                }
                else
                {
                    const Base::vec3_t* light_crd_ptr = reinterpret_cast<const Base::vec3_t*>(illumination_ptr + 1);
                    Base::vec3_t        v;
                    if ((*light_type_ptr) == 1)
                    {
                        // point light
                        v = (*light_crd_ptr) - pixel_position;
                        v = v.calculate_normalized();
                    }
                    else
                    {
                        // directed light
                        v = -(*light_crd_ptr);
                        v /= v.calculate_magnitude();
                    }
                    // diffuse
                    float v_dot_n = v.calculate_dot_product(*normal_ptr);
                    if (v_dot_n > 0)
                    {
                        diffuse_illumination += v_dot_n * (*illumination_ptr);
                        // specular
                        Base::vec3_t vp = v - (*normal_ptr) * v.calculate_dot_product(*normal_ptr);
                        Base::vec3_t r = v - vp * 2.0f;
                        r = r.calculate_normalized();
                        Base::vec3_t d = (*camera_position_ptr) - pixel_position;
                        d = d.calculate_normalized();
                        float d_dot_r = d.calculate_dot_product(r);
                        if (d_dot_r > 0)
                        {
                            specular_illumination += pow(d_dot_r, 16.0f) * (*illumination_ptr);
                        }
                    }
                    light_type_ptr = reinterpret_cast<const int*>(light_crd_ptr + 1);
                }
            }
            float        illumination = ambient_illumination + diffuse_illumination + specular_illumination;
            Base::vec4_t illumination_vec {illumination, illumination, illumination, 1.0f};
            Base::vec4_t final_color = (*pixel_color_ptr) * tex_color * illumination_vec;
            return final_color.clamp(0.0f, 1.0f);
        });
    // load model and texture
    char tex_path[128], obj_path[128];
    snprintf(tex_path, 128, "%s/side_colored.png", SISYPHUS_RESOURCES_FOLDER);
    snprintf(obj_path, 128, "%s/cube_sided.obj", SISYPHUS_RESOURCES_FOLDER);
    uint32_t                       mosaic_texture = load_texture_win(tex_path);
    std::shared_ptr<Util::ObjFile> obj_file = Util::read_obj_model_file(obj_path);

    std::vector<Base::vec4_t> colors;
    for (int vert_idx = 0; vert_idx < obj_file->coord.size(); vert_idx++)
    {
        colors.push_back(s_default_colors[vert_idx % s_default_colors.size()]);
    }
    int gidx = 0;
    for (int i = 0; i < obj_file->faces.size(); i++)
    {
        const Util::ObjFace& face = obj_file->faces[i];
        for (int j = 0; j < 3; j++)
        {
            int          vert_idx = face.indices[j].position - 1;
            int          uv_idx = face.indices[j].texture - 1;
            int          normal_idx = face.indices[j].normal - 1;
            Base::vec4_t pos {
                obj_file->coord[vert_idx].x, obj_file->coord[vert_idx].y, obj_file->coord[vert_idx].z, 1.0f};
            s_model_verts.push_back(pos);
            s_model_inds.push_back(gidx++);
            Base::append_data(s_model_vertex_attribs, colors[vert_idx]);
            Base::append_data(s_model_vertex_attribs, obj_file->uv[uv_idx]);
            Base::append_data(s_model_vertex_attribs, obj_file->normal[normal_idx]);
        }
    }
}

void
sisyphus_application_update(void* data)
{
    int*   update_data_i_ptr = reinterpret_cast<int*>(data);
    int    width = *update_data_i_ptr++;
    int    height = *update_data_i_ptr++;
    int    bpp = *update_data_i_ptr++;
    float* init_data_f_ptr = reinterpret_cast<float*>(update_data_i_ptr);
    float  dtime = *init_data_f_ptr++;
    //
    static float current_time = 0.0f;
    static float period_time = 5.0f; // in seconds
    current_time += dtime;
    float rotation_degree = current_time / period_time;
    if (rotation_degree > 1.0f)
    {
        rotation_degree -= 1.0f;
        current_time -= period_time;
    }
    float angle = 2.0f * Base::pi * rotation_degree;
    // float angley = 30.0f / 360.0f * 2.0f * Base::pi;
    // float anglez = 10.0f / 360.0f * 2.0f * Base::pi;
    const Base::mat4_t rot_matrix_y = Base::mat4_t::calculate_rotation_matrix_around_y(angle);
    const Base::mat4_t rot_matrix_z = Base::mat4_t::calculate_rotation_matrix_around_z(angle);
    Base::mat4_t       rotation_matrix = rot_matrix_y * rot_matrix_z;

    Base::mat4_t scale_matrix = Base::mat4_t::get_identity_matrix();
    scale_matrix.r0.x = 0.5f;
    scale_matrix.r1.y = 0.5f;
    scale_matrix.r2.z = 0.5f;

    Base::mat4_t translation_matrix = Base::mat4_t::get_identity_matrix();
    // translation_matrix.r1.w = 0.3f; // y-shift
    translation_matrix.r2.w = 0.7f; // z-shift

    s_render_context.set_model_matrix(translation_matrix * rotation_matrix * scale_matrix);
    s_render_context.set_view_matrix(Base::mat4_t::get_identity_matrix()); // not really used yet
    s_render_context.set_perspective(90.0f, width / (float)height, 0.4f, 100.0f);
    s_render_context.set_backface_culling(Render::ECullingMode::CounterClockWise);
    //
    std::vector<uint8_t> descriptor_set;
    Base::vec3_t         camera_position {0.0f, 0.0f, 0.0f};
    Base::append_data(descriptor_set, camera_position);
    int light_count = 3;
    Base::append_data(descriptor_set, light_count);
    int   light_ambient = 0;
    float ambient_illumination = 0.2f;
    int   light_point = 1;
    float point_illumination = 0.4f;
    int   light_directed = 2;
    float direct_illumination = 0.4f;
    Base::append_data(descriptor_set, light_ambient);
    Base::append_data(descriptor_set, ambient_illumination);
    Base::append_data(descriptor_set, light_point);
    Base::append_data(descriptor_set, point_illumination);
    Base::vec3_t light_point_position {-2.0f, 0.0f, -1.0f};
    Base::append_data(descriptor_set, light_point_position);
    Base::append_data(descriptor_set, light_directed);
    Base::append_data(descriptor_set, direct_illumination);
    Base::vec3_t light_direction {-1.0f, 0.0f, 1.0f};
    Base::append_data(descriptor_set, light_direction);
    //
    s_render_context.resize(width, height, bpp);
    s_render_context.set_viewport(0, 0, 0, width, height, 1);
    // begin straight filling of color buffer
    s_render_context.clear_depth(0.0f);
    s_render_context.fill(s_bg_color); // fill background and also clear screen
    //
    s_render_context.set_descriptor_set(descriptor_set);
    s_render_context.draw_triangles(
        s_abc,
        s_abc_triangle_indices,
        reinterpret_cast<const uint8_t*>(s_abc_triangle_attribs.data()),
        s_vertex_input_format, s_vertex_output_format);
    s_render_context.draw_triangles(
        s_model_verts, s_model_inds, reinterpret_cast<const uint8_t*>(s_model_vertex_attribs.data()),
        s_vertex_input_format, s_vertex_output_format);
#if TRIANGLE_LINE
    s_render_context.draw_lines(
        s_abc,
        s_abc_line_indices,
        reinterpret_cast<const uint8_t*>(s_abc_triangle_attribs.data()),
        s_vertex_input_format, s_vertex_output_format
    );
#endif
}

void
sisyphus_application_set_log_function(void (*func)(const char* msg, unsigned int msg_size))
{
    s_render_context.set_log_func(func);
}

void
sisyphus_application_close()
{
    // s_render_context.destroy();
}

void
sisyphus_application_get_frame(void* data_ptr, unsigned int data_size)
{
    unsigned int frame_size = s_render_context.get_frame_size();
    assert(frame_size <= data_size);
    const void* frame_data = s_render_context.get_frame();
    memcpy(data_ptr, frame_data, frame_size);
}
}
