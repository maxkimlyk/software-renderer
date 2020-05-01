#include "../common/program.h"

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "White Noise";

void Init(sr::Renderer& renderer)
{}

void Process(sr::Renderer& renderer, Input& input)
{}

void Draw(sr::Renderer& renderer)
{
    for (size_t y = 0; y < renderer.Height(); ++y)
        for (size_t x = 0; x < renderer.Width(); ++x)
        {
            renderer.SetPixel(x, y, Color(rand(), rand(), rand()));
        }
}

int main()
{
    sr::Program program(Init, Process, Draw);
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
