#include "../common/program.h"
#include "../renderer/camera.h"
#include "../renderer/model.h"

#include <cmath>

using namespace sr;

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Scene";
const char* MODEL_NAME = "head.obj";
const char* TEXTURE_FILE = "head_diffuse.tga";

Model model;
Image texture;

Vec3f lightDirection = Normalize(Vec3f{0, -0.2f, 3.0f});

Camera camera;

void Init(Renderer& renderer)
{
    camera.LookAt(Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{0.0f, 1.0f, 3.0f});
}

void Process(Renderer& renderer, Input& input)
{
    const float walkDistance = 0.01f;
    const float rotateAngle = 0.05f;

    if (input.IsHolding(KEY_W))
        camera.Walk(walkDistance);
    if (input.IsHolding(KEY_S))
        camera.Walk(-walkDistance);
    if (input.IsHolding(KEY_A))
        camera.WalkRight(-walkDistance);
    if (input.IsHolding(KEY_D))
        camera.WalkRight(walkDistance);
    if (input.IsHolding(KEY_E))
        camera.RiseUp(walkDistance);
    if (input.IsHolding(KEY_Q))
        camera.RiseUp(-walkDistance);

    if (input.IsHolding(KEY_LEFT))
        camera.Yaw(rotateAngle);
    if (input.IsHolding(KEY_RIGHT))
        camera.Yaw(-rotateAngle);
    if (input.IsHolding(KEY_UP))
        camera.Pitch(rotateAngle);
    if (input.IsHolding(KEY_DOWN))
        camera.Pitch(-rotateAngle);

    renderer.SetViewMatrix(camera.ViewMatrix());
}

void Draw(Renderer& renderer)
{
    DefaultShaders::PhongShader shader;
    renderer.SetShader(shader);
    renderer.Clear();
    for (auto face = model.faces.begin(); face != model.faces.end(); ++face)
    {
        const Vertex& v1 = face->v[0];
        const Vertex& v2 = face->v[1];
        const Vertex& v3 = face->v[2];
        renderer.Triangle(v1, v2, v3);
    }
}

int main()
{
    ObjReader reader;
    int status;

    status = reader.ReadModel(MODEL_NAME, model);
    if (status != 0)
        return status;

    status = LoadTGA(TEXTURE_FILE, texture);
    if (status != 0)
        return status;

    model.Normalize();

    Program program(Init, Process, Draw);
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
