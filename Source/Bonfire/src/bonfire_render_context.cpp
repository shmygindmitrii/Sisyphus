#include "bonfire_render_context.h"
#include "base_utils.h"

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>

Temple::Bonfire::VertexFormat::VertexFormat(
  const std::vector<Temple::Bonfire::EVertexAttribType>& attribs)
{
  size = 0;
  for (int i = 0; i < attribs.size(); i++)
  {
    attributes.push_back(attribs[i]);
    switch (attribs[i])
    {
    case EVertexAttribType::FLOAT32:
      size += 4;
      break;
    case EVertexAttribType::INT32:
      size += 4;
      break;
    case EVertexAttribType::UINT8:
      size += 1;
      break;
    case EVertexAttribType::VEC2:
      size += 8;
      break;
    case EVertexAttribType::VEC3:
      size += 12;
      break;
    case EVertexAttribType::VEC4:
      size += 16;
      break;
    case EVertexAttribType::UV:
      size += 8;
      break;
    }
  }
}

template <typename T>
static inline void
__interpolateAttribsTemplate(
  const uint8_t*& aIn,
  const uint8_t*& bIn,
  uint8_t*&       cOut,
  float           weight)
{
  *(T*)cOut = (*(T*)bIn) * weight - (*(T*)aIn) * weight + *(T*)aIn;
  aIn += sizeof(T);
  bIn += sizeof(T);
  cOut += sizeof(T);
}

void
Temple::Bonfire::interpolateAttributes(
  const uint8_t*      aIn,
  const uint8_t*      bIn,
  uint8_t*            cOut,
  float               weight,
  const VertexFormat& vf)
{
  // cOut should be enough to hold aIn or bIn (they are the same in terms of
  // size) weight is always from 0 to 1
  for (int i = 0; i < vf.attributes.size(); i++)
  {
    switch (vf.attributes[i])
    {
    case EVertexAttribType::FLOAT32:
      __interpolateAttribsTemplate<float>(aIn, bIn, cOut, weight);
      break;
    case EVertexAttribType::INT32:
      __interpolateAttribsTemplate<int32_t>(aIn, bIn, cOut, weight);
      break;
    case EVertexAttribType::UINT8:
      __interpolateAttribsTemplate<uint8_t>(aIn, bIn, cOut, weight);
      break;
    case EVertexAttribType::VEC2:
      __interpolateAttribsTemplate<Base::vec2_t>(aIn, bIn, cOut, weight);
      break;
    case EVertexAttribType::VEC3:
      __interpolateAttribsTemplate<Base::vec3_t>(aIn, bIn, cOut, weight);
      break;
    case EVertexAttribType::VEC4:
      __interpolateAttribsTemplate<Base::vec4_t>(aIn, bIn, cOut, weight);
      break;
    case EVertexAttribType::UV:
      __interpolateAttribsTemplate<Base::vec2_t>(aIn, bIn, cOut, weight);
      break;
    }
  }
}

template <typename T>
static inline void
__multAttribsTemplate(const uint8_t*& aIn, uint8_t*& cOut, float mult)
{
  *(T*)cOut = (*(T*)aIn) * mult;
  aIn += sizeof(T);
  cOut += sizeof(T);
}

void
Temple::Bonfire::multiplyAttributes(
  const uint8_t*      aIn,
  uint8_t*            cOut,
  float               mult,
  const VertexFormat& vf)
{
  for (int i = 0; i < vf.attributes.size(); i++)
  {
    switch (vf.attributes[i])
    {
    case EVertexAttribType::FLOAT32:
      __multAttribsTemplate<float>(aIn, cOut, mult);
      break;
    case EVertexAttribType::INT32:
      __multAttribsTemplate<int32_t>(aIn, cOut, mult);
      break;
    case EVertexAttribType::UINT8:
      __multAttribsTemplate<uint8_t>(aIn, cOut, mult);
      break;
    case EVertexAttribType::VEC2:
      __multAttribsTemplate<Base::vec2_t>(aIn, cOut, mult);
      break;
    case EVertexAttribType::VEC3:
      __multAttribsTemplate<Base::vec3_t>(aIn, cOut, mult);
      break;
    case EVertexAttribType::VEC4:
      __multAttribsTemplate<Base::vec4_t>(aIn, cOut, mult);
      break;
    case EVertexAttribType::UV:
      __multAttribsTemplate<Base::vec2_t>(aIn, cOut, mult);
      break;
    }
  }
}

Temple::Bonfire::Plane::Plane()
    : normal(Base::vec3_t {0.0f, 0.0f, 1.0f})
    , offset(0.0f)
{}

Temple::Bonfire::Plane::Plane(const Base::vec3_t& _normal, float _offset)
    : normal(_normal)
    , offset(_offset)
{}

Temple::Bonfire::RenderContext::RenderContext(
  int width,
  int height,
  int bytesPerPixel)
    : m_width(width)
    , m_height(height)
    , m_bytesPerPixel(bytesPerPixel)
    , m_depthWrite(true)
    , m_depthTest(true)
{
  size_t fullSize = m_width * m_height * (size_t)m_bytesPerPixel;
  assert(fullSize > 0);
  m_data = new uint8_t[fullSize];
  m_depth = new float[m_width * m_height];
  m_builtins.resize(sizeof(Base::mat4) * 5);
}

const uint8_t*
Temple::Bonfire::RenderContext::getFrame() const
{
  return m_data;
}

