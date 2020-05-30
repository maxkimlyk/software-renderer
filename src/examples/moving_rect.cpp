#include "../common/program.h"
#include <cmath>

using namespace sr;

Color color(0);

class Demo
{
    static const size_t RECT_WIDTH = 50;
    static const size_t RECT_HEIGHT = 50;

  public:
    static const size_t Width = 800;
    static const size_t Height = 600;
    inline static const std::string Caption = "Moving Rect";

    void Init(Renderer& renderer)
    {}

    void Process(Renderer& renderer, Input& input)
    {
        const float PI = 3.141592f;
        angle += PI / 100;

        if (angle > 2 * PI)
            angle -= 2 * PI;

        x = int(Width / 2 + (Width / 4) * cos(angle)) - RECT_WIDTH / 2;
        y = int(Height / 2 + (Height / 4) * sin(angle)) - RECT_HEIGHT / 2;

        color = Color(uint8_t(255 * (0.5 + 0.5 * cos(2 * angle + 0.1 * PI))),
                      uint8_t(255 * (0.5 + 0.5 * cos(4 * angle + 0.3 * PI))),
                      uint8_t(255 * (0.5 + 0.5 * cos(6 * angle + 0.7 * PI))));
    }

    void Draw(Renderer& renderer)
    {
        renderer.Clear();
        renderer.DrawSolidRect(x, y, x + RECT_WIDTH, y + RECT_HEIGHT, color);
    }

  private:
    float angle = 0;
    int x = Width / 2;
    int y = Height / 2;
};

int main()
{
    Demo demo;

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
