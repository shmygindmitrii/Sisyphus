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

#include "app.h"

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

static HANDLE s_hWinConsoleOutput = 0;

static void
PrintToWinConsole(const char* pMessage, unsigned int size)
{
  DWORD iCharsWritten = 0;
  WriteConsoleA(s_hWinConsoleOutput, pMessage, size, &iCharsWritten, NULL);
}

struct update_info_t 
{
  int width, height, bpp;
  float dt;
};

void
draw(HWND hWnd)
{
  RECT                                  rect;

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

  HDC hdc = GetDC(hWnd);

  static long long nanoseconds_prev = 0;
  auto      curTime = std::chrono::high_resolution_clock::now();
  auto      duration = curTime.time_since_epoch();
  long long nanoseconds =
    std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
  long long nanoseconds_passed = nanoseconds - nanoseconds_prev;
  float dt = nanoseconds_passed / 1000000000.0f;
  if (nanoseconds_prev == 0)
  {
    dt = 0.0f;
  }
  nanoseconds_prev = nanoseconds;
  update_info_t upd{ width, height, 4, dt};
  temple_application_update(&upd);
  
  static std::vector<uint8_t> frame_data = {};
  frame_data.resize(width * height * 4);
  temple_application_get_frame(frame_data.data(), static_cast<unsigned int>(frame_data.size()));

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
    (unsigned char*)frame_data.data(),
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
  
  // software renderer application is inside
  temple_application_init(nullptr);
  temple_application_set_log_function(PrintToWinConsole);

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