void
Temple::Bonfire::RenderContext::resize(int width, int height, int bytesPerPixel)
{
  size_t oldResolution = m_width * m_height;
  size_t oldSize = oldResolution * (size_t)m_bytesPerPixel;
  m_width = width;
  m_height = height;
  m_bytesPerPixel = bytesPerPixel;
  size_t curResolution = m_width * m_height;
  size_t fullSize = curResolution * (size_t)m_bytesPerPixel;
  assert(fullSize >= 0);
  if (fullSize != oldSize)
  {
    if (m_data != nullptr)
    {
      delete[] m_data;
      m_data = nullptr;
    }
    if (fullSize > 0)
    {
      m_data = new uint8_t[fullSize];
    }
  }
  if (curResolution != oldResolution)
  {
    if (m_depth != nullptr)
    {
      delete[] m_depth;
      m_depth = nullptr;
    }
    if (curResolution > 0)
    {
      m_depth = new float[curResolution];
    }
  }
}

void
Temple::Bonfire::RenderContext::setViewport(
  float xMin,
  float yMin,
  float zMin,
  float xMax,
  float yMax,
  float zMax)
{
  m_viewportMin.x = xMin;
  m_viewportMin.y = yMin;
  m_viewportMin.z = zMin;

  m_viewportMax.x = xMax;
  m_viewportMax.y = yMax;
  m_viewportMax.z = zMax;
}

void
Temple::Bonfire::RenderContext::setDescriptorSet(
  const std::vector<uint8_t>& descriptorSet)
{
  m_descriptorSet = descriptorSet;
}

void
Temple::Bonfire::RenderContext::setVertexShader(
  Temple::Bonfire::vertexShaderFunc vsf)
{
  m_vsf = vsf;
}

void
Temple::Bonfire::RenderContext::setPixelShader(
  Temple::Bonfire::pixelShaderFunc psf)
{
  m_psf = psf;
}

void
Temple::Bonfire::RenderContext::setModelMatrix(const Base::mat4& m)
{
  m_modelMatrix = m;
  m_modelViewMatrix = m_viewMatrix * m_modelMatrix;
  m_transformMatrix = m_perspectiveMatrix * m_modelViewMatrix;
  Base::replaceData(m_builtins, m_modelMatrix, 0);
  Base::replaceData(m_builtins, m_modelViewMatrix, sizeof(Base::mat4) * 3);
  Base::replaceData(m_builtins, m_transformMatrix, sizeof(Base::mat4) * 4);
}

void
Temple::Bonfire::RenderContext::setViewMatrix(const Base::mat4& m)
{
  m_viewMatrix = m;
  m_modelViewMatrix = m_viewMatrix * m_modelMatrix;
  m_transformMatrix = m_perspectiveMatrix * m_modelViewMatrix;
  Base::replaceData(m_builtins, m_viewMatrix, sizeof(Base::mat4));
  Base::replaceData(m_builtins, m_modelViewMatrix, sizeof(Base::mat4) * 3);
  Base::replaceData(m_builtins, m_transformMatrix, sizeof(Base::mat4) * 4);
}

void
Temple::Bonfire::RenderContext::setPerspectiveMatrix(const Base::mat4& m)
{
  m_perspectiveMatrix = m;
  m_transformMatrix = m_perspectiveMatrix * m_modelViewMatrix;
  Base::replaceData(m_builtins, m_perspectiveMatrix, sizeof(Base::mat4) * 2);
  Base::replaceData(m_builtins, m_transformMatrix, sizeof(Base::mat4) * 4);
}

void
Temple::Bonfire::RenderContext::setPerspective(
  float fov,
  float aspect,
  float znear,
  float zfar)
{
  fov = fov * Temple::Base::PI / 180.0f;
  Base::mat4 perspectiveMatrix =
    Base::mat4::projection(fov, aspect, znear, zfar);
  this->setFrustum(fov, aspect, znear, zfar);
  this->setPerspectiveMatrix(perspectiveMatrix);
}

void
Temple::Bonfire::RenderContext::setFrustum(
  float fov,
  float aspect,
  float znear,
  float zfar)
{
  // create 6 planes that forms frustum in view coordinates
  // znear plane
  Base::vec3_t& znerNormal = m_frustum.bounds[0].normal;
  znerNormal.x = 0.0f;
  znerNormal.y = 0.0f;
  znerNormal.z = -1.0f;
  m_frustum.bounds[0].offset = znear; // depends on direction of normal
  // zfar plane
  Base::vec3_t& zfarNormal = m_frustum.bounds[1].normal;
  zfarNormal.x = 0.0f;
  zfarNormal.y = 0.0f;
  zfarNormal.z = 1.0f;
  m_frustum.bounds[1].offset = -zfar;
  // points on planes - here we have world coordinates where y is going from up
  // to down
  Base::vec3_t b {0.0f, znear * tanf(0.5f * fov), znear};
  Base::vec3_t r {znear * tanf(0.5f * fov) * aspect, 0.0f, znear};
  Base::vec3_t t {0.0f, -b.y, znear};
  Base::vec3_t l {-r.x, 0.0f, znear};
  // top plane
  Base::vec3_t& topNormal = m_frustum.bounds[2].normal;
  topNormal.x = 0.0f;
  topNormal.y = -cosf(0.5f * fov);
  topNormal.z = -sinf(0.5f * fov);
  m_frustum.bounds[2].offset = -topNormal.dot(t);
  // bottom plane
  Base::vec3_t& bottomNormal = m_frustum.bounds[3].normal;
  bottomNormal.x = 0.0f;
  bottomNormal.y = -topNormal.y;
  bottomNormal.z = topNormal.z;
  m_frustum.bounds[3].offset = -bottomNormal.dot(b);
  // left plane
  float         horHalfAngle = atanf(aspect * b.y / znear);
  Base::vec3_t& leftNormal = m_frustum.bounds[4].normal;
  leftNormal.x = -cosf(horHalfAngle);
  leftNormal.y = 0.0f;
  leftNormal.z = -sinf(horHalfAngle);
  m_frustum.bounds[4].offset = -leftNormal.dot(l);
  // right plane
  Base::vec3_t& rightNormal = m_frustum.bounds[5].normal;
  rightNormal.x = -leftNormal.x;
  rightNormal.y = 0.0f;
  rightNormal.z = leftNormal.z;
  m_frustum.bounds[5].offset = -rightNormal.dot(r);
}

