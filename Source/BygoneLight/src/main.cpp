#define _USE_MATH_DEFINES
#include <math.h>

#include "framework.h"
#include "bonfire_render_context.h"
#include "base_matrices.h"
#include "obj_file.h"
#include "bonfire_texture_holder.h"
#include "win_tex_loader.h"
#include <cassert>
#include "resource.h"
#include <chrono>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                         // current instance
WCHAR     szTitle[MAX_LOADSTRING];       // The title bar text
WCHAR     szWindowClass[MAX_LOADSTRING]; // the main window class name
HWND      hWnd;

// Forward declarations of functions included in this code module:
ATOM
MyRegisterClass(HINSTANCE hInstance);
BOOL
                 InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

std::vector<Temple::Base::vec4_t> g_modelVerts;
std::vector<int>                  g_modelInds;
std::vector<uint8_t>              g_modelVertAttribs;
static uint32_t                   mosaicTexture;

//
static Temple::Bonfire::col4u              red {255, 0, 0, 255};
static Temple::Bonfire::col4u              green {0, 255, 0, 255};
static Temple::Bonfire::col4u              blue {0, 0, 255, 255};
static Temple::Bonfire::col4u              yellow {200, 200, 0, 255};
static Temple::Bonfire::col4u              pink {200, 154, 165, 255};
static Temple::Bonfire::col4u              turquoise {0, 154, 154, 255};
static Temple::Bonfire::col4u              orange {205, 88, 0, 255};
static Temple::Bonfire::col4u              violet {107, 0, 215, 255};
static std::vector<Temple::Bonfire::col4u> defaultColorsU =
  {red, green, blue, yellow, pink, turquoise, orange, violet};
static std::vector<Temple::Base::vec4_t> defaultColors = {
  Temple::Bonfire::getFloatColor(red),
  Temple::Bonfire::getFloatColor(green),
  Temple::Bonfire::getFloatColor(blue),
  Temple::Bonfire::getFloatColor(yellow),
  Temple::Bonfire::getFloatColor(pink),
  Temple::Bonfire::getFloatColor(turquoise),
  Temple::Bonfire::getFloatColor(orange),
  Temple::Bonfire::getFloatColor(violet)};
static Temple::Bonfire::col4u bgColor {15, 15, 35, 255};
static Temple::Bonfire::col4u lineColor {0, 150, 0, 255};

static HANDLE s_hWinConsoleOutput = 0;

static void
PrintToWinConsole(const char* pMessage, size_t zSize)
{
  DWORD iCharsWritten = 0;
  WriteConsoleA(s_hWinConsoleOutput, pMessage, zSize, &iCharsWritten, NULL);
}

