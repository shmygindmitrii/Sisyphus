#include "bonfire_render_context.h"
#include "base_utils.h"

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
        case VertexAttribType::VEC2:
            size += 8;
            break;
        case VertexAttribType::VEC3:
            size += 12;
            break;
        case VertexAttribType::VEC4:
            size += 16;
            break;
        case VertexAttribType::UV:
            size += 8;
            break;
        }
    }
}

template<typename T>
static inline void __interpolateAttribsTemplate(const uint8_t*& aIn, const uint8_t*& bIn, uint8_t*& cOut, float weight) {
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
            __interpolateAttribsTemplate<float>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::INT32:
            __interpolateAttribsTemplate<int32_t>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::UINT8:
            __interpolateAttribsTemplate<uint8_t>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::VEC2:
            __interpolateAttribsTemplate<Base::vec2>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::VEC3:
            __interpolateAttribsTemplate<Base::vec3>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::VEC4:
            __interpolateAttribsTemplate<Base::vec4>(aIn, bIn, cOut, weight);
            break;
        case VertexAttribType::UV:
            __interpolateAttribsTemplate<Base::vec2>(aIn, bIn, cOut, weight);
            break;
        }
    }
}

template<typename T>
static inline void __multAttribsTemplate(const uint8_t*& aIn, uint8_t*& cOut, float mult) {
    *(T*)cOut = (*(T*)aIn) * mult;
    aIn += sizeof(T);
    cOut += sizeof(T);
}

void Temple::Bonfire::multiplyAttributes(const uint8_t* aIn, uint8_t* cOut, float mult, const VertexFormat& vf) {
    for (int i = 0; i < vf.attributes.size(); i++) {
        switch (vf.attributes[i]) {
        case VertexAttribType::FLOAT32:
            __multAttribsTemplate<float>(aIn, cOut, mult);
            break;
        case VertexAttribType::INT32:
            __multAttribsTemplate<int32_t>(aIn, cOut, mult);
            break;
        case VertexAttribType::UINT8:
            __multAttribsTemplate<uint8_t>(aIn, cOut, mult);
            break;
        case VertexAttribType::VEC2:
            __multAttribsTemplate<Base::vec2>(aIn, cOut, mult);
            break;
        case VertexAttribType::VEC3:
            __multAttribsTemplate<Base::vec3>(aIn, cOut, mult);
            break;
        case VertexAttribType::VEC4:
            __multAttribsTemplate<Base::vec4>(aIn, cOut, mult);
            break;
        case VertexAttribType::UV:
            __multAttribsTemplate<Base::vec2>(aIn, cOut, mult);
            break;
        }
    }
}

Temple::Bonfire::Plane::Plane() : normal(Base::vec3(0.0f, 0.0f, 1.0f)), offset(0.0f) {

}

Temple::Bonfire::Plane::Plane(const Base::vec3& _normal, float _offset) : normal(_normal), offset(_offset) {

}

Temple::Bonfire::RenderContext::RenderContext(int width, int height, int bytesPerPixel) : m_width(width), m_height(height), m_bytesPerPixel(bytesPerPixel),
                                                                                  m_depthWrite(true), m_depthTest(true) {
    size_t fullSize = m_width * m_height * (size_t)m_bytesPerPixel;
    assert(fullSize > 0);
    m_data = new uint8_t[fullSize];
    m_depth = new float[m_width * m_height];
    m_builtins.resize(sizeof(Base::mat4) * 5);
}

const uint8_t* Temple::Bonfire::RenderContext::getFrame() const {
    return m_data;
}

void Temple::Bonfire::RenderContext::resize(int width, int height, int bytesPerPixel) {

    size_t oldResolution = m_width * m_height;
    size_t oldSize = oldResolution * (size_t)m_bytesPerPixel;
    m_width = width;
    m_height = height;
    m_bytesPerPixel = bytesPerPixel;
    size_t curResolution = m_width * m_height;
    size_t fullSize = curResolution * (size_t)m_bytesPerPixel;
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
    if (curResolution != oldResolution) {
        if (m_depth != nullptr) {
            delete[] m_depth;
            m_depth = nullptr;
        }
        if (curResolution > 0) {
            m_depth = new float[curResolution];
        }
    }
}

