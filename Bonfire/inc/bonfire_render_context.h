#pragma once

#include <cstdint>
#include "bonfire_color.h"
#include "base_vectors.h"
#include <tuple>

/*
void ps()
{
    ctx->dfdy(float value);
    SampleTexture(Texture * tex, Sampelr * sampler, flaot2 uv)
    {
        uv *= tex.size();

        float2 slope = dfdy(uv);
        float2 slope = dfdx(uv);

        mip_level = tex_pixel_size / scren_pixel_size;

        mip_level1;
        mip_level2
    }
}
*/

namespace Temple {
    namespace Bonfire {
        enum class RenderMode {
            WIREFRAME,
            TRIANGLE,
        };
        enum class PrimitiveType {
            LINE,
            TRIANGLE,
        };
        enum class VertexAttribType {
            FLOAT32,
            INT32,
            UINT8,
            VEC2,
            VEC3,
            VEC4,
            UV,
        };
        enum class CullingMode {
            None,
            ClockWise,
            CounterClockWise
        };
        struct VertexFormat {
            size_t size;
            std::vector<VertexAttribType> attributes; // position is always in the beginning
            VertexFormat(const std::vector<VertexAttribType>& attribs);
        };
        void interpolateAttributes(const uint8_t* aIn, const uint8_t* bIn, uint8_t* cOut, float weight, const VertexFormat& vf);
        void multiplyAttributes(const uint8_t* aIn, uint8_t* cOut, float mult, const VertexFormat& vf);
        // perVertexData use only inside vertex shader; do not pass further and no need to interpolate
        // everything needed for pixel shader put to perVertexOut - this data will be interpolated
        using vertexShaderFunc = void(*)(const Base::vec4& input, Base::vec4& output, std::vector<uint8_t>& perVertexOut, 
            const uint8_t* perVertexData, const std::vector<uint8_t>& descriptorSet); // over single vertex
        using pixelShaderFunc = Base::vec4 (*)(const Base::vec4& input, const uint8_t* perPixelInp, 
            const std::vector<uint8_t>& descriptorSet); // over single pixel
        class RenderContext {
        private:
            uint8_t* m_data = nullptr;
            float* m_depth = nullptr;
            int m_width = 0;
            int m_height = 0;
            int m_bytesPerPixel = 0;
            Base::vec3 m_viewportMin, m_viewportMax;
            RenderMode m_renderMode;
            std::vector<uint8_t> m_descriptorSet;
            vertexShaderFunc m_vsf = nullptr;
            pixelShaderFunc m_psf = nullptr;
            bool m_depthWrite = true;
            bool m_depthTest = true;
            CullingMode m_backFaceCulling = CullingMode::None;
        public:
            RenderContext(int width, int height, int bytesPerPixel);
            const uint8_t* getFrame() const;
            void resize(int width, int height, int bytesPerPixel);
            void setViewport(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax);
            void setDescriptorSet(const std::vector<uint8_t>& descriptorSet);
            void setVertexShader(vertexShaderFunc vsf);
            void setPixelShader(pixelShaderFunc psf);
            void putPixel(int x, int y, const Base::vec4& color);
            void renderPixelDepthWise(const Base::vec4& p, const uint8_t* data);
            Base::vec4 processVertex(const Base::vec4& v);
            void fill(const col4u& color);
            //
            void setDepthTest(bool flag);
            void setDepthWrite(bool flag);
            void setBackfaceCulling(CullingMode mode);
            void clearDepth(float val);
            void drawLines(const std::vector<Base::vec4>& coords, const std::vector<int>& indices, const uint8_t* vertexData, const VertexFormat& vInFormat, const VertexFormat& vOutFormat);
            void drawTriangles(const std::vector<Base::vec4>& coords, const std::vector<int>& indices, const uint8_t* vertexData, const VertexFormat& vInFormat, const VertexFormat& vOutFormat);
            //
            ~RenderContext();
        };
    }
}