void
draw(HWND hWnd)
{
  RECT                                  rect;
  static Temple::Bonfire::RenderContext renderContext(1, 1, 4);
  static bool                           _logInited = false;
  if (!_logInited)
  {
    _logInited = true;
    renderContext.setLogFunc(PrintToWinConsole);
  }

  // Prepare BITMAPINFO
  BITMAPINFO bmi;
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32; // 32-bit
  bmi.bmiHeader.biCompression = BI_RGB;

  GetClientRect(hWnd, &rect);

  int width = rect.right - rect.left;
  int height = rect.bottom - rect.top;

  bmi.bmiHeader.biWidth = width;
  bmi.bmiHeader.biHeight = -height; // Top-down
  renderContext.resize(width, height, 4);

  HDC hdc = GetDC(hWnd);

  renderContext.setViewport(0, 0, 0, width, height, 1);
  // setScissor(0, 0, width, height);

  // begin straight filling of color buffer
  renderContext.clearDepth(0.0f);
  renderContext.fill(bgColor); // fill background and also clear screen
  Temple::Base::vec4_t              a {-0.0f, -0.0f, +0.2f, +1.0f};
  Temple::Base::vec4_t              b {-0.7f, -0.7f, +0.2f, +1.0f};
  Temple::Base::vec4_t              c {+0.7f, -0.7f, +0.2f, +1.0f};
  Temple::Base::vec2_t              uv0 {0.0f, 0.0f};
  Temple::Base::vec2_t              uv1 {1.0f, 0.0f};
  Temple::Base::vec2_t              uv2 {0.0f, 1.0f};
  Temple::Base::vec3_t              normal {0.0f, 0.0f, -1.0f};
  std::vector<Temple::Base::vec4_t> abc = {a, b, c};
  std::vector<uint8_t>              abcTriangleAttribs;
  Temple::Base::vec4_t              fred = Temple::Bonfire::getFloatColor(red);
  Temple::Base::vec4_t fgreen = Temple::Bonfire::getFloatColor(green);
  Temple::Base::vec4_t fblue = Temple::Bonfire::getFloatColor(blue);
  Temple::Base::append_data(abcTriangleAttribs, fred);
  Temple::Base::append_data(abcTriangleAttribs, uv0);
  Temple::Base::append_data(abcTriangleAttribs, normal);
  Temple::Base::append_data(abcTriangleAttribs, fgreen);
  Temple::Base::append_data(abcTriangleAttribs, uv1);
  Temple::Base::append_data(abcTriangleAttribs, normal);
  Temple::Base::append_data(abcTriangleAttribs, fblue);
  Temple::Base::append_data(abcTriangleAttribs, uv2);
  Temple::Base::append_data(abcTriangleAttribs, normal);

  std::vector<int> abcLineIndices = {0, 1, 1, 2, 2, 0};
  std::vector<int> abcTriangleIndices = {2, 1, 0};

  Temple::Bonfire::VertexFormat vertexOutputFormat({
    Temple::Bonfire::EVertexAttribType::VEC4, // position
    Temple::Bonfire::EVertexAttribType::VEC4, // color
    Temple::Bonfire::EVertexAttribType::VEC2, // tex
    Temple::Bonfire::EVertexAttribType::VEC3, // normal
  });
  Temple::Bonfire::VertexFormat vertexInputFormat({
    Temple::Bonfire::EVertexAttribType::VEC4, // color
    Temple::Bonfire::EVertexAttribType::VEC2, // tex
    Temple::Bonfire::EVertexAttribType::VEC3, // normal
  });

  auto      curTime = std::chrono::high_resolution_clock::now();
  auto      duration = curTime.time_since_epoch();
  long long nanoseconds =
    std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

  long long circle_ns = 10000000000;
  float     angle = ((nanoseconds % circle_ns) / (float)circle_ns) * M_PI *
                2.0f; // unused for now

  // float angley = 30.0f / 360.0f * 2.0f * M_PI;
  // float anglez = 10.0f / 360.0f * 2.0f * M_PI;
  const Temple::Base::mat4_t mRotY = Temple::Base::mat4_t::roty(angle);
  const Temple::Base::mat4_t mRotZ = Temple::Base::mat4_t::rotz(angle);
  Temple::Base::mat4_t       mRotation = mRotY * mRotZ;

  Temple::Base::mat4_t mScale = Temple::Base::mat4_t::identity();
  mScale.r0.x = 0.5f;
  mScale.r1.y = 0.5f;
  mScale.r2.z = 0.5f;

  Temple::Base::mat4_t mTranslation = Temple::Base::mat4_t::identity();
  mTranslation.r1.w = 0.3f; // y-shift
  mTranslation.r2.w = 0.7f; // z-shift

  renderContext.setModelMatrix(mTranslation * mRotation * mScale);
  renderContext.setViewMatrix(
    Temple::Base::mat4_t::identity()); // not really used yet
  renderContext.setPerspective(90.0f, width / (float)height, 0.4f, 100.0f);
  renderContext.setBackfaceCulling(
    Temple::Bonfire::ECullingMode::CounterClockWise);

  std::vector<uint8_t> descriptorSet;
  Temple::Base::vec3_t cameraPosition {0.0f, 0.0f, 0.0f};
  Temple::Base::append_data(descriptorSet, cameraPosition);
  int lightCount = 3;
  Temple::Base::append_data(descriptorSet, lightCount);
  int   lightAmbient = 0;
  float ambientIllumination = 0.2f;
  int   lightPoint = 1;
  float pointIllumination = 0.4f;
  int   lightDirected = 2;
  float directIllumination = 0.4f;
  Temple::Base::append_data(descriptorSet, lightAmbient);
  Temple::Base::append_data(descriptorSet, ambientIllumination);
  Temple::Base::append_data(descriptorSet, lightPoint);
  Temple::Base::append_data(descriptorSet, pointIllumination);
  Temple::Base::vec3_t lightPointPosition {-2.0f, 0.0f, -1.0f};
  Temple::Base::append_data(descriptorSet, lightPointPosition);
  Temple::Base::append_data(descriptorSet, lightDirected);
  Temple::Base::append_data(descriptorSet, directIllumination);
  Temple::Base::vec3_t lightDirection {-1.0f, 0.0f, 1.0f};
  Temple::Base::append_data(descriptorSet, lightDirection);

  renderContext.setDescriptorSet(descriptorSet);
  renderContext.setVertexShader(
    [](
      const Temple::Base::vec4_t& inp,
      Temple::Base::vec4_t&       out,
      std::vector<uint8_t>&       perVertexOut,
      const uint8_t*              perVertexData,
      const std::vector<uint8_t>& builtins,
      const std::vector<uint8_t>& descriptorSet)
    {
      const Temple::Base::mat4_t* modelViewMatrixPtr =
        reinterpret_cast<const Temple::Base::mat4_t*>(builtins.data());
      const Temple::Base::mat4_t* mTransformPtr = modelViewMatrixPtr + 4;
      out = (*modelViewMatrixPtr) * inp;
      // almost rasterized coord is found - need to divide by w and make x,y fit
      // to viewport in another function
      const Temple::Base::vec4_t* colorPtr =
        reinterpret_cast<const Temple::Base::vec4_t*>(perVertexData);
      const Temple::Base::vec2_t* texPtr =
        reinterpret_cast<const Temple::Base::vec2_t*>(colorPtr + 1);
      const Temple::Base::vec3_t* normalPtr =
        reinterpret_cast<const Temple::Base::vec3_t*>(texPtr + 1);
      int offset = 0;
      // rotate position
      Temple::Base::vec4_t pos = out;
      memcpy(perVertexOut.data() + offset, &pos, sizeof(Temple::Base::vec4_t));
      offset += sizeof(Temple::Base::vec4_t);
      //
      memcpy(
        perVertexOut.data() + offset,
        colorPtr,
        sizeof(Temple::Base::vec4_t));
      offset += sizeof(Temple::Base::vec4_t);
      memcpy(
        perVertexOut.data() + offset,
        texPtr,
        sizeof(Temple::Base::vec2_t));
      offset += sizeof(Temple::Base::vec2_t);
      // rotate normal
      Temple::Base::vec4_t normal {
        normalPtr->x,
        normalPtr->y,
        normalPtr->z,
        0.0f};
      normal = (*modelViewMatrixPtr) * normal;
      normal = normal.calculate_normalized();
      memcpy(
        perVertexOut.data() + offset,
        &normal,
        sizeof(Temple::Base::vec3_t));
    });
  renderContext.setPixelShader(
    [](
      const Temple::Base::vec4_t& inp,
      const uint8_t*              perPixelData,
      const std::vector<uint8_t>& builtins,
      const std::vector<uint8_t>& descriptorSet) -> Temple::Base::vec4_t
    {
      const Temple::Base::vec4_t* posPtr =
        reinterpret_cast<const Temple::Base::vec4_t*>(perPixelData);
      const Temple::Base::vec4_t* pixelColorPtr =
        reinterpret_cast<const Temple::Base::vec4_t*>(posPtr + 1);
      const Temple::Base::vec2_t* texPtr =
        reinterpret_cast<const Temple::Base::vec2_t*>(pixelColorPtr + 1);
      const Temple::Base::vec3_t* normalPtr =
        reinterpret_cast<const Temple::Base::vec3_t*>(texPtr + 1);
      // per-pixel
      const Temple::Base::vec4_t& texColor =
        Temple::Bonfire::TextureHolder::instance()->getPixel(
          0,
          texPtr->u,
          texPtr->v);
      // illumination
      const Temple::Base::vec3_t* cameraPositionPtr =
        reinterpret_cast<const Temple::Base::vec3_t*>(descriptorSet.data());
      const int* lightCountPtr =
        reinterpret_cast<const int*>(cameraPositionPtr + 1);
      const int*           lightTypePtr = lightCountPtr + 1;
      Temple::Base::vec3_t pixelPosition {posPtr->x, posPtr->y, posPtr->z};
      float ambientIllumination = 0.0f, diffuseIllumination = 0.0f,
            specularIllumination = 0.0f;
      for (int i = 0; i < *lightCountPtr; i++)
      {
        const float* illuminationPtr =
          reinterpret_cast<const float*>(lightTypePtr + 1);
        if ((*lightTypePtr) == 0)
        {
          // ambient
          ambientIllumination += *illuminationPtr;
          lightTypePtr = reinterpret_cast<const int*>(illuminationPtr + 1);
        }
        else
        {
          const Temple::Base::vec3_t* lightCrd =
            reinterpret_cast<const Temple::Base::vec3_t*>(illuminationPtr + 1);
          Temple::Base::vec3_t v;
          if ((*lightTypePtr) == 1)
          {
            // point light
            v = (*lightCrd) - pixelPosition;
            v = v.calculate_normalized();
          }
          else
          {
            // directed light
            v = -(*lightCrd);
            v /= v.calculate_magnitude();
          }
          // diffuse
          float vDotn = v.calculate_dot_product(*normalPtr);
          if (vDotn > 0)
          {
            diffuseIllumination += vDotn * (*illuminationPtr);
            // specular
            Temple::Base::vec3_t vp =
              v - (*normalPtr) * v.calculate_dot_product(*normalPtr);
            Temple::Base::vec3_t r = v - vp * 2.0f;
            r = r.calculate_normalized();
            Temple::Base::vec3_t d = (*cameraPositionPtr) - pixelPosition;
            d = d.calculate_normalized();
            float dDotR = d.calculate_dot_product(r);
            if (dDotR > 0)
            {
              specularIllumination += pow(dDotR, 16.0f) * (*illuminationPtr);
            }
          }
          lightTypePtr = reinterpret_cast<const int*>(lightCrd + 1);
        }
      }
      float illumination =
        ambientIllumination + diffuseIllumination + specularIllumination;
      Temple::Base::vec4_t illuminationVec {
        illumination,
        illumination,
        illumination,
        1.0f};
      Temple::Base::vec4_t finalColor =
        (*pixelColorPtr) * texColor * illuminationVec;
      return finalColor.clamp(0.0f, 1.0f);
    });

  renderContext.drawTriangles(
    g_modelVerts,
    g_modelInds,
    reinterpret_cast<const uint8_t*>(g_modelVertAttribs.data()),
    vertexInputFormat,
    vertexOutputFormat);
  // renderContext.drawLines(abc, abcLineIndices, abcTriangleAttribs.data(),
  // vertexInputFormat, vertexOutputFormat); renderContext.drawTriangles(abc,
  // abcTriangleIndices, abcTriangleAttribs.data(), vertexInputFormat,
  // vertexOutputFormat);
  //  end of color buffer filling
  //  Draw the buffer to the window
  SetDIBitsToDevice(
    hdc,
    0,
    0,
    width,
    height,
    0,
    0,
    0,
    height,
    (unsigned char*)renderContext.getFrame(),
    &bmi,
    DIB_RGB_COLORS);
  HRESULT hr = DwmFlush();
  if (FAILED(hr))
  {
    // Handle the error. For example, DWM might not be enabled or available.
  }
  ReleaseDC(hWnd, hdc);
}

