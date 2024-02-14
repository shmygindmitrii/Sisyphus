#pragma once

#include <cstdint>
#include <vector>
#include "bonfire_texture.h"
#include "bonfire_color.h"

namespace Temple
{
namespace Bonfire
{
  class TextureHolder {
    std::vector<texture>  m_textures;
    static TextureHolder* s_instance;

  public:
    static TextureHolder*
    instance();
    uint32_t
    addTexture(const uint8_t* pixelData, uint32_t w, uint32_t h, uint32_t ch);
    Base::vec4_t
    getPixel(uint32_t texId, float u, float v) const;
#if _WIN32 && !PLATFORM_XBO
    void
    saveTexture(const char* path, int texId);
#endif
  };
} // namespace Bonfire
} // namespace Temple
