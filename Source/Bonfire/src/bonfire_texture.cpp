#include "bonfire_texture.h"

Temple::Bonfire::texture::texture() : width(0), height(0), channels(0) {

}

Temple::Bonfire::texture::texture(const uint8_t* d, uint32_t w, uint32_t h, uint32_t ch) {
    width = w;
    height = h;
    channels = ch;
    data.resize(w * h * ch);
    memcpy(data.data(), d, w * h * ch);
}

Temple::Base::vec4 Temple::Bonfire::texture::getPixel(float u, float v) const {
    Temple::Base::vec4 pixel;
    int iu = u * width;
    int iv = (1.0f - v) * height;
    uint32_t idx = iv * width * channels + iu * channels;
    pixel.a = 1.0f;
    if (idx < width * height * channels) {
        switch (channels) {
        case 4:
            pixel.a = data[idx + 3] / 255.0f;
        case 3:
            pixel.b = data[idx + 2] / 255.0f;
        case 2:
            pixel.g = data[idx + 1] / 255.0f;
        case 1:
            pixel.r = data[idx] / 255.0f;
        }
    }
    return pixel;
}
