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

}

int main()
{
    Program program;
    program.ProcessCallback = Process;
    program.DrawCallback = Draw;
    return program.Run(WIDTH, HEIGHT, CAPTION);
}