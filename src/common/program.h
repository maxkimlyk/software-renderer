#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <sstream>

#include "../platform/platform_dependent.h"
#include "input.h"
#include "../renderer/renderer.h"
#include "window.h"

namespace sr
{

class Program
{
    Window* window;
    Canvas<uint32_t>* canvas;

    std::string windowCaption;

    bool isRunning;

  public:
    Renderer* renderer;
    Input* input;

    void (*InitCallback)(Renderer& renderer);
    void (*ProcessCallback)(Renderer& renderer, Input& input);
    void (*DrawCallback)(Renderer& renderer);

    uint32_t processInterval = 20;

    Program()
    {
        InitCallback = NULL;
        ProcessCallback = NULL;
        DrawCallback = NULL;
    }

    ~Program()
    {
        DELETE(canvas);
        DELETE(renderer);
        DELETE(input);
        DELETE(window);
    }

    int Run(size_t windowWidth, size_t windowHeight, const std::string& windowCaption)
    {
        canvas = new Canvas<uint32_t>(windowWidth, windowHeight);
        renderer = new Renderer(canvas);
        input = new Input();

        const auto get_frame = [this]() -> Canvas<uint32_t>& { return *renderer->canvas; };
        window = new Window(get_frame, *input);

        int status = window->Create(windowWidth, windowHeight, windowCaption);
        if (status != 0)
        {
            ERROR("Could not create window\n");
            return status;
        }

        this->windowCaption = windowCaption;

        if (InitCallback)
            InitCallback(*renderer);

        return MainLoop();
    }

  private:
    int MainLoop()
    {
        isRunning = true;

        uint32_t secBeginTime = GetTimeMs();
        uint32_t fpsCount = 0;

        uint32_t lastProcessTime = secBeginTime;

        while (isRunning)
        {
            window->MainLoopRoutine();
            if (window->IsClosed())
            {
                isRunning = false;
                break;
            }

            window->Redraw();
            fpsCount += 1;

            bool updated = false;
            uint32_t timeNow = GetTimeMs();
            while (timeNow - lastProcessTime >= processInterval)
            {
                ProcessCallback(*renderer, *input);
                updated = true;
                lastProcessTime += processInterval;
            }

            if (updated)
                DrawCallback(*renderer);

            if (GetTimeMs() - secBeginTime >= 1000)
            {
                std::ostringstream oss;
                oss << windowCaption << " (FPS: " << fpsCount << ")";
                window->SetCaption(oss.str());
                fpsCount = 0;
                secBeginTime += 1000;
            }
        }

        return 0;
    }
};

} // namespace sr

#endif
