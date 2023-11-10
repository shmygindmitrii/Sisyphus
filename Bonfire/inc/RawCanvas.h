#pragma once
#include <cstdint>
namespace Bonfire {
    class RawCanvas {
    private:
        uint8_t* m_data = nullptr;
        int m_width = 0;
        int m_height = 0;
        int m_bytesPerPixel = 0;
    public:
        RawCanvas(int width, int height, int bytesPerPixel);
        void resize(int width, int height, int bytesPerPixel);
        const uint8_t* getData() const;
        void putPixelStraight(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        void putPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        ~RawCanvas();
    };
}
