#include "win_tex_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

uint32_t
load_texture_win(const char* path)
{
  if (s_textures_loaded.find(path) != s_textures_loaded.end())
  {
    return s_textures_loaded[path];
  }
  else
  {
    int            w, h, ch;
    unsigned char* img = stbi_load(path, &w, &h, &ch, 0);
    uint32_t       tex_id =
      Sisyphus::Render::TextureHolder::instance()->add_texture(reinterpret_cast<const uint8_t*>(img), w, h, ch);
    s_textures_loaded[path] = tex_id;
    return tex_id;
  }
}
