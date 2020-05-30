#include "../common/program.h"
#include <cmath>
#include <vector>

using namespace sr;

class Demo
{
    inline static const size_t POINTS_AMOUNT = 100;
    inline static const float VARIANCE = 4.0f;
    inline static float Z = -5.0f;

  public:
    static const size_t Width = 800;
    static const size_t Height = 600;
    inline static const std::string Caption = "Triangles";

    void Init(sr::Renderer& renderer)
    {
        const auto randf = []() { return (float)(rand()) / (float)(RAND_MAX)-0.5f; };

        points_.reserve(POINTS_AMOUNT);
        for (size_t i = 0; i < POINTS_AMOUNT; ++i)
        {
            float x = randf() * VARIANCE;
            float y = randf() * VARIANCE;
            points_.emplace_back(Vec3f{x, y, 0.0f});
        }
    }

    void Process(sr::Renderer& renderer, Input& input)
    {}

    void Draw(sr::Renderer& renderer)
    {
        static const Color red(230, 0, 0);
        static const Color green(0, 230, 0);
        static const Color yellow(230, 230, 0);
        static const Color margenta(230, 0, 230);
        static const Color cyan(0, 230, 230);
        static const Color white(230, 230, 230);

        renderer.Clear();
        renderer.SetProjMatrix(Projection::Ortho(-2.5f, 2.5f, -2.5f, 2.5f, 0.0f, 10.0f));

        for (Vec3f v : points_)
        {
            v.z = Z;
            renderer.Triangle(v, v, v, white);
        }

        // Triangles
        renderer.Triangle(Vec3f{0, 0, Z}, Vec3f{0, 0.5, Z}, Vec3f{0.5, 0, Z}, yellow);
        renderer.Triangle(Vec3f{1, 0, Z}, Vec3f{1.4f, 1, Z}, Vec3f{1.6f, -0.6f, Z}, green);
        renderer.Triangle(Vec3f{-1, 0, Z}, Vec3f{-1.5f, 1, Z}, Vec3f{-0.8f, 1, Z}, red);
        renderer.Triangle(Vec3f{0, 1.3f, Z}, Vec3f{-1.5f, 1.3f, Z}, Vec3f{-0.8f, 1.8f, Z},
                          margenta);

        // Frame
        renderer.Triangle(Vec3f{-2, 1.5, Z - 1}, Vec3f{-2, 2, Z - 1}, Vec3f{-2, -2, Z - 1}, cyan);
        renderer.Triangle(Vec3f{2, 2, Z - 1}, Vec3f{2, 2, Z - 1}, Vec3f{2, -2, Z - 1}, cyan);
        renderer.Triangle(Vec3f{2, 2, Z - 1}, Vec3f{2, 2, Z - 1}, Vec3f{-2, 2, Z - 1}, cyan);
        renderer.Triangle(Vec3f{2, -2, Z - 1}, Vec3f{1.1f, -2, Z - 1}, Vec3f{-2, -2, Z - 1}, cyan);
        renderer.Triangle(Vec3f{-2, 2, Z - 1}, Vec3f{-2, 2, Z - 1}, Vec3f{2, -2, Z - 1}, cyan);
        renderer.Triangle(Vec3f{2, 2, Z - 1}, Vec3f{2, 2, Z - 1}, Vec3f{-2, -2, Z - 1}, cyan);
    }

  private:
    std::vector<Vec3f> points_;
};

int main()
{
    Demo demo;

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
