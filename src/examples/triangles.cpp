#include "../renderer/program.h"
#include <cmath>
#include <vector>

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Triangles";

const size_t POINTS_AMOUNT = 100;
const float VARIANCE = 4.0f;

float z = -5.0f;
std::vector<Vec3f> points;

void Init(sr::Renderer& renderer)
{
    const auto randf = []() { return (float)(rand()) / (float)(RAND_MAX)-0.5f; };

    points.reserve(POINTS_AMOUNT);
    for (size_t i = 0; i < POINTS_AMOUNT; ++i)
    {
        float x = randf() * VARIANCE;
        float y = randf() * VARIANCE;
        points.emplace_back(Vec3f{x, y, 0.0f});
    }
}

void Process(sr::Renderer& renderer, Input& input)
{
    if (input.IsDown(0x26))
        z += 0.1f;
    if (input.IsDown(0x28))
        z -= 0.1f;
}

void Draw(sr::Renderer& renderer)
{
    static const Color red(230, 0, 0);
    static const Color green(0, 230, 0);
    static const Color yellow(230, 230, 0);
    static const Color margenta(230, 0, 230);
    static const Color cyan(0, 230, 230);
    static const Color white(230, 230, 230);

    renderer.Clear();

    for (Vec3f v : points)
    {
        v.z = z;
        renderer.Triangle(v, v, v, white);
    }

    renderer.Triangle(Vec3f{0, 0, z}, Vec3f{0, 0.5, z}, Vec3f{0.5, 0, z}, yellow);
    renderer.Triangle(Vec3f{1, 0, z}, Vec3f{1.4f, 1, z}, Vec3f{1.6f, -0.6f, z}, green);
    renderer.Triangle(Vec3f{-1, 0, z}, Vec3f{-1.5f, 1, z}, Vec3f{-0.8f, 1, z}, red);
    renderer.Triangle(Vec3f{0, 1.3f, z}, Vec3f{-1.5f, 1.3f, z}, Vec3f{-0.8f, 1.8f, z}, margenta);

    renderer.Triangle(Vec3f{-2, 1.5, z}, Vec3f{-2, 2, z}, Vec3f{-2, -1, z}, cyan);
    renderer.Triangle(Vec3f{2, 2, z}, Vec3f{2, 2, z}, Vec3f{2, -1, z}, cyan);
    renderer.Triangle(Vec3f{2, 2, z}, Vec3f{2, 2, z}, Vec3f{-2, 2, z}, cyan);
    renderer.Triangle(Vec3f{2, -1, z}, Vec3f{1.1f, -1, z}, Vec3f{-2, -1, z}, cyan);

    renderer.Triangle(Vec3f{-2.0, 2.0, z}, Vec3f{-2.0, 2.0, z}, Vec3f{2.0, -1.0, z}, cyan);
    renderer.Triangle(Vec3f{2.0, 2.0, z}, Vec3f{2.0, 2.0, z}, Vec3f{-2.0, -1.0, z}, cyan);
}

int main()
{
    sr::Program program;
    program.InitCallback = Init;
    program.ProcessCallback = Process;
    program.DrawCallback = Draw;
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
