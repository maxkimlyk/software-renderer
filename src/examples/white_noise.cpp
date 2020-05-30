#include "../common/program.h"

using namespace sr;

class Demo
{
  public:
    const size_t Width = 800;
    const size_t Height = 600;
    const std::string Caption = "White Noise";

    void Init(Renderer& renderer)
    {}

    void Process(Renderer& renderer, Input& input)
    {}

    void Draw(Renderer& renderer)
    {
        for (size_t y = 0; y < renderer.Height(); ++y)
            for (size_t x = 0; x < renderer.Width(); ++x)
            {
                renderer.SetPixel(x, y, Color(rand(), rand(), rand()));
            }
    }
};

int main()
{
    Demo demo;

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