void Temple::Bonfire::RenderContext::setViewport(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax) {
    m_viewportMin.x = xMin;
    m_viewportMin.y = yMin;
    m_viewportMin.z = zMin;

    m_viewportMax.x = xMax;
    m_viewportMax.y = yMax;
    m_viewportMax.z = zMax;
}

void Temple::Bonfire::RenderContext::setDescriptorSet(const std::vector<uint8_t>& descriptorSet) {
    m_descriptorSet = descriptorSet;
}

void Temple::Bonfire::RenderContext::setVertexShader(Temple::Bonfire::vertexShaderFunc vsf) {
    m_vsf = vsf;
}

void Temple::Bonfire::RenderContext::setPixelShader(Temple::Bonfire::pixelShaderFunc psf) {
    m_psf = psf;
}

void Temple::Bonfire::RenderContext::setModelMatrix(const Base::mat4& m) {
    m_modelMatrix = m;
    m_modelViewMatrix = m_viewMatrix * m_modelMatrix;
    m_transformMatrix = m_perspectiveMatrix * m_modelViewMatrix;
    Base::replaceData(m_builtins, m_modelMatrix, 0);
    Base::replaceData(m_builtins, m_modelViewMatrix, sizeof(Base::mat4) * 3);
    Base::replaceData(m_builtins, m_transformMatrix, sizeof(Base::mat4) * 4);
}

void Temple::Bonfire::RenderContext::setViewMatrix(const Base::mat4& m) {
    m_viewMatrix = m;
    m_modelViewMatrix = m_viewMatrix * m_modelMatrix;
    m_transformMatrix = m_perspectiveMatrix * m_modelViewMatrix;
    Base::replaceData(m_builtins, m_viewMatrix, sizeof(Base::mat4));
    Base::replaceData(m_builtins, m_modelViewMatrix, sizeof(Base::mat4) * 3);
    Base::replaceData(m_builtins, m_transformMatrix, sizeof(Base::mat4) * 4);
}

void Temple::Bonfire::RenderContext::setPerspectiveMatrix(const Base::mat4& m) {
    m_perspectiveMatrix = m;
    m_transformMatrix = m_perspectiveMatrix * m_modelViewMatrix;
    Base::replaceData(m_builtins, m_perspectiveMatrix, sizeof(Base::mat4) * 2);
    Base::replaceData(m_builtins, m_transformMatrix, sizeof(Base::mat4) * 4);
}

void Temple::Bonfire::RenderContext::setPerspective(float fov, float aspect, float znear, float zfar) {
    Base::mat4 perspectiveMatrix = Base::mat4::projection(fov, aspect, znear, zfar);
    this->setFrustum(fov, aspect, znear, zfar);
    this->setPerspectiveMatrix(perspectiveMatrix);
}

