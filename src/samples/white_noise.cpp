#include "../common/program.h"

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "White Noise";

void Process()
{
    Vec3f v;
    v.Fill(0);
    float d = v * v;
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
