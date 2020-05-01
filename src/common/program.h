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

    Program(InitF init_callback, ProcessF process_callback, DrawF draw_callback)
        : init_callback_(init_callback), process_callback_(process_callback),
          draw_callback_(draw_callback)
    {}

    ~Program()
    {}

    int Run(size_t window_width, size_t window_height, const std::string& window_caption)
    {
        frame_ = std::make_unique<Image>(window_width, window_height);
        renderer_ = std::make_unique<Renderer>(*frame_);
        input_ = std::make_unique<Input>();

        const auto get_frame = [this]() -> Image& { return *frame_; };
        window_ = std::make_unique<Window>(get_frame, *input_);

        int status = window_->Create(window_width, window_height, window_caption);
        if (status != 0)
        {
            ERROR("Could not create window_\n");
            return status;
        }

        window_caption_ = window_caption;

        init_callback_(*renderer_);

        return MainLoop();
    }

  private:
    int MainLoop()
    {
        is_running_ = true;

        uint32_t sec_begin_time = GetTimeMs();
        uint32_t fps_counter = 0;

        uint32_t last_process_time = sec_begin_time;

        while (is_running_)
        {
            window_->MainLoopRoutine();
            if (window_->IsClosed())
            {
                is_running_ = false;
                break;
            }

            window_->Redraw();
            fps_counter += 1;

            bool updated = false;
            uint32_t now_time = GetTimeMs();
            while (now_time - last_process_time >= process_interval_)
            {
                process_callback_(*renderer_, *input_);
                updated = true;
                last_process_time += process_interval_;
            }

            if (updated)
                draw_callback_(*renderer_);

            if (GetTimeMs() - sec_begin_time >= 1000)
            {
                std::ostringstream oss;
                oss << window_caption_ << " (FPS: " << fps_counter << ")";
                window_->SetCaption(oss.str());
                fps_counter = 0;
                sec_begin_time += 1000;
            }
        }

        return 0;
    }

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
