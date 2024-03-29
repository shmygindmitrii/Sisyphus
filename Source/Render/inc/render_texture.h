#pragma once

#include <cstdint>
#include <vector>
#include "render_color.h"
#include "base_vectors.h"

namespace Sisyphus
{
namespace Render
{
    struct Texture {
        std::vector<uint8_t> data;
        uint32_t             width, height, channels;
        Texture();
        Texture(const uint8_t* d, uint32_t w, uint32_t h, uint32_t ch);
        Sisyphus::Base::vec4_t
        get_pixel_color(float u, float v) const;
    };
} // namespace Render
} // namespace Sisyphus