void Temple::Bonfire::RenderContext::setFrustum(float fov, float aspect, float znear, float zfar) {
    // create 6 planes that forms frustum in view coordinates
    // znear plane
    Base::vec3& znerNormal = m_frustum.bounds[0].normal;
    znerNormal.x = 0.0f;
    znerNormal.y = 0.0f;
    znerNormal.z = -1.0f;
    m_frustum.bounds[0].offset = znear;

    // zfar plane
    Base::vec3& zfarNormal = m_frustum.bounds[1].normal;
    zfarNormal.x = 0.0f;
    zfarNormal.y = 0.0f;
    zfarNormal.z = 1.0f;
    m_frustum.bounds[1].offset = -zfar;
    // points on planes - here we have world coordinates where y is going from up to down
    Base::vec3 b { 0.0f, znear * tanf(0.5f * fov), 0.0f };
    Base::vec3 r { znear * tanf(0.5f * fov) * aspect, 0.0f, 0.0f };
    Base::vec3 t { 0.0f, -b.y, 0.0f };
    Base::vec3 l { -r.x, 0.0f, 0.0f };
    // top plane
    Base::vec3& topNormal = m_frustum.bounds[2].normal;
    topNormal.x = 0.0f;
    topNormal.y = -sinf(0.5f * fov);
    topNormal.z = -cosf(0.5f * fov);
    m_frustum.bounds[2].offset = -topNormal.dot(t);
    // bottom plane
    Base::vec3& bottomNormal = m_frustum.bounds[3].normal;
    bottomNormal.x = 0.0f;
    bottomNormal.y = -topNormal.y;
    bottomNormal.z = topNormal.z;
    m_frustum.bounds[3].offset = -bottomNormal.dot(b);
    // left plane
    float horHalfAngle = atanf(aspect * b.y / znear);
    Base::vec3& leftNormal = m_frustum.bounds[4].normal;
    leftNormal.x = -cosf(horHalfAngle);
    leftNormal.y = 0.0f;
    leftNormal.z = -sinf(horHalfAngle);
    m_frustum.bounds[4].offset = -leftNormal.dot(l);
    // right plane
    Base::vec3& rightNormal = m_frustum.bounds[5].normal;
    rightNormal.x = -leftNormal.x;
    rightNormal.y = 0.0f;
    rightNormal.z = leftNormal.z;
    m_frustum.bounds[5].offset = -rightNormal.dot(r);
}

bool Temple::Bonfire::RenderContext::outOfSight(const Base::vec4& a, const Base::vec4& b, const Base::vec4& c, float znear, float zfar, float aspect) {
    if ((a.z > zfar && b.z > zfar && c.z > zfar) || (a.z < znear && b.z < znear && c.z < znear)) {
        // whole triangle is too close or too far
        return true;
    }
    return false;
}

void Temple::Bonfire::RenderContext::putPixel(int x, int y, const Base::vec4& color) {
    // color - from 0 to 1 each component
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return;
    int pixelIndex = y * m_width * m_bytesPerPixel + x * m_bytesPerPixel;
    // please, take care about color content in advance - clamp if needed
    m_data[pixelIndex + 0] = (uint8_t)(color.b * 255.0f);
    m_data[pixelIndex + 1] = (uint8_t)(color.g * 255.0f);
    m_data[pixelIndex + 2] = (uint8_t)(color.r * 255.0f);
    m_data[pixelIndex + 3] = (uint8_t)(color.a * 255.0f);
}

void Temple::Bonfire::RenderContext::fill(const col4u& color) {
    uint32_t colors = m_width * m_height * m_bytesPerPixel;
    for (uint32_t i = 0; i < colors; i += m_bytesPerPixel) { // fake m_bytesPerPixel - now always 4 and maybe will always be 4
        m_data[i + 0] = color.b;
        m_data[i + 1] = color.g;
        m_data[i + 2] = color.r;
        m_data[i + 3] = color.a;
    }
}

Temple::Base::vec4 Temple::Bonfire::RenderContext::processVertex(const Base::vec4& v) {
    Base::vec4 c = m_perspectiveMatrix * v;

    float w = c.w;
    c.w = 1.0f;
    c = c / w;

    Base::vec3 crd{ c.x, -c.y, c.z };
    crd = (crd + 1.0f) * 0.5f * (m_viewportMax - m_viewportMin) + m_viewportMin;

    return Base::vec4(crd, c.w);
}

void Temple::Bonfire::RenderContext::setDepthTest(bool flag) {
    m_depthTest = flag;
}

void Temple::Bonfire::RenderContext::setDepthWrite(bool flag) {
    m_depthWrite = flag;
}

void Temple::Bonfire::RenderContext::setBackfaceCulling(CullingMode mode) {
    m_backFaceCulling = mode;
}

void Temple::Bonfire::RenderContext::clearDepth(float val) {
    for (int i = 0; i < m_width * m_height; i++) {
        m_depth[i] = val;
    }
}

