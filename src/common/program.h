#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <memory>
#include <sstream>

#include "../platform/platform.h"
#include "../renderer/renderer.h"
#include "input.h"
#include "window.h"

namespace sr
{

class Program
{
  public:
    using InitF = std::function<void(Renderer&)>;
    using ProcessF = std::function<void(Renderer&, Input&)>;
    using DrawF = std::function<void(Renderer&)>;

    Program(InitF init_callback, ProcessF process_callback, DrawF draw_callback);

    int Run(size_t window_width, size_t window_height, const std::string& window_caption);

  private:
    int MainLoop();

    InitF init_callback_;
    ProcessF process_callback_;
    DrawF draw_callback_;

    std::unique_ptr<Window> window_;
    std::unique_ptr<Image> frame_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<Input> input_;

    std::string window_caption_;

    uint32_t process_interval_ = 20;

    bool is_running_;
};

} // namespace sr

#endif
