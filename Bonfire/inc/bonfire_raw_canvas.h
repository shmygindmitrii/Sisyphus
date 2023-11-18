#pragma once
#include <cstdint>
#include "bonfire_color.h"
#include "base_vectors.h"

namespace Temple {
    namespace Bonfire {
        using vertexShaderFunc = void(*)(const Base::vec4& input, Base::vec4* output, const void* descriptorSet);
        class RawCanvas {
        private:
            uint8_t* m_data = nullptr;
            int m_width = 0;
            int m_height = 0;
            int m_bytesPerPixel = 0;
            Base::vec3 m_viewportMin, m_viewportMax;
            const void* m_descriptorSet = nullptr;
            vertexShaderFunc m_vsf = nullptr;
        public:
            RawCanvas(int width, int height, int bytesPerPixel);
            const uint8_t* getData() const;
            void resize(int width, int height, int bytesPerPixel);
            void setViewport(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax);
            void setDescriptorSet(const void* descriptorSet);
            void setVertexShader(vertexShaderFunc vsf);
            void putPixelStraight(int x, int y, const col4u& color);
            void putPixel(int x, int y, const col4u& color);
            Base::vec4 processVertex(const Base::vec4& v);
            void fill(const col4u& color);
            void drawLine(const Base::vec4 &a, const Base::vec4& b, const col4u& color);
            void drawTriangle(const Base::vec4& a, const Base::vec4& b, const Base::vec4& c, const col4u& color);
            ~RawCanvas();
        };
    }
}