bool
Temple::Bonfire::RenderContext::outOfSight(
  const Base::vec4_t& a,
  const Base::vec4_t& b,
  const Base::vec4_t& c,
  float               znear,
  float               zfar,
  float               aspect)
{
  if (
    (a.z > zfar && b.z > zfar && c.z > zfar) ||
    (a.z < znear && b.z < znear && c.z < znear))
  {
    // whole triangle is too close or too far
    return true;
  }
  return false;
}

void
Temple::Bonfire::RenderContext::putPixel(
  int                 x,
  int                 y,
  const Base::vec4_t& color)
{
  // color - from 0 to 1 each component
  if (x < 0 || x >= m_width || y < 0 || y >= m_height)
  {
    return;
  }
  int pixelIndex = y * m_width * m_bytesPerPixel + x * m_bytesPerPixel;
  // please, take care about color content in advance - clamp if needed
  m_data[pixelIndex + 0] = (uint8_t)(color.b * 255.0f);
  m_data[pixelIndex + 1] = (uint8_t)(color.g * 255.0f);
  m_data[pixelIndex + 2] = (uint8_t)(color.r * 255.0f);
  m_data[pixelIndex + 3] = (uint8_t)(color.a * 255.0f);
}

void
Temple::Bonfire::RenderContext::fill(const col4u& color)
{
  uint32_t colors = m_width * m_height * m_bytesPerPixel;
  for (uint32_t i = 0; i < colors; i += m_bytesPerPixel)
  { // fake m_bytesPerPixel - now always 4 and maybe will always be 4
    m_data[i + 0] = color.b;
    m_data[i + 1] = color.g;
    m_data[i + 2] = color.r;
    m_data[i + 3] = color.a;
  }
}

Temple::Base::vec4_t
Temple::Bonfire::RenderContext::processVertex(const Base::vec4_t& v)
{
  Base::vec4_t c = m_perspectiveMatrix * v;

  float w = c.w;
  c.w = 1.0f;
  c = c / w;

  Base::vec3_t crd {c.x, -c.y, c.z};
  crd = (crd + 1.0f) * 0.5f * (m_viewportMax - m_viewportMin) + m_viewportMin;

  return Base::vec4_t {crd.x, crd.y, crd.z, c.w};
}

void
Temple::Bonfire::RenderContext::setDepthTest(bool flag)
{
  m_depthTest = flag;
}

void
Temple::Bonfire::RenderContext::setDepthWrite(bool flag)
{
  m_depthWrite = flag;
}

void
Temple::Bonfire::RenderContext::setBackfaceCulling(ECullingMode mode)
{
  m_backFaceCulling = mode;
}

void
Temple::Bonfire::RenderContext::clearDepth(float val)
{
  for (int i = 0; i < m_width * m_height; i++)
  {
    m_depth[i] = val;
  }
}

void
Temple::Bonfire::RenderContext::renderPixelDepthWise(
  const Base::vec4_t& p,
  const uint8_t*      data)
{
  int pixFlatIdx = ((int)p.y) * m_width + (int)p.x;
  if (pixFlatIdx >= 0 && pixFlatIdx < m_width * m_height)
  {
    if (m_depthTest)
    {
      if (p.z > m_depth[pixFlatIdx])
      {
        this->putPixel(
          p.x,
          p.y,
          this->m_psf(p, data, this->m_builtins, this->m_descriptorSet));
      }
    }
    else
    {
      this->putPixel(
        p.x,
        p.y,
        this->m_psf(p, data, this->m_builtins, this->m_descriptorSet));
    }
    if (m_depthWrite)
    {
      if (p.z > m_depth[pixFlatIdx])
      {
        m_depth[pixFlatIdx] = p.z;
      }
    }
  }
}

static inline float
getWeightBetween(float x, float y, float x0, float y0, float x1, float y1)
{
  float dx = x1 - x0;
  float dy = y1 - y0;
  if (fabs(dx) < fabs(dy))
  {
    return (y - y0) / (y1 - y0);
  }
  else
  {
    if (EQUAL_FLOATS(x0, x1))
    {
      return 0.0f;
    }
    return (x - x0) / (x1 - x0);
  }
}

static float
segmentPlaneIntersection(
  const Temple::Base::vec3_t&   a,
  const Temple::Base::vec3_t&   b,
  const Temple::Bonfire::Plane& p)
{
  const Temple::Base::vec3_t ab = b - a;
  float k = (-p.normal.dot(a) - p.offset) / p.normal.dot(ab);
  return k;
}

static inline float
pointPlaneSide(const Temple::Base::vec3_t& a, const Temple::Bonfire::Plane& p)
{
  // -value - under
  // 0 - on
  // +value - above
  // literally put this into the plane equation
  return p.normal.dot(a) + p.offset;
}

