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

const uint8_t* Temple::Bonfire::RawCanvas::getData() const {
    return m_data;
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

void Temple::Bonfire::RawCanvas::drawLine(const Base::vec3& a, const Base::vec3& b, const col4u& color) {
    Base::vec3 a0(a);
    Base::vec3 b0(b);
    if (a.y > b.y) {
        a0 = b;
        b0 = a;
    }
    if (EQUAL_FLOATS(a0.x, b0.x)) {
        // y-line
        int yStep = (int)(b0.y - a0.y);
        for (int y = a0.y; y < b0.y + 1e-3f; y++) {
            this->putPixel((int)a0.x, (int)y, color);
        }
    }
    else {
        if (EQUAL_FLOATS(a0.y, b0.y)) {
            // x-line
            if (a.x > b.x) {
                a0 = b;
                b0 = a;
            }
            for (int x = a0.x; x <= b0.x + 1e-3f; x++) {
                this->putPixel((int)x, (int)a0.y, color);
            }
        }
        else {
            //
            float yDif = b0.y - a0.y;
            float xDif = b0.x - a0.x;
            float step = yDif / xDif;
            float yStep = yDif / abs(yDif);
            if (abs(yDif) > abs(xDif)) {
                for (float y = a0.y; y <= b0.y; y += yStep) {
                    float x = a0.x + (y - a0.y) / step;
                    this->putPixel((int)x, (int)y, color);
                }
            }
            else {
                if (a.x > b.x) {
                    a0 = b;
                    b0 = a;
                }
                xDif = b0.x - a0.x;
                float xStep = xDif / abs(xDif);
                for (float x = a0.x; x <= b0.x; x += xStep) {
                    float y = a0.y + (x - a0.x) * step;
                    this->putPixel((int)x, (int)y, color);
                }
            }
        }
    }
}

void Temple::Bonfire::RawCanvas::drawTriangle(const Base::vec3& a, const Base::vec3& b, const Base::vec3& c, const col4u& color) {
    this->drawLine(a, b, color);
    this->drawLine(b, c, color);
    this->drawLine(c, a, color);
}

Temple::Bonfire::RawCanvas::~RawCanvas() {
    delete[] m_data;
    m_data = nullptr;
}
