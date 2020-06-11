#include "../common/program.h"
#include "../renderer/camera.h"
#include "../renderer/model.h"

using namespace sr;

namespace
{
Vec3f ApplyTransform(const Vec3f& vec, const Mat4f& transform)
{
    const Vec4f extended = Embed<4, float>(vec, 0);
    const Vec4f transformed = transform * extended;
    return Project<3, float>(transformed);
}

void PrintUsage()
{
    std::cout << "Controls:\n"
              << "'W','A','S','D' to walk\n"
              << "'E','Q' to move up and down\n"
              << "ARROWS to look around\n"
              << "'Z','X' to change light direction\n"
              << "'T' to toggle texture\n";
}
} // namespace

class Demo
{
  public:
    inline static const std::string Caption = "Head";
    static const size_t Width = 800;
    static const size_t Height = 600;

    Demo() : textured_shader_(texture_), used_shader(&colored_shader_)
    {}

    int Load()
    {
        static const std::string model_name = "skull.obj";
        static const std::string texture_name = "skull_diffuse.tga";

        ObjReader reader;
        int status;

        LOG("Loading resouces...\n");

        status = reader.ReadModel(model_name.c_str(), model_);
        if (status != 0)
        {
            ERROR("Failed to load model_ %s\n", model_name.c_str());
            return status;
        }
        model_.Normalize();

        status = LoadTGA(texture_name.c_str(), texture_);
        if (status != 0)
        {
            ERROR("Failed to texture_ %s\n", texture_name.c_str());
            return status;
        }

        LOG("Loaded succesfully\n\n");

        PrintUsage();

        return 0;
    }

    void Init(Renderer&)
    {
        camera_.LookAt(Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{0.0f, 0.5f, 1.5f});
    }

    void Process(Renderer& renderer, Input& input)
    {
        const float walk_distance = 0.01f;
        const float rotate_angle = 0.05f;

        if (input.IsHolding(KEY_W))
            camera_.Walk(walk_distance);
        if (input.IsHolding(KEY_S))
            camera_.Walk(-walk_distance);
        if (input.IsHolding(KEY_A))
            camera_.WalkRight(-walk_distance);
        if (input.IsHolding(KEY_D))
            camera_.WalkRight(walk_distance);
        if (input.IsHolding(KEY_E))
            camera_.RiseUp(walk_distance);
        if (input.IsHolding(KEY_Q))
            camera_.RiseUp(-walk_distance);

        if (input.IsHolding(KEY_LEFT))
            camera_.Yaw(rotate_angle);
        if (input.IsHolding(KEY_RIGHT))
            camera_.Yaw(-rotate_angle);
        if (input.IsHolding(KEY_UP))
            camera_.Pitch(rotate_angle);
        if (input.IsHolding(KEY_DOWN))
            camera_.Pitch(-rotate_angle);

        if (input.IsHolding(KEY_Z))
            RotateLight(-rotate_angle);
        if (input.IsHolding(KEY_X))
            RotateLight(rotate_angle);

        if (input.IsPressed(KEY_T))
            ToggleShaders();

        renderer.Matrices.SetView(camera_.ViewMatrix());
    }

    void Draw(Renderer& renderer)
    {
        renderer.Clear();

        renderer.SetShader(*used_shader);
        for (auto face = model_.faces.begin(); face != model_.faces.end(); ++face)
        {
            const Vertex& v1 = face->v[0];
            const Vertex& v2 = face->v[1];
            const Vertex& v3 = face->v[2];
            renderer.Triangle(v1, v2, v3);
        }
    }

  private:
    void RotateLight(float angle)
    {
        const Mat4f transform = Transform::RotateY(angle);
        light_direction_ = ApplyTransform(light_direction_, transform);
        colored_shader_.light_direction = light_direction_;
        textured_shader_.light_direction = light_direction_;
    }

    void ToggleShaders()
    {
        if (used_shader == &colored_shader_)
            used_shader = &textured_shader_;
        else
            used_shader = &colored_shader_;
    }

    Model model_;
    Image texture_;
    Camera camera_;
    Vec3f light_direction_ = Vec3f{0.0f, 0.0f, 1.0f};

    DefaultShaders::SmoothLight colored_shader_;
    DefaultShaders::SmoothTexture textured_shader_;
    Shader* used_shader;
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
