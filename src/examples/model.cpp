#include <cmath>
#include <list>

#include "../common/program.h"
#include "../renderer/camera.h"
#include "../renderer/definitions.h"
#include "../renderer/model.h"

using namespace sr;

class Demo
{
    inline static const std::string MODEL_NAME = "palm.obj";
    inline static const float CAM_DISTANCE = 0.8f;
    inline static const float CAM_BASE_HEIGHT = 0.5f;
    inline static const Color COLOR = Color(211, 211, 178);
    inline static const float SPEED = 0.01f;

  public:
    static const size_t Width = 800;
    static const size_t Height = 600;
    inline static const std::string Caption = "Model";

    int Load()
    {
        LOG("Loading...\n");

        ObjReader reader;
        int status = reader.ReadModel(MODEL_NAME.c_str(), model_);
        if (status != 0)
        {
            ERROR("Failed to load model_: %s\n", MODEL_NAME.c_str());
            return status;
        }

        model_.Normalize();

        LOG("Loaded succesfully\n");

        return 0;
    }

    void Init(Renderer& renderer)
    {}

    void Process(Renderer& renderer, Input& input)
    {
        angle_ += SPEED;
    }

    void Draw(Renderer& renderer)
    {

        float phi = 2.0f * angle_;
        float theta = 0.1f + 0.4f * sin(angle_);

        float x = cos(theta) * sin(phi) * CAM_DISTANCE;
        float y = cos(theta) * cos(phi) * CAM_DISTANCE;
        float z = CAM_BASE_HEIGHT + sin(theta) * CAM_DISTANCE;

        camera_.LookAt(Vec3f{0.0f, CAM_BASE_HEIGHT, 0.0f}, Vec3f{x, z, y});
        renderer.SetViewMatrix(camera_.ViewMatrix());

        renderer.Clear();
        for (auto face = model_.faces.begin(); face != model_.faces.end(); ++face)
        {
            Vec3f v1 = face->v[0].coord;
            Vec3f v2 = face->v[1].coord;
            Vec3f v3 = face->v[2].coord;
            renderer.TriangleFrame(v1, v2, v3, COLOR);
        }
    }

  private:
    Model model_;
    Camera camera_;

    float angle_ = 0.0f;
};

int main()
{
    Demo demo;

    if (int status = demo.Load(); status != 0)
    {
        return status;
    }

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
