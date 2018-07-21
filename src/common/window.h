#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <windows.h>
#include <string>

#include "canvas.h"
#include "renderer.h"
#include "input.h"

static const char *WINDOW_CLASS_NAME = "rendererwindowclass";
static const DWORD BPP = 32;
static const size_t STR_SIZE =32;

class Window
{
    HINSTANCE hInstance;
    HWND hWnd;

    Renderer *renderer;
    Input *input;

public:
    bool closed;

    Window(Renderer *renderer, Input *input)
    {
        this->renderer = renderer;
        this->input = input;
    }

    int Create(size_t width, size_t height, const std::string &caption)
    {
        hInstance = GetModuleHandle(NULL);

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

        hWnd = CreateWindowEx(dwExStyle, WINDOW_CLASS_NAME,
                              caption.c_str(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                              100, 100,
                              windowRect.right - windowRect.left,
                              windowRect.bottom - windowRect.top,
                              NULL, NULL, hInstance, (LPVOID)(this));
        if (!hWnd)
        {
            printf("Could not create window\n");
            return -2;
        }

        ShowWindow(hWnd, SW_SHOW);
        SetForegroundWindow(hWnd);
        SetFocus(hWnd);

        closed = false;

        return 0;
    }

    int MainLoopRoutine()
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                closed = true;
                return -1;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return 0;
    }

    void Redraw()
    {
        RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
    }

    void SetCaption(std::string &text)
    {
        SetWindowText(hWnd, text.c_str());
    }

private:
    int RegisterWindowClass()
    {
        WNDCLASS wc;
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
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

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        static HDC hCompatibleDC = 0;
        static Window *thisptr = NULL;

        switch (uMsg)
        {
        case WM_CREATE:
        {
            HDC hdc = GetDC(hWnd);

            hCompatibleDC = CreateCompatibleDC(hdc);
            if (!hCompatibleDC)
                WARNING("hCompatibleDC is null\n");

            ReleaseDC(hWnd, hdc);

            CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
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
            HDC hdc = BeginPaint(hWnd, &ps);
            if (!hdc)
                WARNING("hDC is null\n");

            RECT rect;
            GetClientRect(hWnd, &rect);
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

            uint32_t *bitPointer;
            size_t bufferSize = bmi.bmiHeader.biSizeImage;

            HBITMAP hBitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)(&bitPointer), NULL, NULL);
            if (!hBitmap)
            {
                WARNING("Could not create DIB section\n");
                EndPaint(hWnd, &ps);
                return -1;
            }

            Canvas<uint32_t> *canvas = thisptr->renderer->canvas;
            canvas->CopyTo((uint32_t*)bitPointer, bufferSize);

            HGDIOBJ hOldObj = SelectObject(hCompatibleDC, hBitmap);

            BitBlt(hdc, 0, 0, max_width, max_height, hCompatibleDC, 0, 0, SRCCOPY);

            DeleteObject(hBitmap);
            EndPaint(hWnd, &ps);

            return 0;
        }

        case WM_CLOSE:
            ReleaseDC(hWnd, hCompatibleDC);
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            thisptr->input->OnKeyDown(wParam);
            return 0;

        case WM_KEYUP:
            thisptr->input->OnKeyUp(wParam);
            return 0;

        case WM_ACTIVATE:
            if (HIWORD(wParam) == 0)
                thisptr->input->OnFocus();
            else
                thisptr->input->OnFocusLost();
            return 0;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
};

#endif
