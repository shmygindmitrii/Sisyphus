#include "render_texture.h"

Sisyphus::Render::Texture::Texture()
    : width(0)
    , height(0)
    , channels(0)
{}

Sisyphus::Render::Texture::Texture(const uint8_t* d, uint32_t w, uint32_t h, uint32_t ch)
{
    width = w;
    height = h;
    channels = ch;
    data.resize(w * h * ch);
    memcpy(data.data(), d, w * h * ch);
}

Sisyphus::Base::vec4_t
Sisyphus::Render::Texture::get_pixel_color(float u, float v) const
{
    Sisyphus::Base::vec4_t pixel;
    int                    iu = u * width;
    int                    iv = (1.0f - v) * height;
    uint32_t               idx = iv * width * channels + iu * channels;
    pixel.a = 1.0f;
    if (idx < width * height * channels)
    {
        switch (channels)
        {
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
