#pragma once

#include <cstdint>
#include <vector>
#include "bonfire_color.h"
#include "base_vectors.h"

namespace Temple {
    namespace Bonfire {
        struct texture {
            std::vector<uint8_t> data;
            uint32_t width, height, channels;
            texture();
            texture(const uint8_t* d, uint32_t w, uint32_t h, uint32_t ch);
            Temple::Base::vec4 getPixel(float u, float v) const;
        };
    }
}
