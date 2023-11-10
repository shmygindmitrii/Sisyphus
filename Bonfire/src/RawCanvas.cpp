#include "RawCanvas.h"
#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>

Bonfire::RawCanvas::RawCanvas(int width, int height, int bytesPerPixel) : m_width(width), m_height(height), m_bytesPerPixel(bytesPerPixel) {
    size_t fullSize = m_width * m_height * (size_t)m_bytesPerPixel;
    assert(fullSize > 0);
    m_data = new uint8_t[fullSize];
}

void Bonfire::RawCanvas::resize(int width, int height, int bytesPerPixel) {
    size_t oldSize = m_width * m_height * (size_t)m_bytesPerPixel;
    m_width = width;
    m_height = height;
    m_bytesPerPixel = bytesPerPixel;
    size_t fullSize = m_width * m_height * (size_t)m_bytesPerPixel;
    assert(fullSize > 0);
    if (fullSize != oldSize) {
        if (m_data != nullptr) {
            delete[] m_data;
        }
        m_data = new uint8_t[fullSize];
    }
}

const uint8_t* Bonfire::RawCanvas::getData() const {
    return m_data;
}

void Bonfire::RawCanvas::putPixelStraight(int x, int y, col4u color) {
    // coordinates 
    // x : 0 to w (left to right)
    // y : 0 to h (top to bottom)
    int pixelIndex = y * m_width * m_bytesPerPixel + x * m_bytesPerPixel;
    m_data[pixelIndex + 0] = color.r;
    m_data[pixelIndex + 1] = color.g;
    m_data[pixelIndex + 2] = color.b;
    m_data[pixelIndex + 3] = color.a;
}

void Bonfire::RawCanvas::putPixel(int x, int y, col4u color) {
    // coordinates 
    // x : -w/2 to w/2 (left to right)
    // y : -h/2 to h/2 (bottom to up)
    int cx = x + m_width / 2;
    int cy = m_height / 2 - y;
    this->putPixelStraight(cx, cy, color);
}

void Bonfire::RawCanvas::fill(col4u color) {
    uint32_t colors = m_width * m_height * m_bytesPerPixel;
    for (uint32_t i = 0; i < colors; i += m_bytesPerPixel) { // fake m_bytesPerPixel - now always 4 and maybe will always be 4
        m_data[i + 0] = color.b;
        m_data[i + 1] = color.g;
        m_data[i + 2] = color.r;
        m_data[i + 3] = color.a;
    }
}

Bonfire::RawCanvas::~RawCanvas() {
    delete[] m_data;
    m_data = nullptr;
}
