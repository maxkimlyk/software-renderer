#include "window.h"

#include <memory>

#include <X11/Xlib.h>

#include "../../renderer/renderer.h"

namespace
{
const size_t BYTES_PER_PIXEL = 3;
const size_t PITCH = 8;
} // namespace

namespace sr
{

Window::Window(Renderer* renderer)
{
    this->renderer = renderer;
}

int Window::Create(size_t width, size_t height, const std::string& caption)
{
    static const int border_width = 5;

    this->width = width;
    this->height = height;

    display = XOpenDisplay(NULL);
    visual = DefaultVisual(display, 0);
    const int depth = DefaultDepth(display, 0);

    XSetWindowAttributes attributes;
    attributes.background_pixel = XBlackPixel(display, 0);

    window = XCreateWindow(display, XRootWindow(display, 0), 0, 0, width, height, border_width,
                           depth, InputOutput, visual, CWBackPixel, &attributes);

    SetCaption(caption);

    XSelectInput(display, window, ExposureMask | StructureNotifyMask);

    const int screen_num = DefaultScreen(display);
    gc = DefaultGC(display, screen_num);

    XMapWindow(display, window);

    return 0;
}

void Window::SetCaption(const std::string& string)
{
    XStoreName(display, window, string.c_str());
}

void Window::HandleExpose()
{
    // TODO: make this function better
    static std::unique_ptr<uint8_t[]> image_buffer = nullptr;

    Canvas<uint32_t>* canvas = renderer->canvas;

    if (!image_buffer)
    {
        const size_t size = canvas->GetPitchedSize(PITCH);
        image_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[size]);
    }

    const int screen_num = DefaultScreen(display);

    canvas->CopyPitched((uint32_t*)(image_buffer.get()), PITCH);
    XImage* img = XCreateImage(display, visual, 8 * BYTES_PER_PIXEL, ZPixmap, 0,
                               (char*)(image_buffer.get()), width, height, PITCH, 0);
    XPutImage(display, window, DefaultGC(display, screen_num), img, 0, 0, 0, 0, width, height);
    XFree(img);
}

void Window::MainLoopRoutine()
{
    while (true)
    {
        XEvent event;
        bool is_new_event =
            XCheckWindowEvent(display, window, ExposureMask | StructureNotifyMask, &event);
        if (!is_new_event)
            return;

        switch (event.type)
        {
        case Expose:
        {
            HandleExpose();
            break;
        }
        }
    }
}

void Window::Redraw()
{
    XEvent event;
    event.type = Expose;
    XSendEvent(display, window, False, ExposureMask, &event);
}

} // namespace sr
