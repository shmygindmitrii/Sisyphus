#include "framework.h"
#include "bonfire_raw_canvas.h"
#include "base_matrices.h"
#include <cassert>
#include "resource.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWnd;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void draw(HWND hWnd) {
    RECT rect;
    static Temple::Bonfire::RawCanvas canvas(1, 1, 4);

    // Prepare BITMAPINFO
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;     // 32-bit
    bmi.bmiHeader.biCompression = BI_RGB;

    // background color
    Temple::Bonfire::col4u bgColor{ 15, 15, 35, 255 };
    Temple::Bonfire::col4u lineColor{ 0, 150, 0, 255 };

    GetClientRect(hWnd, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // Top-down
    canvas.resize(width, height, 4);

    PAINTSTRUCT ps;
    HDC hdc = GetDC(hWnd);

    canvas.setViewport(0, 0, 0, width, height, 1);
    // begin straight filling of color buffer
    canvas.fill(bgColor); // fill background and also clear screen
    //canvas.drawLine(a, b, lineColor);
    Temple::Base::vec4 a{ -0.0f, -0.0f, +0.0f, +1.0f };
    Temple::Base::vec4 b{ -0.7f, +0.0f, +0.0f, +1.0f };
    Temple::Base::vec4 c{ -0.7f, +0.7f, +0.0f, +1.0f };

    auto curTime = std::chrono::high_resolution_clock::now();
    auto duration = curTime.time_since_epoch();
    long long seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    //float angle = seconds * 0.1f;

    static float angle = 0.0f;
    angle += 0.01f;

    const Temple::Base::mat4 mRotZ = Temple::Base::mat4::rotz(angle);
    Temple::Base::mat4 mAspectTransform = mRotZ;
    mAspectTransform.c0.x *= height / (float)width;
    mAspectTransform.c1.x *= height / (float)width;
    mAspectTransform.c2.x *= height / (float)width;
        
    canvas.setDescriptorSet(&mAspectTransform);
    canvas.setVertexShader([](const Temple::Base::vec4& inp, Temple::Base::vec4* out, const void* descriptorSet) { 
        const Temple::Base::mat4* mRot = reinterpret_cast<const Temple::Base::mat4*>(descriptorSet);
        *out = (*mRot) * inp;
    });
    
    canvas.drawTriangle(a, b, c, lineColor);

    //canvas.drawLine(b, c, lineColor);
    // end of color buffer filling
    // 
    // Draw the buffer to the window
    SetDIBitsToDevice(hdc, 0, 0, width, height, 0, 0, 0, height, (unsigned char*)canvas.getData(), &bmi, DIB_RGB_COLORS);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BYGONE_LIGHT));

    MSG msg;

    // Main message loop:
    BOOL bRet;
    while (TRUE) {
        // PeekMessage, unlike GetMessage, doesn't block if there's no message
        bRet = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

        if (bRet) {
            // Process the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // Check for WM_QUIT message
            if (msg.message == WM_QUIT)
                break;
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
ATOM MyRegisterClass(HINSTANCE hInstance)
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        draw(hWnd);
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
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
