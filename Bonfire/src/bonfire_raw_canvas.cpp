#include "bonfire_raw_canvas.h"

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>

Temple::Bonfire::VertexFormat::VertexFormat(const std::vector<Temple::Bonfire::VertexAttribType>& attribs) {
    size = 0;
    for (int i = 0; i < attribs.size(); i++) {
        attributes.push_back(attribs[i]);
        switch (attribs[i]) {
        case VertexAttribType::FLOAT32:
            size += 4;
            break;
        case VertexAttribType::INT32:
            size += 4;
            break;
        case VertexAttribType::UINT8:
            size += 1;
            break;
        case VertexAttribType::VEC3:
            size += 12;
            break;
        case VertexAttribType::VEC4:
            size += 16;
            break;
        case VertexAttribType::COL4U:
            size += 4;
            break;
        }
    }
}

template<typename T>
static inline void interpolateTemplate(const uint8_t*& aIn, const uint8_t*& bIn, uint8_t*& cOut, float weight) {
    *(T*)cOut = (*(T*)bIn) * weight - (*(T*)aIn) * weight + *(T*)aIn;
    aIn += sizeof(T);
    bIn += sizeof(T);
    cOut += sizeof(T);
}

void Temple::Bonfire::interpolateAttributes(const uint8_t* aIn, const uint8_t* bIn, uint8_t* cOut, float weight, const VertexFormat& vf) {
    // cOut should be enough to hold aIn or bIn (they are the same in terms of size)
    // weight is always from 0 to 1
    for (int i = 0; i < vf.attributes.size(); i++) {
        switch (vf.attributes[i]) {
        case VertexAttribType::FLOAT32:
            interpolateTemplate<float>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::INT32:
            interpolateTemplate<int32_t>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::UINT8:
            interpolateTemplate<uint8_t>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::VEC3:
            interpolateTemplate<Base::vec3>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::VEC4:
            interpolateTemplate<Base::vec4>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::COL4U:
            interpolateTemplate<uint8_t>(aIn, bIn, cOut, weight);
            interpolateTemplate<uint8_t>(aIn, bIn, cOut, weight);
            interpolateTemplate<uint8_t>(aIn, bIn, cOut, weight);
            interpolateTemplate<uint8_t>(aIn, bIn, cOut, weight); 
            break;
        }
    }
}

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
    this->m_vsf(a, &va, &color, this->m_descriptorSet);
    this->m_vsf(b, &vb, &color, this->m_descriptorSet);
    this->m_vsf(c, &vc, &color, this->m_descriptorSet);

    va = processVertex(va);
    vb = processVertex(vb);
    vc = processVertex(vc);

    // cut segments

    switch (m_renderMode) {
    case RenderMode::TRIANGLE:
        this->drawFilledTriangle(va, vb, vc, color);
        break;
    case RenderMode::WIREFRAME:
    default:
        this->drawLine(va, vb, color);
        this->drawLine(vb, vc, color);
        this->drawLine(vc, va, color);
    }
}

void Temple::Bonfire::RawCanvas::drawLines(const std::vector<Base::vec4>& coords, const std::vector<int> indices, const uint8_t* vertexData, int vertexDataSize, const VertexFormat& vf) {
    if (indices.size() == 0 || indices.size() % 2 != 0) return;
    for (int i = 0; i < indices.size(); i += 2) {
        const Base::vec4& va = coords[indices[i]];
        const Base::vec4& vb = coords[indices[i + 1]];
        const uint8_t* aData = &vertexData[indices[i] * vf.size];
        const uint8_t* bData = &vertexData[indices[i + 1] * vf.size];
        // draw single line here
        Base::vec4 a, b;
        this->m_vsf(va, &a, aData, this->m_descriptorSet);
        this->m_vsf(vb, &b, bData, this->m_descriptorSet);
        //
        a = processVertex(a);
        b = processVertex(b);
        // obtained vertex shader results and go to the pixel stage
        Base::vec4 a0(a);
        Base::vec4 b0(b);
        const uint8_t *aData0 = aData, *bData0 = bData;
        if (a.y > b.y) {
            a0 = b;
            b0 = a;
            aData0 = bData;
            bData0 = aData;
        }
        // a is bottom vertex and b is top vertex
        float yDif = b0.y - a0.y;
        float xDif = b0.x - a0.x;
        if (fabs(yDif) < 0.001f && fabs(xDif) < 0.001f) {
            // point
            this->m_psf(this, a0, aData0, this->m_descriptorSet);
        }
        else {
            if (fabs(yDif) > fabs(xDif)) {
                float slope = xDif / yDif;
                std::vector<uint8_t> cOut(vf.size);
                Base::vec4 c;
                for (c.y = a0.y; c.y < b0.y; c.y += 1.0f) {
                    c.x = a0.x + (c.y - a0.y) * slope;
                    float weight = (c.y - a0.y) / (b0.y - a0.y);
                    c.z = (b.z - a.z) * weight + a.z;
                    interpolateAttributes(aData0, bData0, &cOut[0], weight, vf);
                    this->m_psf(this, c, cOut.data(), this->m_descriptorSet);
                }
            }
            else {
                if (a0.x > b0.x) {
                    Base::vec4 c0 = a0;
                    const uint8_t* cData0 = aData0;
                    a0 = b0;
                    b0 = c0;
                    aData0 = bData0;
                    bData0 = cData0;
                }
                float slope = yDif / xDif;
                std::vector<uint8_t> cOut(vf.size);
                Base::vec4 c;
                for (c.x = a0.x; c.x < b0.x; c.x += 1.0f) {
                    c.y = a0.y + (c.x - a0.x) * slope;
                    float weight = (c.x - a0.x) / (b0.x - a0.x);
                    c.z = (b.z - a.z) * weight + a.z;
                    interpolateAttributes(aData0, bData0, &cOut[0], weight, vf);
                    this->m_psf(this, c, cOut.data(), this->m_descriptorSet);
                }
            }
        }
    }
}