void Temple::Bonfire::RenderContext::renderPixelDepthWise(const Base::vec4& p, const uint8_t* data) {
    int pixFlatIdx = ((int)p.y) * m_width + (int)p.x;
    if (pixFlatIdx >= 0 && pixFlatIdx < m_width * m_height) {
        if (m_depthTest) {
            if (p.z > m_depth[pixFlatIdx]) {
                this->putPixel(p.x, p.y, this->m_psf(p, data, this->m_builtins, this->m_descriptorSet));
            }
        }
        else {
            this->putPixel(p.x, p.y, this->m_psf(p, data, this->m_builtins, this->m_descriptorSet));
        }
        if (m_depthWrite) {
            if (p.z > m_depth[pixFlatIdx]) {
                m_depth[pixFlatIdx] = p.z;
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

void Temple::Bonfire::RenderContext::drawLines(const std::vector<Base::vec4>& coords, const std::vector<int>& indices, 
    const uint8_t* vertexData, const VertexFormat& vInFormat, const VertexFormat& vOutFormat) {
    if (indices.size() == 0 || indices.size() % 2 != 0) return;
    for (int i = 0; i < indices.size(); i += 2) {
        const Base::vec4& va = coords[indices[i]];
        const Base::vec4& vb = coords[indices[i + 1]];
        const uint8_t* aData = &vertexData[indices[i] * vInFormat.size];
        const uint8_t* bData = &vertexData[indices[i + 1] * vInFormat.size];
        // draw single line here
        Base::vec4 a, b;
        std::vector<uint8_t> aVertexOut(vOutFormat.size), bVertexOut(vOutFormat.size);
        this->m_vsf(va, a, aVertexOut, aData, this->m_builtins, this->m_descriptorSet);
        this->m_vsf(vb, b, bVertexOut, bData, this->m_builtins, this->m_descriptorSet);
        //
        a = processVertex(a);
        b = processVertex(b);
        // obtained vertex shader results and go to the pixel stage
        Base::vec4 a0(a);
        Base::vec4 b0(b);
        // perspective correct interpolation part - normalize by z first
        std::vector<uint8_t> vDepthedA(vOutFormat.size), vDepthedB(vOutFormat.size);
        uint8_t *vDepthedAPtr = vDepthedA.data(), *vDepthedBPtr = vDepthedB.data();
        multiplyAttributes(aVertexOut.data(), vDepthedAPtr, a0.w, vOutFormat);
        multiplyAttributes(bVertexOut.data(), vDepthedBPtr, b0.w, vOutFormat);
        //
        const uint8_t *aData0 = vDepthedAPtr, *bData0 = vDepthedBPtr;
        if (a.y > b.y) {
            std::swap(a0, b0);
            std::swap(aData0, bData0);
        }
        // a is bottom vertex and b is top vertex
        float yDif = b0.y - a0.y;
        float xDif = b0.x - a0.x;
        if (fabs(yDif) < 0.001f && fabs(xDif) < 0.001f) {
            // point
            renderPixelDepthWise(a0, aData0);
        }
        else {
            std::vector<uint8_t> pixelOut(vOutFormat.size);
            if (fabs(yDif) > fabs(xDif)) {
                float slope = xDif / yDif;
                std::vector<uint8_t> cOut(vOutFormat.size);
                Base::vec4 c;
                for (c.y = a0.y; c.y < b0.y; c.y += 1.0f) {
                    c.x = a0.x + (c.y - a0.y) * slope;
                    float weight = (c.y - a0.y) / (b0.y - a0.y);
                    c.w = (b0.w - a0.w) * weight + a0.w;
                    float pzo = 1.0f / c.w;
                    c.z = (b0.z - a0.z) * weight + a0.z;
                    interpolateAttributes(aData0, bData0, cOut.data(), weight, vOutFormat);
                    multiplyAttributes(cOut.data(), pixelOut.data(), pzo, vOutFormat);
                    renderPixelDepthWise(c, pixelOut.data());
                }
            }
            else {
                if (a0.x > b0.x) {
                    std::swap(a0, b0);
                    std::swap(aData0, bData0);
                }
                float slope = yDif / xDif;
                std::vector<uint8_t> cOut(vOutFormat.size);
                Base::vec4 c;
                for (c.x = a0.x; c.x < b0.x; c.x += 1.0f) {
                    c.y = a0.y + (c.x - a0.x) * slope;
                    float weight = (c.x - a0.x) / (b0.x - a0.x);
                    c.w = (b0.w - a0.w) * weight + a0.w;
                    float pzo = 1.0f / c.w;
                    c.z = (b0.z - a0.z) * weight + a0.z;
                    interpolateAttributes(aData0, bData0, cOut.data(), weight, vOutFormat);
                    multiplyAttributes(cOut.data(), pixelOut.data(), pzo, vOutFormat);
                    renderPixelDepthWise(c, pixelOut.data());
                }
            }
        }
    }
}

void Temple::Bonfire::RenderContext::drawTriangles(const std::vector<Base::vec4>& coords, const std::vector<int>& indices, 
                                                   const uint8_t* vertexData, const VertexFormat& vInFormat, const VertexFormat& vOutFormat) {
    if (indices.size() == 0 || indices.size() % 3 != 0) return;
    int fragments = 0;
    for (int i = 0; i < indices.size(); i += 3) {
        const Base::vec4& va(coords[indices[i]]);
        const Base::vec4& vb(coords[indices[i + 1]]);
        const Base::vec4& vc(coords[indices[i + 2]]);
        const uint8_t* aData = &vertexData[indices[i] * vInFormat.size];
        const uint8_t* bData = &vertexData[indices[i + 1] * vInFormat.size];
        const uint8_t* cData = &vertexData[indices[i + 2] * vInFormat.size];
        
        // find transformed world coords, need them twice
        Base::vec4 aWorld = m_modelViewMatrix * va;
        Base::vec4 bWorld = m_modelViewMatrix * vb;
        Base::vec4 cWorld = m_modelViewMatrix * vc;

        // backface culling
        if (m_backFaceCulling != CullingMode::None) {
            Temple::Base::vec4 side0, side1, outsideNormal;
            switch (m_backFaceCulling) {
            case CullingMode::ClockWise:
                side0 = cWorld - aWorld;
                side1 = bWorld - cWorld;
                outsideNormal = side0.cross(side1);
                break;
            case CullingMode::CounterClockWise:
            default:
                side0 = bWorld - aWorld;
                side1 = cWorld - bWorld;
                outsideNormal = side0.cross(side1);
                break;
            }
            const Base::vec3 z { 0.0f, 0.0f, 1.0f };
            Base::vec3 n { outsideNormal.x, outsideNormal.y, outsideNormal.z };
            n = n.norm();
            if (z.dot(n) > 0.0f) {
                continue;
            }
        }
        // occlussion culling
        /*
        if (outOfSight(aWorld, bWorld, cWorld, )) {
            continue;
        }
        */
        //
        Base::vec4 a, b, c;
        std::vector<uint8_t> aVertexOut(vOutFormat.size), bVertexOut(vOutFormat.size), cVertexOut(vOutFormat.size);
        this->m_vsf(va, a, aVertexOut, aData, this->m_builtins, this->m_descriptorSet);
        this->m_vsf(vb, b, bVertexOut, bData, this->m_builtins, this->m_descriptorSet);
        this->m_vsf(vc, c, cVertexOut, cData, this->m_builtins, this->m_descriptorSet);
        //
        uint8_t* aVertexOutPtr = aVertexOut.data();
        uint8_t* bVertexOutPtr = bVertexOut.data();
        uint8_t* cVertexOutPtr = cVertexOut.data();
        //
        a = processVertex(a);
        b = processVertex(b);
        c = processVertex(c);
        //
        Base::vec4 sa = a, sb = b, sc = c;
        if (sa.y > sc.y) {
            std::swap(sa, sc);
            std::swap(aVertexOutPtr, cVertexOutPtr);
        }
        if (sa.y > sb.y) {
            std::swap(sa, sb);
            std::swap(aVertexOutPtr, bVertexOutPtr);
        }
        if (sb.y > sc.y) {
            std::swap(sb, sc);
            std::swap(bVertexOutPtr, cVertexOutPtr);
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
        std::vector<uint8_t> vInterpolatedAC(vOutFormat.size), vInterpolatedAB(vOutFormat.size), vInterpolatedBC(vOutFormat.size), vInterpolatedLR(vOutFormat.size);
        std::vector<uint8_t> vDepthedA(vOutFormat.size), vDepthedB(vOutFormat.size), vDepthedC(vOutFormat.size), vDepthedP(vOutFormat.size);
        // divide attributes by original z - lesser attributes, that are located further
        multiplyAttributes(aVertexOutPtr, vDepthedA.data(), sa.w, vOutFormat);
        multiplyAttributes(bVertexOutPtr, vDepthedB.data(), sb.w, vOutFormat);
        multiplyAttributes(cVertexOutPtr, vDepthedC.data(), sc.w, vOutFormat);
        //
        if (leftToRight) {
            Base::vec4 c;
            for (idx = 0; idx < xab.size() % (n + 1); idx++) {
                float leftx = xab[idx];
                float rightx = xac[idx];
                float vWeightAB = getWeightBetween(leftx, bottomy, sa.x, sa.y, sb.x, sb.y);
                float vWeightAC = getWeightBetween(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
                interpolateAttributes(vDepthedA.data(), vDepthedB.data(), &vInterpolatedAB[0], vWeightAB, vOutFormat);
                interpolateAttributes(vDepthedA.data(), vDepthedC.data(), &vInterpolatedAC[0], vWeightAC, vOutFormat);
                float lz = sa.z + (sb.z - sa.z) * vWeightAB;
                float rz = sa.z + (sc.z - sa.z) * vWeightAC;
                float lwo = sa.w + (sb.w - sa.w) * vWeightAB;
                float rwo = sa.w + (sc.w - sa.w) * vWeightAC;
                while (leftx < rightx) {
                    float hWeight = (leftx - xab[idx]) / (xac[idx] - xab[idx]);
                    interpolateAttributes(vInterpolatedAB.data(), vInterpolatedAC.data(), &vInterpolatedLR[0], hWeight, vOutFormat);
                    c.x = leftx;
                    c.y = bottomy;
                    c.z = lz + (rz - lz) * hWeight;
                    float pwo = lwo + (rwo - lwo) * hWeight;
                    float pzo = 1.0f / pwo;
                    c.w = pwo;
                    multiplyAttributes(vInterpolatedLR.data(), vDepthedP.data(), pzo, vOutFormat);
                    renderPixelDepthWise(c, vDepthedP.data());
                    leftx += 1.0f;
                }
                bottomy += 1.0f;
            }
            for (; idx < n; idx++) {
                float leftx = xbc[idx - xab.size()];
                float rightx = xac[idx];
                float vWeightBC = getWeightBetween(leftx, bottomy, sb.x, sb.y, sc.x, sc.y);
                float vWeightAC = getWeightBetween(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
                interpolateAttributes(vDepthedB.data(), vDepthedC.data(), &vInterpolatedBC[0], vWeightBC, vOutFormat);
                interpolateAttributes(vDepthedA.data(), vDepthedC.data(), &vInterpolatedAC[0], vWeightAC, vOutFormat);
                float lz = sb.z + (sc.z - sb.z) * vWeightBC;
                float rz = sa.z + (sc.z - sa.z) * vWeightAC;
                float lwo = sb.w + (sc.w - sb.w) * vWeightBC;
                float rwo = sa.w + (sc.w - sa.w) * vWeightAC;
                while (leftx < rightx) {
                    float hWeight = (leftx - xbc[idx - xab.size()]) / (xac[idx] - xbc[idx - xab.size()]);
                    interpolateAttributes(vInterpolatedBC.data(), vInterpolatedAC.data(), &vInterpolatedLR[0], hWeight, vOutFormat);
                    c.x = leftx;
                    c.y = bottomy;
                    c.z = lz + (rz - lz) * hWeight;
                    float pwo = lwo + (rwo - lwo) * hWeight;
                    float pzo = 1.0f / pwo;
                    c.w = pwo;
                    multiplyAttributes(vInterpolatedLR.data(), vDepthedP.data(), pzo, vOutFormat);
                    renderPixelDepthWise(c, vDepthedP.data());
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
                interpolateAttributes(vDepthedA.data(), vDepthedB.data(), &vInterpolatedAB[0], vWeightAB, vOutFormat);
                interpolateAttributes(vDepthedA.data(), vDepthedC.data(), &vInterpolatedAC[0], vWeightAC, vOutFormat);
                float lz = sa.z + (sc.z - sa.z) * vWeightAC;
                float rz = sa.z + (sb.z - sa.z) * vWeightAB;
                float lwo = sa.w + (sc.w - sa.w) * vWeightAC;
                float rwo = sa.w + (sb.w - sa.w) * vWeightAB;
                while (leftx < rightx) {
                    float hWeight = (leftx - xac[idx]) / (xab[idx] - xac[idx]);
                    interpolateAttributes(vInterpolatedAC.data(), vInterpolatedAB.data(), &vInterpolatedLR[0], hWeight, vOutFormat);
                    c.x = leftx;
                    c.y = bottomy;
                    c.z = lz + (rz - lz) * hWeight;
                    float pwo = lwo + (rwo - lwo) * hWeight;
                    float pzo = 1.0f / pwo;
                    c.w = pwo;
                    multiplyAttributes(vInterpolatedLR.data(), vDepthedP.data(), pzo, vOutFormat);
                    renderPixelDepthWise(c, vDepthedP.data());
                    leftx += 1.0f;
                }
                bottomy += 1.0f;
            }
            for (; idx < n; idx++) {
                float leftx = xac[idx];
                float rightx = xbc[idx - xab.size()];
                float vWeightAC = getWeightBetween(leftx, bottomy, sa.x, sa.y, sc.x, sc.y);
                float vWeightBC = getWeightBetween(rightx, bottomy, sb.x, sb.y, sc.x, sc.y);
                interpolateAttributes(vDepthedB.data(), vDepthedC.data(), &vInterpolatedBC[0], vWeightBC, vOutFormat);
                interpolateAttributes(vDepthedA.data(), vDepthedC.data(), &vInterpolatedAC[0], vWeightAC, vOutFormat);
                float lz = sa.z + (sc.z - sa.z) * vWeightAC;
                float rz = sb.z + (sc.z - sb.z) * vWeightBC;
                float lwo = sa.w + (sc.w - sa.w) * vWeightAC;
                float rwo = sb.w + (sc.w - sb.w) * vWeightBC;
                while (leftx < rightx) {
                    float hWeight = (leftx - xac[idx]) / (xbc[idx - xab.size()] - xac[idx]);
                    interpolateAttributes(vInterpolatedAC.data(), vInterpolatedBC.data(), &vInterpolatedLR[0], hWeight, vOutFormat);
                    c.x = leftx;
                    c.y = bottomy;
                    c.z = lz + (rz - lz) * hWeight;
                    float pwo = lwo + (rwo - lwo) * hWeight;
                    float pzo = 1.0f / pwo;
                    c.w = pwo;
                    multiplyAttributes(vInterpolatedLR.data(), vDepthedP.data(), pzo, vOutFormat);
                    renderPixelDepthWise(c, vDepthedP.data());
                    leftx += 1.0f;
                }
                bottomy += 1.0f;
            }
        }
        fragments++;
    }
    if (m_log != nullptr) {
        static char msg[128];
        int iwr = snprintf(msg, 128, "triangles drawn: %d \n", fragments);
        m_log(msg, iwr);
    }
}

void Temple::Bonfire::RenderContext::setLogFunc(logFunc log) {
    m_log = log;
}

Temple::Bonfire::RenderContext::~RenderContext() {
    delete[] m_data;
    m_data = nullptr;
}
