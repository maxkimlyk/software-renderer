#include "window.h"

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "TEST";

int main()
{
    Window window;
    int status = window.Create(WIDTH, HEIGHT, CAPTION);
    if (status != 0)
        return status;

    window.MainLoop();

    return 0;
}