static void
cullTriangleTwoPointOutside(
  const Temple::Base::vec4_t&          a,
  const Temple::Base::vec4_t&          b,
  const Temple::Base::vec4_t&          c,
  const uint8_t*                       pDataA,
  const uint8_t*                       pDataB,
  const uint8_t*                       pDataC,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passedVertexCoords,
  std::vector<uint8_t>&                passedVertexData)
{
  // a and b are outside
  float cSide = pointPlaneSide(c.xyz, p);
  if (fabs(cSide) <= Temple::Base::EPS)
  {
    // c is on the plane, discard
  }
  else
  {
    // two outside and one point is inside
    float                kA = segmentPlaneIntersection(c.xyz, a.xyz, p);
    float                kB = segmentPlaneIntersection(c.xyz, b.xyz, p);
    std::vector<uint8_t> dataAK = {};
    std::vector<uint8_t> dataBK = {};
    dataAK.resize(vf.size);
    dataBK.resize(vf.size);
    Temple::Bonfire::interpolateAttributes(
      pDataC,
      pDataA,
      dataAK.data(),
      kA,
      vf);
    Temple::Bonfire::interpolateAttributes(
      pDataC,
      pDataB,
      dataBK.data(),
      kB,
      vf);
    Temple::Base::vec4_t ak = (a - c) * kA + c;
    ak.w = c.w;
    Temple::Base::vec4_t bk = (b - c) * kB + c;
    bk.w = b.w;
    //
    passedVertexCoords.emplace_back(c);
    passedVertexCoords.emplace_back(ak);
    passedVertexCoords.emplace_back(bk);
    Temple::Base::appendData(passedVertexData, pDataC, vf.size, 0);
    Temple::Base::appendData(passedVertexData, dataAK.data(), vf.size, 0);
    Temple::Base::appendData(passedVertexData, dataBK.data(), vf.size, 0);
  }
}

static void
cullTriangleOnePointOutsideOneOn(
  const Temple::Base::vec4_t&          a,
  const Temple::Base::vec4_t&          b,
  const Temple::Base::vec4_t&          c,
  const uint8_t*                       pDataA,
  const uint8_t*                       pDataB,
  const uint8_t*                       pDataC,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passedVertexCoords,
  std::vector<uint8_t>&                passedVertexData)
{
  // a is outside and b is on the plane
  float cSide = pointPlaneSide(c.xyz, p);
  if (fabs(cSide) <= Temple::Base::EPS)
  {
    // only segment is on the plane, we can discard it
  }
  else
  {
    // a is outside, b is on the plane, c is inside
    float                kA = segmentPlaneIntersection(c.xyz, a.xyz, p);
    std::vector<uint8_t> dataCK = {};
    dataCK.resize(vf.size);
    Temple::Bonfire::interpolateAttributes(
      pDataC,
      pDataA,
      dataCK.data(),
      kA,
      vf);
    Temple::Base::vec4_t ck = (a - c) * kA + c;
    ck.w = c.w;
    //
    passedVertexCoords.emplace_back(b);
    passedVertexCoords.emplace_back(c);
    passedVertexCoords.emplace_back(ck);
    Temple::Base::appendData(passedVertexData, pDataB, vf.size, 0);
    Temple::Base::appendData(passedVertexData, pDataC, vf.size, 0);
    Temple::Base::appendData(passedVertexData, dataCK.data(), vf.size, 0);
  }
}

static void
cullTriangleOnePointOutside(
  const Temple::Base::vec4_t&          a,
  const Temple::Base::vec4_t&          b,
  const Temple::Base::vec4_t&          c,
  const uint8_t*                       pDataA,
  const uint8_t*                       pDataB,
  const uint8_t*                       pDataC,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passedVertexCoords,
  std::vector<uint8_t>&                passedVertexData)
{
  // a-point is outside
  float bSide = pointPlaneSide(b.xyz, p);
  float cSide = pointPlaneSide(c.xyz, p);
  if (bSide > 0.0f)
  {
    // a and b are outside
    cullTriangleTwoPointOutside(
      a,
      b,
      c,
      pDataA,
      pDataB,
      pDataC,
      vf,
      p,
      passedVertexCoords,
      passedVertexData);
  }
  else
  {
    if (cSide > 0.0f)
    {
      // a and c are outside
      cullTriangleTwoPointOutside(
        a,
        c,
        b,
        pDataA,
        pDataC,
        pDataB,
        vf,
        p,
        passedVertexCoords,
        passedVertexData);
    }
    else
    {
      // only one point is really outside
      if (fabs(bSide) < Temple::Base::EPS)
      {
        // b is on the plane
        cullTriangleOnePointOutsideOneOn(
          a,
          b,
          c,
          pDataA,
          pDataB,
          pDataC,
          vf,
          p,
          passedVertexCoords,
          passedVertexData);
      }
      else if (fabs(cSide) < Temple::Base::EPS)
      {
        // c is on the plane
        cullTriangleOnePointOutsideOneOn(
          a,
          c,
          b,
          pDataA,
          pDataC,
          pDataB,
          vf,
          p,
          passedVertexCoords,
          passedVertexData);
      }
      else
      {
        // no point is on the plane, just one outside and two inside
        float                kB = segmentPlaneIntersection(b.xyz, a.xyz, p);
        float                kC = segmentPlaneIntersection(c.xyz, a.xyz, p);
        std::vector<uint8_t> dataBK = {};
        std::vector<uint8_t> dataCK = {};
        dataBK.resize(vf.size);
        dataCK.resize(vf.size);
        Temple::Bonfire::interpolateAttributes(
          pDataB,
          pDataA,
          dataBK.data(),
          kB,
          vf);
        Temple::Bonfire::interpolateAttributes(
          pDataC,
          pDataA,
          dataCK.data(),
          kC,
          vf);
        Temple::Base::vec4_t bk = (a - b) * kB + b;
        bk.w = b.w;
        Temple::Base::vec4_t ck = (a - c) * kC + c;
        ck.w = c.w;
        //
        passedVertexCoords.emplace_back(b);
        passedVertexCoords.emplace_back(c);
        passedVertexCoords.emplace_back(bk);
        Temple::Base::appendData(passedVertexData, pDataB, vf.size, 0);
        Temple::Base::appendData(passedVertexData, pDataC, vf.size, 0);
        Temple::Base::appendData(passedVertexData, dataBK.data(), vf.size, 0);
        //
        passedVertexCoords.emplace_back(c);
        passedVertexCoords.emplace_back(ck);
        passedVertexCoords.emplace_back(bk);
        Temple::Base::appendData(passedVertexData, pDataC, vf.size, 0);
        Temple::Base::appendData(passedVertexData, dataCK.data(), vf.size, 0);
        Temple::Base::appendData(passedVertexData, dataBK.data(), vf.size, 0);
      }
    }
  }
}

