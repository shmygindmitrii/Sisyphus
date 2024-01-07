#include "bonfire_raw_canvas.h"

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>

Temple::Bonfire::RawCanvas::RawCanvas(int width, int height, int bytesPerPixel) : m_width(width), m_height(height), m_bytesPerPixel(bytesPerPixel) {
    size_t fullSize = m_width * m_height * (size_t)m_bytesPerPixel;
    assert(fullSize > 0);
    m_data = new uint8_t[fullSize];
    m_renderMode = RenderMode::WIREFRAME;
}

const uint8_t* Temple::Bonfire::RawCanvas::getData() const {
    return m_data;
}

void Temple::Bonfire::RawCanvas::resize(int width, int height, int bytesPerPixel) {
    size_t oldSize = m_width * m_height * (size_t)m_bytesPerPixel;
    m_width = width;
    m_height = height;
    m_bytesPerPixel = bytesPerPixel;
    size_t fullSize = m_width * m_height * (size_t)m_bytesPerPixel;
    assert(fullSize >= 0);
    if (fullSize != oldSize) {
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
        if (fullSize > 0) {
            m_data = new uint8_t[fullSize];
        }
    }
}

void Temple::Bonfire::RawCanvas::setViewport(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax) {
    m_viewportMin.x = xMin;
    m_viewportMin.y = yMin;
    m_viewportMin.z = zMin;

    m_viewportMax.x = xMax;
    m_viewportMax.y = yMax;
    m_viewportMax.z = zMax;
}

void Temple::Bonfire::RawCanvas::setDescriptorSet(const void* descriptorSet) {
    m_descriptorSet = descriptorSet;
}

void Temple::Bonfire::RawCanvas::setRenderMode(RenderMode m) {
    m_renderMode = m;
}

void Temple::Bonfire::RawCanvas::setVertexShader(Temple::Bonfire::vertexShaderFunc vsf) {
    m_vsf = vsf;
}

void Temple::Bonfire::RawCanvas::setPixelShader(Temple::Bonfire::pixelShaderFunc psf) {
    m_psf = psf;
}

void Temple::Bonfire::RawCanvas::putPixel(int x, int y, const col4u& color) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return;
    // coordinates 
    // x : 0 to w (left to right)
    // y : 0 to h (top to bottom)
    int pixelIndex = y * m_width * m_bytesPerPixel + x * m_bytesPerPixel;
    m_data[pixelIndex + 0] = color.b;
    m_data[pixelIndex + 1] = color.g;
    m_data[pixelIndex + 2] = color.r;
    m_data[pixelIndex + 3] = color.a;
}

void Temple::Bonfire::RawCanvas::fill(const col4u& color) {
    uint32_t colors = m_width * m_height * m_bytesPerPixel;
    for (uint32_t i = 0; i < colors; i += m_bytesPerPixel) { // fake m_bytesPerPixel - now always 4 and maybe will always be 4
        m_data[i + 0] = color.b;
        m_data[i + 1] = color.g;
        m_data[i + 2] = color.r;
        m_data[i + 3] = color.a;
    }
}

void Temple::Bonfire::RawCanvas::drawLine(const Base::vec4& a, const Base::vec4& b, const col4u& color) {
    Base::vec4 a0(a);
    Base::vec4 b0(b);
    if (a.y > b.y) {
        a0 = b;
        b0 = a;
    }
    // a is bottom vertex and b is top vertex
    float yDif = b0.y - a0.y;
    float xDif = b0.x - a0.x;
    if (fabs(yDif) < 0.001f && fabs(xDif) < 0.001f) {
        // point
        this->putPixel(a0.x, a0.y, color);
    }
    else {
        if (fabs(yDif) > fabs(xDif)) {
            float slope = xDif / yDif;
            for (float y = a0.y; y < b0.y; y += 1.0f) {
                float x = a0.x + (y - a0.y) * slope;
                this->putPixel((int)x, (int)y, color);
            }
        }
        else {
            if (a0.x > b0.x) {
                Base::vec4 c0 = a0;
                a0 = b0;
                b0 = c0;
            }
            float slope = yDif / xDif;
            for (float x = a0.x; x < b0.x; x += 1.0f) {
                float y = a0.y + (x - a0.x) * slope;
                this->putPixel((int)x, (int)y, color);
            }
        }
    }
}

Temple::Base::vec4 Temple::Bonfire::RawCanvas::processVertex(const Base::vec4& v) {
    Base::vec4 c(v);
    float w = c.w;
    c.w = 1.0f;
    c = c / w;

    Base::vec3 crd{ c.x, c.y, c.z };
    crd = (crd + 1.0f) * 0.5f * (m_viewportMax - m_viewportMin) + m_viewportMin;

    return Base::vec4(crd, c.w);
}

void Temple::Bonfire::RawCanvas::drawFilledTriangle(const Base::vec4& a, const Base::vec4& b, const Base::vec4& c, const col4u& color) {
    Base::vec4 sa = a, sb = b, sc = c;
    if (sa.y > sc.y) {
        std::swap(sa, sc);
    }
    if (sa.y > sb.y) {
        std::swap(sa, sb);
    }
    if (sb.y > sc.y) {
        std::swap(sb, sc);
    }
    // get interpolated values
    std::vector<float> xab = Base::interpolate(sa.y, sa.x, sb.y, sb.x);
    std::vector<float> xbc = Base::interpolate(sb.y, sb.x, sc.y, sc.x);
    std::vector<float> xac = Base::interpolate(sa.y, sa.x, sc.y, sc.x); // long side x
    int nzeros = (int)(xab.size() == 0) + (int)(xbc.size() == 0) + (int)(xac.size() == 0);
    if (nzeros > 1) {
        return;
    }
    //
    xab.insert(xab.end(), xbc.begin(), xbc.end());
    int n = std::min(xab.size(), xac.size());
    // now we have only two vectors with x-values, need to understand, what is left and what is right
    int middle = n / 2;
    if (xac[middle] < xab[middle]) {
        std::swap(xac, xab);
    }
    // from xab to xac
    float bottomy = sa.y;
    for (int i = 0; i < n; i++) {
        float leftx = xab[i];
        float rightx = xac[i];
        while (leftx < rightx) {
            this->putPixel((int)leftx, (int)bottomy, color);
            leftx += 1.0f;
        }
        bottomy += 1.0f;
    }
}

void Temple::Bonfire::RawCanvas::drawTriangle(const Base::vec4& a, const Base::vec4& b, const Base::vec4& c, const col4u& color) {
    Base::vec4 va, vb, vc;
    this->m_vsf(a, &va, this->m_descriptorSet);
    this->m_vsf(b, &vb, this->m_descriptorSet);
    this->m_vsf(c, &vc, this->m_descriptorSet);

    va = processVertex(va);
    vb = processVertex(vb);
    vc = processVertex(vc);

    // cut segments

    switch (m_renderMode) {
    case TRIANGLE:
        this->drawFilledTriangle(va, vb, vc, color);
        break;
    case WIREFRAME:
    default:
        this->drawLine(va, vb, color);
        this->drawLine(vb, vc, color);
        this->drawLine(vc, va, color);
    }
}

Temple::Bonfire::RawCanvas::~RawCanvas() {
    delete[] m_data;
    m_data = nullptr;
}
