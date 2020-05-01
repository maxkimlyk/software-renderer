#ifndef _LINUX_WINDOW_H_
#define _LINUX_WINDOW_H_

#include <functional>
#include <memory>
#include <string>

#include <X11/Xlib.h>

#include "../../common/input.h"
#include "../../renderer/canvas.h" // TODO: move Image to common code

namespace sr
{

class Window
{
  public:
    using GetFrameFunc = std::function<Image&(void)>;

    Window(GetFrameFunc get_frame, Input& input);

    int Create(size_t width, size_t height, const std::string& caption);
    void SetCaption(const std::string& caption);
    void MainLoopRoutine();
    void Redraw();

    bool IsClosed() const;

  private:
    void HandleExpose() const;
    void CreateImageBuffer(size_t width, size_t height);
    void PreventResizing() const;
    void SubscribeToWindowClosing();

    bool closed_;
    ::Display* display_;
    ::Visual* visual_;
    ::Window window_;
    ::GC gc_;
    ::Atom wm_delete_message_;
    std::unique_ptr<uint8_t[]> image_buffer_;
    size_t width_;
    size_t height_;
    size_t stride_;

    GetFrameFunc get_frame_;
    Input& input_;
};

} // namespace sr

#endif