int APIENTRY
wWinMain(
  _In_ HINSTANCE     hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPWSTR        lpCmdLine,
  _In_ int           nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // Initialize global strings
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_BYGONE_LIGHT, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Perform application initialization:
  if (!InitInstance(hInstance, nCmdShow))
  {
    return FALSE;
  }

  char texPath[128], objPath[128];
  snprintf(texPath, 128, "%s/side_colored.png", TEMPLE_RESOURCES_FOLDER);
  snprintf(objPath, 128, "%s/cube_sided.obj", TEMPLE_RESOURCES_FOLDER);
  mosaicTexture = loadTexture(texPath);
  std::shared_ptr<Temple::Barn::ObjFile> objFile =
    Temple::Barn::ReadObj(objPath);

  std::vector<Temple::Base::vec4_t> colors;
  for (int vertIdx = 0; vertIdx < objFile->coord.size(); vertIdx++)
  {
    colors.push_back(defaultColors[vertIdx % defaultColors.size()]);
  }
  int gidx = 0;
  for (int i = 0; i < objFile->faces.size(); i++)
  {
    const Temple::Barn::ObjFace& face = objFile->faces[i];
    for (int j = 0; j < 3; j++)
    {
      int                  vertIdx = face.indices[j].position - 1;
      int                  uvIdx = face.indices[j].texture - 1;
      int                  normalIdx = face.indices[j].normal - 1;
      Temple::Base::vec4_t pos {
        objFile->coord[vertIdx].x,
        objFile->coord[vertIdx].y,
        objFile->coord[vertIdx].z,
        1.0f};
      g_modelVerts.push_back(pos);
      g_modelInds.push_back(gidx++);
      Temple::Base::append_data(g_modelVertAttribs, colors[vertIdx]);
      Temple::Base::append_data(g_modelVertAttribs, objFile->uv[uvIdx]);
      Temple::Base::append_data(g_modelVertAttribs, objFile->normal[normalIdx]);
    }
  }
  HACCEL hAccelTable =
    LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BYGONE_LIGHT));

  MSG msg;

  // Main message loop:
  BOOL bRet;
  while (TRUE)
  {
    // PeekMessage, unlike GetMessage, doesn't block if there's no message
    bRet = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

    if (bRet)
    {
      // Process the message
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      // Check for WM_QUIT message
      if (msg.message == WM_QUIT)
      {
        break;
      }
    }

    draw(hWnd);
  }

  return (int)msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM
MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BYGONE_LIGHT));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BYGONE_LIGHT);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

BOOL
InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  hInst = hInstance; // Store instance handle in our global variable

  hWnd = CreateWindowW(
    szWindowClass,
    szTitle,
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    0,
    852,
    480,
    nullptr,
    nullptr,
    hInstance,
    nullptr);

  if (!hWnd)
  {
    return FALSE;
  }

  BOOL bRes = AllocConsole();
  assert(bRes);
  s_hWinConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_COMMAND:
  {
    int wmId = LOWORD(wParam);
    // Parse the menu selections:
    switch (wmId)
    {
    case IDM_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
  }
  break;
  case WM_PAINT:
  {
    // nothing to do
    // draw(hWnd);
  }
  break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// Message handler for about box.
INT_PTR CALLBACK
About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}
