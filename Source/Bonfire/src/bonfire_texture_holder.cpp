#include "bonfire_texture_holder.h"
#if _WIN32 && !PLATFORM_XBO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

Temple::Bonfire::TextureHolder* Temple::Bonfire::TextureHolder::s_instance =
  nullptr;

Temple::Bonfire::TextureHolder*
Temple::Bonfire::TextureHolder::instance()
{
  if (s_instance == nullptr)
  {
    s_instance = new Temple::Bonfire::TextureHolder();
  }
  return s_instance;
}

uint32_t
Temple::Bonfire::TextureHolder::addTexture(
  const uint8_t* pixelData,
  uint32_t       w,
  uint32_t       h,
  uint32_t       ch)
{
  m_textures.push_back(texture(pixelData, w, h, ch));
  return m_textures.size() - 1;
}

Temple::Base::vec4
Temple::Bonfire::TextureHolder::getPixel(uint32_t texId, float u, float v) const
{
  Temple::Base::vec4 pixel;
  pixel.a = 1.0f;
  if (texId < m_textures.size())
  {
    const texture& tex = m_textures[texId];
    return tex.getPixel(u, v);
  }
  return pixel;
}

void
Temple::Bonfire::TextureHolder::saveTexture(const char* path, int texId)
{
  if (texId >= 0 && texId < m_textures.size())
  {
    const texture& tex = m_textures[texId];
    stbi_write_png(
      path,
      tex.width,
      tex.height,
      tex.channels,
      tex.data.data(),
      tex.width * tex.channels);
  }
}