static void
cullTriangleByPlane(
  const Temple::Base::vec4_t&          a,
  const Temple::Base::vec4_t&          b,
  const Temple::Base::vec4_t&          c,
  const uint8_t*                       pDataA,
  const uint8_t*                       pDataB,
  const uint8_t*                       pDataC,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passedVertexCoords,
  std::vector<uint8_t>&                passedVertexData)
{
  float aSide = pointPlaneSide(a.xyz, p);
  float bSide = pointPlaneSide(b.xyz, p);
  float cSide = pointPlaneSide(c.xyz, p);
  // here we have many possible cases
  if (aSide <= 0.0f && bSide <= 0.0f && cSide <= 0.0f)
  {
    // everything is under the plane, add whole triangle
    passedVertexCoords.emplace_back(a);
    passedVertexCoords.emplace_back(b);
    passedVertexCoords.emplace_back(c);
    Temple::Base::appendData(passedVertexData, pDataA, vf.size, 0);
    Temple::Base::appendData(passedVertexData, pDataB, vf.size, 0);
    Temple::Base::appendData(passedVertexData, pDataC, vf.size, 0);
  }
  else if (aSide > 0.0f && bSide > 0.0f && cSide > 0.0f)
  {
    // skip, whole triangle is above
  }
  else
  {
    // here is intersection
    if (aSide > 0.0f)
    {
      cullTriangleOnePointOutside(
        a,
        b,
        c,
        pDataA,
        pDataB,
        pDataC,
        vf,
        p,
        passedVertexCoords,
        passedVertexData);
    }
    else if (bSide > 0.0f)
    {
      cullTriangleOnePointOutside(
        b,
        a,
        c,
        pDataB,
        pDataA,
        pDataC,
        vf,
        p,
        passedVertexCoords,
        passedVertexData);
    }
    else if (cSide > 0.0f)
    {
      cullTriangleOnePointOutside(
        c,
        a,
        b,
        pDataC,
        pDataA,
        pDataB,
        vf,
        p,
        passedVertexCoords,
        passedVertexData);
    }
    else
    {
      // impossible to reach, at least one point should be outside
      assert(false);
    }
  }
}

static void
cullTrianglesByPlane(
  std::vector<Temple::Base::vec4_t>&   inputVertexCoords,
  const uint8_t*                       pData,
  const Temple::Bonfire::VertexFormat& vf,
  const Temple::Bonfire::Plane&        p,
  std::vector<Temple::Base::vec4_t>&   passedVertexCoords,
  std::vector<uint8_t>&                passedVertexData)
{
  for (int i = 0; i < inputVertexCoords.size(); i += 3)
  {
    const Temple::Base::vec4_t& a = inputVertexCoords[i];
    const Temple::Base::vec4_t& b = inputVertexCoords[i + 1];
    const Temple::Base::vec4_t& c = inputVertexCoords[i + 2];
    const uint8_t*              pDataA = pData + i * vf.size;
    const uint8_t*              pDataB = pData + (i + 1) * vf.size;
    const uint8_t*              pDataC = pData + (i + 2) * vf.size;
    cullTriangleByPlane(
      a,
      b,
      c,
      pDataA,
      pDataB,
      pDataC,
      vf,
      p,
      passedVertexCoords,
      passedVertexData);
  }
}

void
Temple::Bonfire::RenderContext::cullTriangleByFrustum(
  const Base::vec4_t&        a,
  const Base::vec4_t&        b,
  const Base::vec4_t&        c,
  const uint8_t*             aData,
  const uint8_t*             bData,
  const uint8_t*             cData,
  const VertexFormat&        vf,
  std::vector<Base::vec4_t>& passedVertexCoords,
  std::vector<uint8_t>&      passedVertexData)
{
  std::vector<Base::vec4_t> vertexPassedFront = {a, b, c};
  std::vector<uint8_t>      dataPassedFront = {};
  Base::appendData(dataPassedFront, aData, vf.size, 0);
  Base::appendData(dataPassedFront, bData, vf.size, 0);
  Base::appendData(dataPassedFront, cData, vf.size, 0);
  std::vector<Base::vec4_t> vertexPassedBack = {};
  std::vector<uint8_t>      dataPassedBack = {};

  std::vector<Base::vec4_t>* pVertexPassedFront = &vertexPassedFront;
  std::vector<Base::vec4_t>* pVertexPassedBack = &vertexPassedBack;
  std::vector<uint8_t>*      pDataPassedFront = &dataPassedFront;
  std::vector<uint8_t>*      pDataPassedBack = &dataPassedBack;

  for (int i = 0; i < 6; i++)
  {
    const Bonfire::Plane& p = this->m_frustum.bounds[i];
    pVertexPassedBack->clear();
    pDataPassedBack->clear();
    cullTrianglesByPlane(
      *pVertexPassedFront,
      pDataPassedFront->data(),
      vf,
      p,
      *pVertexPassedBack,
      *pDataPassedBack);
    std::swap(pVertexPassedFront, pVertexPassedBack);
    std::swap(pDataPassedFront, pDataPassedBack);
  }
  for (int i = 0; i < pVertexPassedFront->size(); i++)
  {
    passedVertexCoords.emplace_back((*pVertexPassedFront)[i]);
    Temple::Base::appendData(
      passedVertexData,
      pDataPassedFront->data() + (vf.size * i),
      vf.size,
      0);
  }
}

