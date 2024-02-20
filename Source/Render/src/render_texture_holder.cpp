#include "render_texture_holder.h"
#if _WIN32 && !PLATFORM_XBO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

Sisyphus::Render::TextureHolder* Sisyphus::Render::TextureHolder::s_instance = nullptr;

Sisyphus::Render::TextureHolder*
Sisyphus::Render::TextureHolder::instance()
{
    if (s_instance == nullptr)
    {
        s_instance = new Sisyphus::Render::TextureHolder();
    }
    return s_instance;
}

uint32_t
Sisyphus::Render::TextureHolder::add_texture(const uint8_t* pixelData, uint32_t w, uint32_t h, uint32_t ch)
{
    m_textures.push_back(Texture(pixelData, w, h, ch));
    return m_textures.size() - 1;
}

Sisyphus::Base::vec4_t
Sisyphus::Render::TextureHolder::get_pixel(uint32_t texId, float u, float v)
{
    Sisyphus::Base::vec4_t pixel;
    pixel.a = 1.0f;
    if (texId < m_textures.size())
    {
        Texture* tex_ptr = &m_textures[texId];
        pixel = tex_ptr->get_pixel_color(u, v);
    }
    return pixel;
}

void
Sisyphus::Render::TextureHolder::save_texture(const char* path, int texId)
{
    if (texId >= 0 && texId < m_textures.size())
    {
        const Texture& tex = m_textures[texId];
        stbi_write_png(path, tex.width, tex.height, tex.channels, tex.data.data(), tex.width * tex.channels);
    }
}
