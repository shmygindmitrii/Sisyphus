#pragma once
#include <cstdint>
#include "bonfire_color.h"
#include "base_vectors.h"

namespace Temple {
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
            void putPixelStraight(int x, int y, const col4u& color);
            void putPixel(int x, int y, const col4u& color);
            void fill(const col4u& color);
            void drawLine(const Base::vec3 &a, const Base::vec3& b, const col4u& color);
            void drawTriangle(const Base::vec3& a, const Base::vec3& b, const Base::vec3& c, const col4u& color);
            ~RawCanvas();
        };
    }
}
