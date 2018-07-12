#include "../common/program.h"
#include <cmath>

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Lines";

float angle = 0;
const int size = 100;

Color color(0);

void Process()
{
    const float PI = 3.141592f;
    angle += PI / 100;

    if (angle > 2 * PI)
        angle -= 2 * PI;

    color = Color(
        uint8_t( 255 * (0.5 + 0.5 * cos(2*angle + 0.1 * PI)) ),
        uint8_t( 255 * (0.5 + 0.5 * cos(4*angle + 0.3 * PI)) ),
        uint8_t( 255 * (0.5 + 0.5 * cos(6*angle + 0.7 * PI)) )
    );
}

void Draw(Renderer &renderer)
{
    renderer.Clear();
    Vec2i center = {WIDTH / 2, HEIGHT / 2};

    Vec2i points[] = {
        {center[0] - size, center[1] - size},
        {center[0] - size, center[1] + size},
        {center[0] + size, center[1] + size},
        {center[0] + size, center[1] - size}
    };

    renderer.Line(points[0], points[1], color);
    renderer.Line(points[1], points[2], color);
    renderer.Line(points[2], points[3], color);
    renderer.Line(points[3], points[0], color);
    renderer.Line(points[0], points[2], color);
    renderer.Line(points[1], points[3], color);
}

int main()
{
    Program program;
    program.ProcessCallback = Process;
    program.DrawCallback = Draw;
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
