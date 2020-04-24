#ifndef _LINUX_WINDOW_H_
#define _LINUX_WINDOW_H_

#include <X11/Xlib.h>
#include <string>

namespace sr
{

class Window
{
    Display* display;
    Visual* visual;
    ::Window window;
    GC gc;
    size_t width;
    size_t height;

    class Renderer* renderer;

    void HandleExpose();

  public:
    Window(class Renderer* renderer);

    int Create(size_t width, size_t height, const std::string& caption);
    void SetCaption(const std::string& caption);
    void MainLoopRoutine();
    void Redraw();

    bool closed = false; // TODO
};

} // namespace sr

#endif