void
Temple::Bonfire::RenderContext::drawLines(
  const std::vector<Base::vec4_t>& coords,
  const std::vector<int>&          indices,
  const uint8_t*                   vertexData,
  const VertexFormat&              vInFormat,
  const VertexFormat&              vOutFormat)
{
  if (indices.size() == 0 || indices.size() % 2 != 0)
  {
    return;
  }
  for (int i = 0; i < indices.size(); i += 2)
  {
    const Base::vec4_t& va = coords[indices[i]];
    const Base::vec4_t& vb = coords[indices[i + 1]];
    const uint8_t*      aData = &vertexData[indices[i] * vInFormat.size];
    const uint8_t*      bData = &vertexData[indices[i + 1] * vInFormat.size];
    // draw single line here
    Base::vec4_t         a, b;
    std::vector<uint8_t> aVertexOut(vOutFormat.size),
      bVertexOut(vOutFormat.size);
    this->m_vsf(
      va,
      a,
      aVertexOut,
      aData,
      this->m_builtins,
      this->m_descriptorSet);
    this->m_vsf(
      vb,
      b,
      bVertexOut,
      bData,
      this->m_builtins,
      this->m_descriptorSet);
    //
    a = processVertex(a);
    b = processVertex(b);
    // obtained vertex shader results and go to the pixel stage
    Base::vec4_t a0(a);
    Base::vec4_t b0(b);
    // perspective correct interpolation part - normalize by z first
    std::vector<uint8_t> vDepthedA(vOutFormat.size), vDepthedB(vOutFormat.size);
    uint8_t *vDepthedAPtr = vDepthedA.data(), *vDepthedBPtr = vDepthedB.data();
    multiplyAttributes(aVertexOut.data(), vDepthedAPtr, a0.w, vOutFormat);
    multiplyAttributes(bVertexOut.data(), vDepthedBPtr, b0.w, vOutFormat);
    //
    const uint8_t *aData0 = vDepthedAPtr, *bData0 = vDepthedBPtr;
    if (a.y > b.y)
    {
      std::swap(a0, b0);
      std::swap(aData0, bData0);
    }
    // a is bottom vertex and b is top vertex
    float yDif = b0.y - a0.y;
    float xDif = b0.x - a0.x;
    if (fabs(yDif) < 0.001f && fabs(xDif) < 0.001f)
    {
      // point
      renderPixelDepthWise(a0, aData0);
    }
    else
    {
      std::vector<uint8_t> pixelOut(vOutFormat.size);
      if (fabs(yDif) > fabs(xDif))
      {
        float                slope = xDif / yDif;
        std::vector<uint8_t> cOut(vOutFormat.size);
        Base::vec4_t         c;
        for (c.y = a0.y; c.y < b0.y; c.y += 1.0f)
        {
          c.x = a0.x + (c.y - a0.y) * slope;
          float weight = (c.y - a0.y) / (b0.y - a0.y);
          c.w = (b0.w - a0.w) * weight + a0.w;
          float pzo = 1.0f / c.w;
          c.z = (b0.z - a0.z) * weight + a0.z;
          interpolateAttributes(
            aData0,
            bData0,
            cOut.data(),
            weight,
            vOutFormat);
          multiplyAttributes(cOut.data(), pixelOut.data(), pzo, vOutFormat);
          renderPixelDepthWise(c, pixelOut.data());
        }
      }
      else
      {
        if (a0.x > b0.x)
        {
          std::swap(a0, b0);
          std::swap(aData0, bData0);
        }
        float                slope = yDif / xDif;
        std::vector<uint8_t> cOut(vOutFormat.size);
        Base::vec4_t         c;
        for (c.x = a0.x; c.x < b0.x; c.x += 1.0f)
        {
          c.y = a0.y + (c.x - a0.x) * slope;
          float weight = (c.x - a0.x) / (b0.x - a0.x);
          c.w = (b0.w - a0.w) * weight + a0.w;
          float pzo = 1.0f / c.w;
          c.z = (b0.z - a0.z) * weight + a0.z;
          interpolateAttributes(
            aData0,
            bData0,
            cOut.data(),
            weight,
            vOutFormat);
          multiplyAttributes(cOut.data(), pixelOut.data(), pzo, vOutFormat);
          renderPixelDepthWise(c, pixelOut.data());
        }
      }
    }
  }
}

