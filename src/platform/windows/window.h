#ifndef _WINDOWS_WINDOW_H_
#define _WINDOWS_WINDOW_H_

#include <functional>
#include <string>

#include <windows.h>

#include "../../common/canvas.h"
#include "../../common/input.h"
#include "../../renderer/renderer.h"

namespace sr
{

static const char* WINDOW_CLASS_NAME = "rendererwindowclass";
static const DWORD BPP = 32;
static const size_t STR_SIZE = 32;

class Window
{
    using GetFrameFunc = std::function<Image&(void)>;

    HINSTANCE hinstance_;
    HWND hwnd_;

    Input& input_;

    GetFrameFunc get_frame_;
    bool closed_;

  public:
    Window(GetFrameFunc get_frame, Input& input)
        : get_frame_(get_frame), input_(input), closed_(true)
    {}

    bool IsClosed() const
    {
        return closed_;
    }

    int Create(size_t width, size_t height, const std::string& caption)
    {
        hinstance_ = GetModuleHandle(NULL);

        int status = RegisterWindowClass();
        if (status != 0)
            return status;

        DWORD dwExStyle;
        DWORD dwStyle;

        RECT windowRect;
        windowRect.left = 0;
        windowRect.right = width;
        windowRect.top = 0;
        windowRect.bottom = height;

        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

        AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);

        hwnd_ =
            CreateWindowEx(dwExStyle, WINDOW_CLASS_NAME, caption.c_str(),
                           WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, 100, 100,
                           windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                           NULL, NULL, hinstance_, (LPVOID)(this));
        if (!hwnd_)
        {
            printf("Could not create window\n");
            return -2;
        }

        ShowWindow(hwnd_, SW_SHOW);
        SetForegroundWindow(hwnd_);
        SetFocus(hwnd_);

        closed_ = false;

        return 0;
    }

    int MainLoopRoutine()
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                closed_ = true;
                return -1;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return 0;
    }

    void Redraw()
    {
        RedrawWindow(hwnd_, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
    }

    void SetCaption(std::string& text)
    {
        SetWindowText(hwnd_, text.c_str());
    }

  private:
    int RegisterWindowClass()
    {
        WNDCLASS wc;
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hinstance_;
        wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = WINDOW_CLASS_NAME;

        if (!RegisterClass(&wc))
        {
            printf("Could not register window class\n");
            return -1;
        }

        return 0;
    }

    static LRESULT CALLBACK WndProc(HWND hwnd_, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        static HDC hCompatibleDC = 0;
        static Window* thisptr = NULL;

        switch (uMsg)
        {
        case WM_CREATE:
        {
            HDC hdc = GetDC(hwnd_);

            hCompatibleDC = CreateCompatibleDC(hdc);
            if (!hCompatibleDC)
                WARNING("hCompatibleDC is null\n");

            ReleaseDC(hwnd_, hdc);

            CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
            thisptr = (Window*)(cs->lpCreateParams);

            break;
        }

        case WM_SYSCOMMAND:
            if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
                return 0;
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd_, &ps);
            if (!hdc)
                WARNING("hDC is null\n");

            RECT rect;
            GetClientRect(hwnd_, &rect);
            const int max_width = rect.right - rect.left;
            const int max_height = rect.bottom - rect.top;

            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
            bmi.bmiHeader.biWidth = max_width;
            bmi.bmiHeader.biHeight = -max_height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = BPP;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biSizeImage = max_width * max_height * sizeof(uint32_t);
            bmi.bmiHeader.biClrUsed = 0;
            bmi.bmiHeader.biClrImportant = 0;

            uint32_t* bitPointer;
            size_t bufferSize = bmi.bmiHeader.biSizeImage;

            HBITMAP hBitmap =
                CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)(&bitPointer), NULL, NULL);
            if (!hBitmap)
            {
                WARNING("Could not create DIB section\n");
                EndPaint(hwnd_, &ps);
                return -1;
            }

            Image& frame = thisptr->get_frame_();
            frame.CopyTo((uint32_t*)bitPointer, bufferSize);

            HGDIOBJ hOldObj = SelectObject(hCompatibleDC, hBitmap);

            BitBlt(hdc, 0, 0, max_width, max_height, hCompatibleDC, 0, 0, SRCCOPY);

            DeleteObject(hBitmap);
            EndPaint(hwnd_, &ps);

            return 0;
        }

        case WM_CLOSE:
            ReleaseDC(hwnd_, hCompatibleDC);
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            thisptr->input_.OnKeyDown(wParam);
            return 0;

        case WM_KEYUP:
            thisptr->input_.OnKeyUp(wParam);
            return 0;

        case WM_ACTIVATE:
            if (HIWORD(wParam) == 0)
                thisptr->input_.OnFocus();
            else
                thisptr->input_.OnFocusLost();
            return 0;
        }

        return DefWindowProc(hwnd_, uMsg, wParam, lParam);
    }
};

} // namespace sr

#endif
