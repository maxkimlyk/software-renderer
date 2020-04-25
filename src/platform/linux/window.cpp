#include "window.h"

#include <memory>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace
{
const size_t BYTES_PER_PIXEL = 3;
const size_t PITCH = 8;
const size_t WINDOW_BORDER_WIDTH = 5;
} // namespace

namespace sr
{

Window::Window(GetFrameFunc get_frame) : get_frame_(get_frame)
{}

int Window::Create(size_t width, size_t height, const std::string& caption)
{
    display_ = XOpenDisplay(NULL);
    visual_ = DefaultVisual(display_, 0);
    const int depth = DefaultDepth(display_, 0);

    XSetWindowAttributes attributes;
    attributes.background_pixel = XBlackPixel(display_, 0);

    window_ =
        XCreateWindow(display_, XRootWindow(display_, 0), 0, 0, width, height, WINDOW_BORDER_WIDTH,
                      depth, InputOutput, visual_, CWBackPixel, &attributes);

    SetCaption(caption);

    XSelectInput(display_, window_, ExposureMask | StructureNotifyMask);

    const int screen_num = DefaultScreen(display_);
    gc_ = DefaultGC(display_, screen_num);

    CreateImageBuffer(width, height);

    XMapWindow(display_, window_);

    PreventResizing();

    return 0;
}

void Window::CreateImageBuffer(size_t width, size_t height)
{
    width_ = width;
    height_ = height;
    stride_ = (width + PITCH - 1) / PITCH * PITCH;
    const size_t size = stride_ * height * sizeof(uint32_t);
    image_buffer_ = std::unique_ptr<uint8_t[]>(new uint8_t[size]);
}

void Window::PreventResizing() const
{
    XSizeHints* sh = XAllocSizeHints();
    sh->flags =  PMinSize | PMaxSize;
    sh->min_width = width_;
    sh->min_height = height_;
    sh->max_width = width_;
    sh->max_height = height_;
    XSetWMNormalHints(display_, window_, sh);
}

void Window::SetCaption(const std::string& string)
{
    XStoreName(display_, window_, string.c_str());
}

void Window::HandleExpose() const
{
    const int screen_num = DefaultScreen(display_);

    Canvas<uint32_t>& frame = get_frame_();

    frame.CopyWithStride((uint32_t*)(image_buffer_.get()), stride_);
    XImage* img = XCreateImage(display_, visual_, 8 * BYTES_PER_PIXEL, ZPixmap, 0,
                               (char*)(image_buffer_.get()), width_, height_, PITCH, 0);
    XPutImage(display_, window_, DefaultGC(display_, screen_num), img, 0, 0, 0, 0, width_, height_);
    XFree(img);
}

void Window::MainLoopRoutine()
{
    while (true)
    {
        XEvent event;
        bool is_new_event =
            XCheckWindowEvent(display_, window_, ExposureMask | StructureNotifyMask, &event);
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
    XSendEvent(display_, window_, False, ExposureMask, &event);
}

} // namespace sr
