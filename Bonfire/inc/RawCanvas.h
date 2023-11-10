#pragma once

namespace Bonfire {
    class RawCanvas {
    private:
        unsigned char* m_data = nullptr;
        int m_width = 0;
        int m_height = 0;
        int m_bytesPerPixel = 0;
    public:
        RawCanvas(int width, int height, int bytesPerPixel);
        void resize(int width, int height, int bytesPerPixel);
        const unsigned char* getData() const;
        void putPixelStraight(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        ~RawCanvas();
    };
}
