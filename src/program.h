#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <sstream>

#include "window.h"
#include "renderer.h"

class Program
{
    Window window;
    bool isRunning;

public:
    void (*ProcessCallback)();
    void (*DrawCallback)(Renderer &renderer);

    int Run(size_t windowWidth, size_t windowHeight, const std::string &windowCaption)
    {
        int status = window.Create(windowWidth, windowHeight, windowCaption);
        if (status != 0)
        {
            ERROR("Could not create window\n");
            return status;
        }

        return MainLoop();
    }

private:
    int MainLoop()
    {
        isRunning = true;

        DWORD secBeginTime = timeGetTime();
        uint32_t fpsCount = 0;

        MSG msg;
        while (isRunning)
        {
            window.MainLoopRoutine();
            if (window.closed)
            {
                isRunning = false;
                break;
            }

            window.Redraw();
            fpsCount += 1;

            if (timeGetTime() - secBeginTime >= 1000)
            {
                std::ostringstream oss;
                oss << "FPS: " << fpsCount;
                window.SetCaption(oss.str());
                fpsCount = 0;
                secBeginTime += 1000;
            }
        }

        return msg.wParam;
    }
};

#endif
