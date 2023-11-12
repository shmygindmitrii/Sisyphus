#include "bonfire_raw_canvas.h"

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>

Temple::Bonfire::RawCanvas::RawCanvas(int width, int height, int bytesPerPixel) : m_width(width), m_height(height), m_bytesPerPixel(bytesPerPixel) {
    size_t fullSize = m_width * m_height * (size_t)m_bytesPerPixel;
    assert(fullSize > 0);
    m_data = new uint8_t[fullSize];
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

void Temple::Bonfire::RawCanvas::putPixelStraight(int x, int y, const col4u& color) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return;
    // coordinates 
    // x : 0 to w (left to right)
    // y : 0 to h (top to bottom)
    int pixelIndex = y * m_width * m_bytesPerPixel + x * m_bytesPerPixel;
    m_data[pixelIndex + 0] = color.r;
    m_data[pixelIndex + 1] = color.g;
    m_data[pixelIndex + 2] = color.b;
    m_data[pixelIndex + 3] = color.a;
}

void Temple::Bonfire::RawCanvas::putPixel(int x, int y, const col4u& color) {
    // coordinates 
    // x : -w/2 to w/2 (left to right)
    // y : -h/2 to h/2 (bottom to up)
    int cx = x + m_width / 2;
    int cy = m_height / 2 - y;
    this->putPixelStraight(cx, cy, color);
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
        this->putPixelStraight(a0.x, a0.y, color);
    }
    else {
        if (fabs(yDif) >= fabs(xDif)) {
            float yStep = yDif / fabs(yDif);
            float slope = xDif / yDif;
            for (float y = a0.y; y < b0.y; y += yStep) {
                float x = a0.x + (y - a0.y) * slope;
                this->putPixelStraight((int)x, (int)y, color);
            }
        }
        else {
            if (a.x > b.x) {
                a0 = b;
                b0 = a;
            }
            xDif = b0.x - a0.x;
            float xStep = xDif / fabs(xDif);
            float slope = yDif / xDif;
            for (float x = a0.x; x < b0.x; x += xStep) {
                float y = a0.y + (x - a0.x) * slope;
                this->putPixelStraight((int)x, (int)y, color);
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


void Temple::Bonfire::RawCanvas::drawTriangle(const Base::vec4& a, const Base::vec4& b, const Base::vec4& c, const col4u& color) {
    Base::vec4 va = processVertex(a);
    Base::vec4 vb = processVertex(b);
    Base::vec4 vc = processVertex(c);

    this->drawLine(va, vb, color);
    this->drawLine(vb, vc, color);
    this->drawLine(vc, va, color);
}

Temple::Bonfire::RawCanvas::~RawCanvas() {
    delete[] m_data;
    m_data = nullptr;
}
