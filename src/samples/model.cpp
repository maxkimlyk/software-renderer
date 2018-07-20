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

float angle = 0.0f;
float angleSpeed = 0.01;
const float maxViewpoint = .5f;

void Process()
{
    static const float PI = 3.14159265f;
    angle += angleSpeed;
    if (angle > 2 * PI)
        angle -= 2 * PI;
}

void Draw(Renderer &renderer)
{
    float x = sin(2.0f * angle) * maxViewpoint;
    float y = cos(2.0f * angle) * maxViewpoint;

    renderer.camera.LookAt(
        Vec3f {0.0f, 0.0f, 0.0f},
        Vec3f {x, 0.0f, y},
        Vec3f {0.0f, 1.0f, 0.0f});

    renderer.UpdateTransformMatrix();

    renderer.Clear();
    for (auto face = model.faces.begin(); face != model.faces.end(); ++face)
    {
        Vec3f v1 = face->v[0].coord;
        Vec3f v2 = face->v[1].coord;
        Vec3f v3 = face->v[2].coord;
        renderer.TriangleMesh(v1, v2, v3, color);
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
