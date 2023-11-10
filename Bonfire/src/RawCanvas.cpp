#include "RawCanvas.h"

#include <assert.h>

Bonfire::RawCanvas::RawCanvas(int width, int height, int bytesPerPixel) : m_width(width), m_height(height), m_bytesPerPixel(bytesPerPixel) {
    size_t fullSize = m_width * m_height * (size_t)m_bytesPerPixel;
    assert(fullSize > 0);
    m_data = new unsigned char[fullSize];
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
        m_data = new unsigned char[fullSize];
    }
}

const unsigned char* Bonfire::RawCanvas::getData() const {
    return m_data;
}

void Bonfire::RawCanvas::putPixelStraight(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    int pixelIndex = y * m_width * m_bytesPerPixel + x * m_bytesPerPixel;
    m_data[pixelIndex + 0] = r;
    m_data[pixelIndex + 1] = g;
    m_data[pixelIndex + 2] = b;
    m_data[pixelIndex + 3] = a;
}



Bonfire::RawCanvas::~RawCanvas() {
    delete[] m_data;
    m_data = nullptr;
}
