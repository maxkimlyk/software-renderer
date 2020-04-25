#ifndef _LINUX_WINDOW_H_
#define _LINUX_WINDOW_H_

#include <functional>
#include <string>
#include <memory>

#include <X11/Xlib.h>

#include "../../renderer/canvas.h" // TODO: move Image to common code

namespace sr
{

class Window
{
  public:
    using GetFrameFunc = std::function<Image&(void)>;

    Window(GetFrameFunc get_frame);

    int Create(size_t width, size_t height, const std::string& caption);
    void SetCaption(const std::string& caption);
    void MainLoopRoutine();
    void Redraw();

    bool closed = false; // TODO

  private:
    void HandleExpose();
    void CreateImageBuffer(size_t width, size_t height);

    ::Display* display_;
    ::Visual* visual_;
    ::Window window_;
    ::GC gc_;
    std::unique_ptr<uint8_t[]> image_buffer_;
    size_t width_;
    size_t height_;
    size_t stride_;
    GetFrameFunc get_frame_;
};

} // namespace sr

#endif
