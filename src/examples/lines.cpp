#include "../common/program.h"
#include <cmath>

using namespace sr;

class Demo
{
    static const int SIZE = 200;

  public:
    static const size_t Width = 800;
    static const size_t Height = 600;
    inline static const std::string Caption = "Lines";

    void Init(Renderer& renderer)
    {}

    void Process(Renderer& renderer, Input& input)
    {
        const float PI = 3.141592f;
        angle_ += PI / 100;

        if (angle_ > 2 * PI)
            angle_ -= 2 * PI;

        color_ = Color(uint8_t(255 * (0.5 + 0.5 * cos(2 * angle_ + 0.1 * PI))),
                       uint8_t(255 * (0.5 + 0.5 * cos(4 * angle_ + 0.3 * PI))),
                       uint8_t(255 * (0.5 + 0.5 * cos(6 * angle_ + 0.7 * PI))));
    }

    void Draw(Renderer& renderer)
    {
        renderer.Clear();
        Vec2i center = {Width / 2, Height / 2};

        Vec2i points[] = {{center[0] - SIZE, center[1] - SIZE},
                          {center[0] - SIZE, center[1] + SIZE},
                          {center[0] + SIZE, center[1] + SIZE},
                          {center[0] + SIZE, center[1] - SIZE}};

        renderer.Line(points[0], points[1], color_);
        renderer.Line(points[1], points[2], color_);
        renderer.Line(points[2], points[3], color_);
        renderer.Line(points[3], points[0], color_);
        renderer.Line(points[0], points[2], color_);
        renderer.Line(points[1], points[3], color_);
    }

  private:
    float angle_ = 0.0f;
    Color color_ = Color(0);
};

int main()
{
    Demo demo;

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
