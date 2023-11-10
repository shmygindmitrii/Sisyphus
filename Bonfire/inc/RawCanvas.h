#pragma once
#include <cstdint>
#include "Color.h"

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
        void putPixelStraight(int x, int y, col4u color);
        void putPixel(int x, int y, col4u color);
        void fill(col4u color);
        ~RawCanvas();
    };
}
