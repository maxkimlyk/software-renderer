#include "../common/program.h"
#include "../common/model.h"
#include <cmath>
#include <list>

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Model";

const char *MODEL_NAME = "palm.obj";

Model model;
Color color(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);

void Process()
{

}

void Draw(Renderer &renderer)
{
    static const Vec2i origin = {WIDTH / 2, HEIGHT / 2};
    static const float xf = WIDTH / 2;
    static const float yf = HEIGHT / 2;

    renderer.Clear();
    for (auto face = model.faces.begin(); face != model.faces.end(); ++face)
    {
        Vec3f v1 = face->v[0].coord;
        Vec3f v2 = face->v[1].coord;
        Vec3f v3 = face->v[2].coord;
        Vec2i p1 = Vec2i {(int)(v1[0] * xf), (int)(v1[1] * yf)} + origin;
        Vec2i p2 = Vec2i {(int)(v2[0] * xf), (int)(v2[1] * yf)} + origin;
        Vec2i p3 = Vec2i {(int)(v3[0] * xf), (int)(v3[1] * yf)} + origin;
        renderer.Line(p1, p2, color);
        renderer.Line(p2, p3, color);
        renderer.Line(p3, p1, color);
    }
}

int main()
{
    ObjReader reader;
    int status = reader.ReadModel(MODEL_NAME, model);
    if (status != 0)
        return status;

    model.Normalize();

    Program program;
    program.ProcessCallback = Process;
    program.DrawCallback = Draw;
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
