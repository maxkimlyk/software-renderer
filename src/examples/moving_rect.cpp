#include "../common/program.h"
#include <cmath>

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Moving Rect";

const size_t rectWidth = 50;
const size_t rectHeight = 50;

float angle = 0;
int x = WIDTH / 2;
int y = HEIGHT / 2;

Color color(0);

void Process(sr::Renderer& renderer, Input& input)
{
    const float PI = 3.141592f;
    angle += PI / 100;

    if (angle > 2 * PI)
        angle -= 2 * PI;

    x = int(WIDTH / 2 + (WIDTH / 4) * cos(angle)) - rectWidth / 2;
    y = int(HEIGHT / 2 + (HEIGHT / 4) * sin(angle)) - rectHeight / 2;

    color = Color(uint8_t(255 * (0.5 + 0.5 * cos(2 * angle + 0.1 * PI))),
                  uint8_t(255 * (0.5 + 0.5 * cos(4 * angle + 0.3 * PI))),
                  uint8_t(255 * (0.5 + 0.5 * cos(6 * angle + 0.7 * PI))));
}

void Draw(sr::Renderer& renderer)
{
    renderer.Clear();
    renderer.DrawSolidRect(x, y, x + rectWidth, y + rectHeight, color);
}

int main()
{
    sr::Program program;
    program.ProcessCallback = Process;
    program.DrawCallback = Draw;
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
