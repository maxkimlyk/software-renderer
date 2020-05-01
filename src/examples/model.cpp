#include "../renderer/model.h"
#include "../common/program.h"
#include "../renderer/camera.h"
#include <cmath>
#include <list>

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Model";

const char* MODEL_NAME = "palm.obj";

Model model;
Color color(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);

float angle = 0.0f;
float angleSpeed = 0.01f;
const float maxViewpoint = 3.0f;

void Init(sr::Renderer& renderer)
{}

void Process(sr::Renderer& renderer, Input& input)
{
    static const float PI = 3.14159265f;
    angle += angleSpeed;
    if (angle > 2 * PI)
        angle -= 2 * PI;
}

void Draw(sr::Renderer& renderer)
{
    static Camera camera;

    float x = 0.5f * sin(2.0f * angle) * maxViewpoint;
    float y = 0.5f * cos(2.0f * angle) * maxViewpoint;

    camera.LookAt(Vec3f{0.0f, 0.5f, 0.0f}, Vec3f{x, 1.0f, y});

    renderer.viewMatrix = camera.ViewMatrix();
    renderer.UpdateMatrices();

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

    sr::Program program;
    program.InitCallback = Init;
    program.ProcessCallback = Process;
    program.DrawCallback = Draw;
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
