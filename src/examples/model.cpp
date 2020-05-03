#include <cmath>
#include <list>

#include "../common/program.h"
#include "../renderer/camera.h"
#include "../renderer/definitions.h"
#include "../renderer/model.h"

using namespace sr;

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Model";

const char* MODEL_NAME = "palm.obj";

Model model;
Color color(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);

float angle = 0.0f;
float angle_speed = 0.01f;
const float max_viewport = 1.5f;
const float cam_base_height = 0.5f;

void Init(Renderer& renderer)
{}

void Process(Renderer& renderer, Input& input)
{
    angle += angle_speed;
}

void Draw(Renderer& renderer)
{
    Camera camera;

    float phi = 2.0f * angle;
    float theta = 0.1f + 0.4f * sin(angle);

    float x = cos(theta) * sin(phi) * max_viewport;
    float y = cos(theta) * cos(phi) * max_viewport;
    float z = cam_base_height + sin(theta) * max_viewport;

    camera.LookAt(Vec3f{0.0f, cam_base_height, 0.0f}, Vec3f{x, z, y});
    renderer.SetViewMatrix(camera.ViewMatrix());

    renderer.Clear();
    for (auto face = model.faces.begin(); face != model.faces.end(); ++face)
    {
        Vec3f v1 = face->v[0].coord;
        Vec3f v2 = face->v[1].coord;
        Vec3f v3 = face->v[2].coord;
        renderer.TriangleFrame(v1, v2, v3, color);
    }
}

int main()
{
    ObjReader reader;
    int status = reader.ReadModel(MODEL_NAME, model);
    if (status != 0)
        return status;

    model.Normalize();

    Program program(Init, Process, Draw);
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
