#include "../common/program.h"
#include "../common/model.h"
#include "../common/camera.h"

#include <cmath>

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Scene";
const char *MODEL_NAME = "palm.obj";

Model model;
Color color(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);
Vec3f lightDirection = Normalize(Vec3f {0, -0.2f, 3.0f});

Camera camera;

void Init(Renderer &renderer)
{
    camera.LookAt(
        Vec3f {0.0f, 0.0f, 0.0f},
        Vec3f {0.0f, 1.0f, 3.0f}
    );
}

void Process(Renderer &renderer, Input &input)
{
    const float walkDistance = 0.01f;
    const float rotateAngle = 0.05f;

    if (input.IsDown('W'))
        camera.Walk(walkDistance);
    if (input.IsDown('S'))
        camera.Walk(-walkDistance);
    if (input.IsDown('A'))
        camera.WalkRight(-walkDistance);
    if (input.IsDown('D'))
        camera.WalkRight(walkDistance);
    if (input.IsDown(VK_LEFT))
        camera.Yaw(rotateAngle);
    if (input.IsDown(VK_RIGHT))
        camera.Yaw(-rotateAngle);
    if (input.IsDown(VK_UP))
        camera.Pitch(rotateAngle);
    if (input.IsDown(VK_DOWN))
        camera.Pitch(-rotateAngle);

    renderer.viewMatrix = camera.ViewMatrix();
    renderer.UpdateMatrices();
}

void Draw(Renderer &renderer)
{
    renderer.Clear();
    for (auto face = model.faces.begin(); face != model.faces.end(); ++face)
    {
        Vec3f v1 = face->v[0].coord;
        Vec3f v2 = face->v[1].coord;
        Vec3f v3 = face->v[2].coord;

        Vec3f norm = Normalize( Cross(v3 - v1, v3 - v2) );
        float lightIntensity = lightDirection * norm;

        if (lightIntensity > 0)
        {
            Color color(255 * lightIntensity, 255 * lightIntensity, 255 * lightIntensity);
            renderer.Triangle(v1, v2, v3, color);
        }

        // renderer.TriangleMesh(v1, v2, v3, color);

    }

    // renderer.TriangleMesh(
    //     Vec3f {5.0f, 0.0f, 5.0f},
    //     Vec3f {5.0f, 0.0f, -5.0f},
    //     Vec3f {-5.0f, 0.0f, -5.0f},
    //     color
    // );

    // renderer.TriangleMesh(
    //     Vec3f {-1.0f, 0.0f, 1.0f},
    //     Vec3f {1.0f, 0.0f, 1.0f},
    //     Vec3f {1.0f, 1.0f, 1.0f},
    //     color
    // );
}

int main()
{
    ObjReader reader;
    int status = reader.ReadModel(MODEL_NAME, model);
    if (status != 0)
        return status;

    model.Normalize();

    Program program;
    program.InitCallback = Init;
    program.ProcessCallback = Process;
    program.DrawCallback = Draw;
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
