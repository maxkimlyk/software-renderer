#include "program.h"
#include "renderer.h"

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "TEST";

void Process()
{

}

void Draw(Renderer &renderer)
{
    for (size_t y = 0; y < renderer.Height(); ++y)
        for (size_t x = 0; x < renderer.Width(); ++x)
        {
            renderer.SetPixel(x, y, Color(rand(), rand(), rand()));
        }
}

int main()
{
    Program program;
    program.ProcessCallback = Process;
    program.DrawCallback = Draw;
    return program.Run(WIDTH, HEIGHT, CAPTION);
}