static inline float getWeightBetween(float x, float y, float x0, float y0, float x1, float y1) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    if (fabs(dx) < fabs(dy)) {
        return (y - y0) / (y1 - y0);
    }
    else {
        if (EQUAL_FLOATS(x0, x1)) {
            return 0.0f;
        }
        return (x - x0) / (x1 - x0);
    }
}

void Temple::Bonfire::RawCanvas::drawTriangles(const std::vector<Base::vec4>& coords, const std::vector<int> indices, 
                                               const uint8_t* vertexData, int vertexDataSize, const VertexFormat& vf) {
    if (indices.size() == 0 || indices.size() % 3 != 0) return;
    for (int i = 0; i < indices.size(); i += 3) {
        const Base::vec4& va(coords[indices[i]]);
        const Base::vec4& vb(coords[indices[i + 1]]);
        const Base::vec4& vc(coords[indices[i + 2]]);
        const uint8_t* aData = &vertexData[indices[i] * vf.size];
        const uint8_t* bData = &vertexData[indices[i + 1] * vf.size];
        const uint8_t* cData = &vertexData[indices[i + 2] * vf.size];
        //
        Base::vec4 a, b, c;
        this->m_vsf(va, &a, aData, this->m_descriptorSet);
        this->m_vsf(vb, &b, bData, this->m_descriptorSet);
        this->m_vsf(vc, &c, cData, this->m_descriptorSet);
        //
        a = processVertex(a);
        b = processVertex(b);
        c = processVertex(c);
        //
        Base::vec4 sa = a, sb = b, sc = c;
        if (sa.y > sc.y) {
            std::swap(sa, sc);
            std::swap(aData, cData);
        }
        if (sa.y > sb.y) {
            std::swap(sa, sb);
            std::swap(aData, bData);
        }
        if (sb.y > sc.y) {
            std::swap(sb, sc);
            std::swap(bData, cData);
        }
        // get interpolated values - line coordinates, only one component
        std::vector<float> xab = Base::interpolate(sa.y, sa.x, sb.y, sb.x);
        std::vector<float> xbc = Base::interpolate(sb.y, sb.x, sc.y, sc.x);
        std::vector<float> xac = Base::interpolate(sa.y, sa.x, sc.y, sc.x); // long side x
        int nzeros = (int)(xab.size() == 0) + (int)(xbc.size() == 0) + (int)(xac.size() == 0);
        if (nzeros > 1) {
            return;
        }
        // here is the diffrence - we don't want to merge xab and xbc
        int n = std::min(xac.size(), xab.size() + xbc.size());
        int middle = n / 2;
        bool leftToRight = true;
        if (middle >= xab.size()) {
            if (xac[middle] < xbc[middle - xab.size()]) {
                leftToRight = false;
            }
        }
        else {
            if (xac[middle] < xab[middle]) {
                leftToRight = false;
            }
        }
        float bottomy = sa.y;
        int idx = 0;
        std::vector<uint8_t> vInterpolatedAC(vf.size), vInterpolatedAB(vf.size), vInterpolatedBC(vf.size), vInterpolatedLR(vf.size);
        if (leftToRight) {
            Base::vec4 c;
            for (idx = 0; idx < xab.size() % (n + 1); idx++) {
                float leftx = xab[idx];
                float rightx = xac[idx];
                float vWeightAB = getWeightBetween(leftx, bottomy, sa.x, sa.y, sb.x, sb.y);
                float vWeightAC = getWeightBetween(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
                interpolateAttributes(aData, bData, &vInterpolatedAB[0], vWeightAB, vf);
                interpolateAttributes(aData, cData, &vInterpolatedAC[0], vWeightAC, vf);
                float lz = sa.z + (sb.z - sa.z) * vWeightAB;
                float rz = sa.z + (sc.z - sa.z) * vWeightAC;
                while (leftx < rightx) {
                    float hWeight = (leftx - xab[idx]) / (xac[idx] - xab[idx]);
                    interpolateAttributes(vInterpolatedAB.data(), vInterpolatedAC.data(), &vInterpolatedLR[0], hWeight, vf);
                    c.x = leftx;
                    c.y = bottomy;
                    c.z = lz + (rz - lz) * hWeight;
                    this->m_psf(this, c, vInterpolatedLR.data(), this->m_descriptorSet);
                    leftx += 1.0f;
                }
                bottomy += 1.0f;
            }
            for (; idx < n; idx++) {
                float leftx = xbc[idx - xab.size()];
                float rightx = xac[idx];
                float vWeightBC = getWeightBetween(leftx, bottomy, sb.x, sb.y, sc.x, sc.y);
                float vWeightAC = getWeightBetween(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
                interpolateAttributes(bData, cData, &vInterpolatedBC[0], vWeightBC, vf);
                interpolateAttributes(aData, cData, &vInterpolatedAC[0], vWeightAC, vf);
                float lz = sb.z + (sc.z - sb.z) * vWeightBC;
                float rz = sa.z + (sc.z - sa.z) * vWeightAC;
                while (leftx < rightx) {
                    float hWeight = (leftx - xbc[idx - xab.size()]) / (xac[idx] - xbc[idx - xab.size()]);
                    interpolateAttributes(vInterpolatedBC.data(), vInterpolatedAC.data(), &vInterpolatedLR[0], hWeight, vf);
                    c.x = leftx;
                    c.y = bottomy;
                    c.z = lz + (rz - lz) * hWeight;
                    this->m_psf(this, c, vInterpolatedLR.data(), this->m_descriptorSet);
                    leftx += 1.0f;
                }
                bottomy += 1.0f;
            }
        }
        else {
            Base::vec4 c;
            for (idx = 0; idx < xab.size() % (n + 1); idx++) {
                float leftx = xac[idx];
                float rightx = xab[idx];
                float vWeightAC = getWeightBetween(leftx, bottomy, sa.x, sa.y, sc.x, sc.y);
                float vWeightAB = getWeightBetween(rightx, bottomy, sa.x, sa.y, sb.x, sb.y);
                interpolateAttributes(aData, bData, &vInterpolatedAB[0], vWeightAB, vf);
                interpolateAttributes(aData, cData, &vInterpolatedAC[0], vWeightAC, vf);
                float lz = sa.z + (sb.z - sa.z) * vWeightAB;
                float rz = sa.z + (sc.z - sa.z) * vWeightAC;
                while (leftx < rightx) {
                    float hWeight = (leftx - xac[idx]) / (xab[idx] - xac[idx]);
                    interpolateAttributes(vInterpolatedAC.data(), vInterpolatedAB.data(), &vInterpolatedLR[0], hWeight, vf);
                    c.x = leftx;
                    c.y = bottomy;
                    c.z = lz + (rz - lz) * hWeight;
                    this->m_psf(this, c, vInterpolatedLR.data(), this->m_descriptorSet);
                    leftx += 1.0f;
                }
                bottomy += 1.0f;
            }
            for (; idx < n; idx++) {
                float leftx = xac[idx];
                float rightx = xbc[idx - xab.size()];
                float vWeightAC = getWeightBetween(leftx, bottomy, sa.x, sa.y, sc.x, sc.y);
                float vWeightBC = getWeightBetween(rightx, bottomy, sb.x, sb.y, sc.x, sc.y);
                interpolateAttributes(bData, cData, &vInterpolatedBC[0], vWeightBC, vf);
                interpolateAttributes(aData, cData, &vInterpolatedAC[0], vWeightAC, vf);
                float lz = sa.z + (sc.z - sa.z) * vWeightAC;
                float rz = sb.z + (sc.z - sb.z) * vWeightBC;
                while (leftx < rightx) {
                    float hWeight = (leftx - xac[idx]) / (xbc[idx - xab.size()] - xac[idx]);
                    interpolateAttributes(vInterpolatedAC.data(), vInterpolatedBC.data(), &vInterpolatedLR[0], hWeight, vf);
                    c.x = leftx;
                    c.y = bottomy;
                    c.z = lz + (rz - lz) * hWeight;
                    this->m_psf(this, c, vInterpolatedLR.data(), this->m_descriptorSet);
                    leftx += 1.0f;
                }
                bottomy += 1.0f;
            }
        }
    }
}

Temple::Bonfire::RawCanvas::~RawCanvas() {
    delete[] m_data;
    m_data = nullptr;
}