void
Temple::Bonfire::RenderContext::drawTriangles(
  const std::vector<Base::vec4_t>& coords,
  const std::vector<int>&          indices,
  const uint8_t*                   vertexData,
  const VertexFormat&              vInFormat,
  const VertexFormat&              vOutFormat)
{
  if (indices.size() == 0 || indices.size() % 3 != 0)
  {
    return;
  }
  int fragments = 0;
  for (int i = 0; i < indices.size(); i += 3)
  {
    const Base::vec4_t& va(coords[indices[i]]);
    const Base::vec4_t& vb(coords[indices[i + 1]]);
    const Base::vec4_t& vc(coords[indices[i + 2]]);
    const uint8_t*      aData = &vertexData[indices[i] * vInFormat.size];
    const uint8_t*      bData = &vertexData[indices[i + 1] * vInFormat.size];
    const uint8_t*      cData = &vertexData[indices[i + 2] * vInFormat.size];
    //
    Base::vec4_t         aWorld, bWorld, cWorld;
    std::vector<uint8_t> aVertexOut(vOutFormat.size),
      bVertexOut(vOutFormat.size), cVertexOut(vOutFormat.size);

    this->m_vsf(
      va,
      aWorld,
      aVertexOut,
      aData,
      this->m_builtins,
      this->m_descriptorSet);
    this->m_vsf(
      vb,
      bWorld,
      bVertexOut,
      bData,
      this->m_builtins,
      this->m_descriptorSet);
    this->m_vsf(
      vc,
      cWorld,
      cVertexOut,
      cData,
      this->m_builtins,
      this->m_descriptorSet);

    // backface culling
    if (m_backFaceCulling != ECullingMode::None)
    {
      Temple::Base::vec4_t side0, side1, outsideNormal;
      switch (m_backFaceCulling)
      {
      case ECullingMode::ClockWise:
        side0 = cWorld - aWorld;
        side1 = bWorld - cWorld;
        outsideNormal = side0.cross(side1);
        break;
      case ECullingMode::CounterClockWise:
      default:
        side0 = bWorld - aWorld;
        side1 = cWorld - bWorld;
        outsideNormal = side0.cross(side1);
        break;
      }
      Base::vec4_t z = aWorld + bWorld + cWorld;
      float        zlength = z.magnitude();
      if (zlength < Temple::Base::EPS)
      {
        continue;
      }
      z = z * (1.0f / zlength);
      Base::vec3_t n {outsideNormal.x, outsideNormal.y, outsideNormal.z};
      n = n.norm();
      if (z.xyz.dot(n) > 0.0f)
      {
        continue;
      }
    }
    // frustum culling
    std::vector<uint8_t>      viewPassedVertexData = {};
    std::vector<Base::vec4_t> viewPassedVertexCoords = {};
    this->cullTriangleByFrustum(
      aWorld,
      bWorld,
      cWorld,
      aVertexOut.data(),
      bVertexOut.data(),
      cVertexOut.data(),
      vOutFormat,
      viewPassedVertexCoords,
      viewPassedVertexData);
    // rasterization
    for (int j = 0; j < viewPassedVertexCoords.size(); j += 3)
    {
      const Base::vec4_t& aVisible(viewPassedVertexCoords[j]);
      const Base::vec4_t& bVisible(viewPassedVertexCoords[j + 1]);
      const Base::vec4_t& cVisible(viewPassedVertexCoords[j + 2]);
      //
      uint8_t* aVertexOutPtr = &viewPassedVertexData[j * vOutFormat.size];
      uint8_t* bVertexOutPtr = &viewPassedVertexData[(j + 1) * vOutFormat.size];
      uint8_t* cVertexOutPtr = &viewPassedVertexData[(j + 2) * vOutFormat.size];
      //
      Base::vec4_t a, b, c;
      a = processVertex(aVisible);
      b = processVertex(bVisible);
      c = processVertex(cVisible);
      //
      Base::vec4_t sa = a, sb = b, sc = c;
      if (sa.y > sc.y)
      {
        std::swap(sa, sc);
        std::swap(aVertexOutPtr, cVertexOutPtr);
      }
      if (sa.y > sb.y)
      {
        std::swap(sa, sb);
        std::swap(aVertexOutPtr, bVertexOutPtr);
      }
      if (sb.y > sc.y)
      {
        std::swap(sb, sc);
        std::swap(bVertexOutPtr, cVertexOutPtr);
      }
      // get interpolated values - line coordinates, only one component
      std::vector<float> xab = Base::interpolate(sa.y, sa.x, sb.y, sb.x);
      std::vector<float> xbc = Base::interpolate(sb.y, sb.x, sc.y, sc.x);
      std::vector<float> xac =
        Base::interpolate(sa.y, sa.x, sc.y, sc.x); // long side x
      int nzeros = (int)(xab.size() == 0) + (int)(xbc.size() == 0) +
                   (int)(xac.size() == 0);
      if (nzeros > 1)
      {
        return;
      }
      // here is the diffrence - we don't want to merge xab and xbc
      int  n = std::min(xac.size(), xab.size() + xbc.size());
      int  middle = n / 2;
      bool leftToRight = true;
      if (middle >= xab.size())
      {
        if (xac[middle] < xbc[middle - xab.size()])
        {
          leftToRight = false;
        }
      }
      else
      {
        if (xac[middle] < xab[middle])
        {
          leftToRight = false;
        }
      }
      float                bottomy = sa.y;
      int                  idx = 0;
      std::vector<uint8_t> vInterpolatedAC(vOutFormat.size),
        vInterpolatedAB(vOutFormat.size), vInterpolatedBC(vOutFormat.size),
        vInterpolatedLR(vOutFormat.size);
      std::vector<uint8_t> vDepthedA(vOutFormat.size),
        vDepthedB(vOutFormat.size), vDepthedC(vOutFormat.size),
        vDepthedP(vOutFormat.size);
      // divide attributes by original z - lesser attributes, that are located
      // further
      multiplyAttributes(aVertexOutPtr, vDepthedA.data(), sa.w, vOutFormat);
      multiplyAttributes(bVertexOutPtr, vDepthedB.data(), sb.w, vOutFormat);
      multiplyAttributes(cVertexOutPtr, vDepthedC.data(), sc.w, vOutFormat);
      //
      if (leftToRight)
      {
        Base::vec4_t c;
        for (idx = 0; idx < xab.size() % (n + 1); idx++)
        {
          float leftx = xab[idx];
          float rightx = xac[idx];
          float vWeightAB =
            getWeightBetween(leftx, bottomy, sa.x, sa.y, sb.x, sb.y);
          float vWeightAC =
            getWeightBetween(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
          interpolateAttributes(
            vDepthedA.data(),
            vDepthedB.data(),
            vInterpolatedAB.data(),
            vWeightAB,
            vOutFormat);
          interpolateAttributes(
            vDepthedA.data(),
            vDepthedC.data(),
            vInterpolatedAC.data(),
            vWeightAC,
            vOutFormat);
          float lz = sa.z + (sb.z - sa.z) * vWeightAB;
          float rz = sa.z + (sc.z - sa.z) * vWeightAC;
          float lwo = sa.w + (sb.w - sa.w) * vWeightAB;
          float rwo = sa.w + (sc.w - sa.w) * vWeightAC;
          while (leftx < rightx)
          {
            float hWeight = (leftx - xab[idx]) / (xac[idx] - xab[idx]);
            interpolateAttributes(
              vInterpolatedAB.data(),
              vInterpolatedAC.data(),
              vInterpolatedLR.data(),
              hWeight,
              vOutFormat);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * hWeight;
            float pwo = lwo + (rwo - lwo) * hWeight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiplyAttributes(
              vInterpolatedLR.data(),
              vDepthedP.data(),
              pzo,
              vOutFormat);
            renderPixelDepthWise(c, vDepthedP.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
        for (; idx < n; idx++)
        {
          float leftx = xbc[idx - xab.size()];
          float rightx = xac[idx];
          float vWeightBC =
            getWeightBetween(leftx, bottomy, sb.x, sb.y, sc.x, sc.y);
          float vWeightAC =
            getWeightBetween(rightx, bottomy, sa.x, sa.y, sc.x, sc.y);
          interpolateAttributes(
            vDepthedB.data(),
            vDepthedC.data(),
            vInterpolatedBC.data(),
            vWeightBC,
            vOutFormat);
          interpolateAttributes(
            vDepthedA.data(),
            vDepthedC.data(),
            vInterpolatedAC.data(),
            vWeightAC,
            vOutFormat);
          float lz = sb.z + (sc.z - sb.z) * vWeightBC;
          float rz = sa.z + (sc.z - sa.z) * vWeightAC;
          float lwo = sb.w + (sc.w - sb.w) * vWeightBC;
          float rwo = sa.w + (sc.w - sa.w) * vWeightAC;
          while (leftx < rightx)
          {
            float hWeight = (leftx - xbc[idx - xab.size()]) /
                            (xac[idx] - xbc[idx - xab.size()]);
            interpolateAttributes(
              vInterpolatedBC.data(),
              vInterpolatedAC.data(),
              vInterpolatedLR.data(),
              hWeight,
              vOutFormat);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * hWeight;
            float pwo = lwo + (rwo - lwo) * hWeight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiplyAttributes(
              vInterpolatedLR.data(),
              vDepthedP.data(),
              pzo,
              vOutFormat);
            renderPixelDepthWise(c, vDepthedP.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
      }
      else
      {
        Base::vec4_t c;
        for (idx = 0; idx < xab.size() % (n + 1); idx++)
        {
          float leftx = xac[idx];
          float rightx = xab[idx];
          float vWeightAC =
            getWeightBetween(leftx, bottomy, sa.x, sa.y, sc.x, sc.y);
          float vWeightAB =
            getWeightBetween(rightx, bottomy, sa.x, sa.y, sb.x, sb.y);
          interpolateAttributes(
            vDepthedA.data(),
            vDepthedB.data(),
            vInterpolatedAB.data(),
            vWeightAB,
            vOutFormat);
          interpolateAttributes(
            vDepthedA.data(),
            vDepthedC.data(),
            vInterpolatedAC.data(),
            vWeightAC,
            vOutFormat);
          float lz = sa.z + (sc.z - sa.z) * vWeightAC;
          float rz = sa.z + (sb.z - sa.z) * vWeightAB;
          float lwo = sa.w + (sc.w - sa.w) * vWeightAC;
          float rwo = sa.w + (sb.w - sa.w) * vWeightAB;
          while (leftx < rightx)
          {
            float hWeight = (leftx - xac[idx]) / (xab[idx] - xac[idx]);
            interpolateAttributes(
              vInterpolatedAC.data(),
              vInterpolatedAB.data(),
              vInterpolatedLR.data(),
              hWeight,
              vOutFormat);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * hWeight;
            float pwo = lwo + (rwo - lwo) * hWeight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiplyAttributes(
              vInterpolatedLR.data(),
              vDepthedP.data(),
              pzo,
              vOutFormat);
            renderPixelDepthWise(c, vDepthedP.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
        for (; idx < n; idx++)
        {
          float leftx = xac[idx];
          float rightx = xbc[idx - xab.size()];
          float vWeightAC =
            getWeightBetween(leftx, bottomy, sa.x, sa.y, sc.x, sc.y);
          float vWeightBC =
            getWeightBetween(rightx, bottomy, sb.x, sb.y, sc.x, sc.y);
          interpolateAttributes(
            vDepthedB.data(),
            vDepthedC.data(),
            vInterpolatedBC.data(),
            vWeightBC,
            vOutFormat);
          interpolateAttributes(
            vDepthedA.data(),
            vDepthedC.data(),
            vInterpolatedAC.data(),
            vWeightAC,
            vOutFormat);
          float lz = sa.z + (sc.z - sa.z) * vWeightAC;
          float rz = sb.z + (sc.z - sb.z) * vWeightBC;
          float lwo = sa.w + (sc.w - sa.w) * vWeightAC;
          float rwo = sb.w + (sc.w - sb.w) * vWeightBC;
          while (leftx < rightx)
          {
            float hWeight =
              (leftx - xac[idx]) / (xbc[idx - xab.size()] - xac[idx]);
            interpolateAttributes(
              vInterpolatedAC.data(),
              vInterpolatedBC.data(),
              vInterpolatedLR.data(),
              hWeight,
              vOutFormat);
            c.x = leftx;
            c.y = bottomy;
            c.z = lz + (rz - lz) * hWeight;
            float pwo = lwo + (rwo - lwo) * hWeight;
            float pzo = 1.0f / pwo;
            c.w = pwo;
            multiplyAttributes(
              vInterpolatedLR.data(),
              vDepthedP.data(),
              pzo,
              vOutFormat);
            renderPixelDepthWise(c, vDepthedP.data());
            leftx += 1.0f;
          }
          bottomy += 1.0f;
        }
      }
      fragments++;
    }
  }
  if (m_log != nullptr)
  {
    static char msg[128];
    int         iwr = snprintf(msg, 128, "triangles drawn: %d \n", fragments);
    m_log(msg, iwr);
  }
}

void
Temple::Bonfire::RenderContext::setLogFunc(logFunc log)
{
  m_log = log;
}

Temple::Bonfire::RenderContext::~RenderContext()
{
  delete[] m_data;
  m_data = nullptr;
}
