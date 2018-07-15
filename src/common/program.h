#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <sstream>

#include "window.h"
#include "renderer.h"

class Program
{
    Window *window;
    Canvas<uint32_t> *canvas;
    Renderer *renderer;

    std::string windowCaption;

    bool isRunning;

public:
    void (*ProcessCallback)();
    void (*DrawCallback)(Renderer &renderer);

    DWORD processTimeout = 20;

    ~Program()
    {
        DELETE(canvas);
        DELETE(renderer);
        DELETE(window);
    }

    int Run(size_t windowWidth, size_t windowHeight, const std::string &windowCaption)
    {
        canvas = new Canvas<uint32_t>(windowWidth, windowHeight);
        renderer = new Renderer(canvas);
        window = new Window(renderer);

        int status = window->Create(windowWidth, windowHeight, windowCaption);
        if (status != 0)
        {
            ERROR("Could not create window\n");
            return status;
        }

        this->windowCaption = windowCaption;

        return MainLoop();
    }

private:
    int MainLoop()
    {
        isRunning = true;

        DWORD secBeginTime = timeGetTime();
        uint32_t fpsCount = 0;

        DWORD lastProcessTime = secBeginTime;

        while (isRunning)
        {
            window->MainLoopRoutine();
            if (window->closed)
            {
                isRunning = false;
                break;
            }

            window->Redraw();
            fpsCount += 1;

            bool updated = false;
            DWORD timeNow = timeGetTime();
            while (timeNow - lastProcessTime >= processTimeout)
            {
                ProcessCallback();
                updated = true;
                lastProcessTime += processTimeout;
            }

            if (updated)
                DrawCallback(*renderer);

            if (timeGetTime() - secBeginTime >= 1000)
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

#endif
