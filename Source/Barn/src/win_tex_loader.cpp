#include "win_tex_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

uint32_t loadTexture(const char* path) {
     if (s_texturesLoaded.find(path) != s_texturesLoaded.end()) {
         return s_texturesLoaded[path];
     }
     else {
         int w, h, ch;
         unsigned char* img = stbi_load(path, &w, &h, &ch, 0);
         uint32_t texId = Temple::Bonfire::TextureHolder::instance()->addTexture(reinterpret_cast<const uint8_t*>(img), w, h, ch);
         s_texturesLoaded[path] = texId;
         return texId;
     }
}
