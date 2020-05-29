#include "../common/program.h"
#include "../renderer/camera.h"
#include "../renderer/model.h"

using namespace sr;

namespace
{
void AddQuad(std::vector<Face>& result, const Vec3f& bl, const Vec3f& tr, const Vec3f& norm)
{
    // clang-format off
    result.push_back({Vertex(bl, norm, Vec2f{0.0f, 0.0f}),
                      Vertex(Vec3f{tr.x, bl.y, bl.z}, norm, Vec2f{1.0f, 0.0f}),
                      Vertex(tr, norm, Vec2f{1.0f, 1.0f})});
    result.push_back({Vertex(bl, norm, Vec2f{0.0f, 0.0f}),
                      Vertex(tr, norm, Vec2f{1.0f, 1.0f}),
                      Vertex(Vec3f{bl.x, tr.y, tr.z}, norm, Vec2f{0.0f, 1.0f})});
    // clang-format on
}

std::vector<Face> InitModel()
{
    const float offset = 2.0f;
    const float z_offset = 1.0f;
    const size_t count = 3;

    std::vector<Face> result;
    const Vec3f norm = {0.0f, 0.0f, 1.0f};

    const float min_y = -offset * (count / 2);

    for (size_t i = 0; i < count; ++i)
    {
        // clang-format off
        AddQuad(result,
                Vec3f{-0.5f, min_y + i * offset - 0.5f, 0.0f + z_offset},
                Vec3f{0.5f, min_y + i * offset + 0.5f, 0.0f + z_offset},
                norm);
        // clang-format off
    }

    return result;
}
} // namespace

class Demo
{
  public:
    inline static const std::string Caption = "Scene";
    static const size_t Width = 800;
    static const size_t Height = 600;

    void Init(Renderer&)
    {
        faces_ = InitModel();
        camera_.LookAt(Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{0.0f, 0.0f, 100.0f});
    }

    void Process(Renderer& renderer, Input& input)
    {
        const float walk_distance = 0.5f;
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

        renderer.SetViewMatrix(camera_.ViewMatrix());
    }

    void Draw(Renderer& renderer)
    {
        renderer.Clear();
        for (const auto& face : faces_) {
            renderer.Triangle(face.v[0], face.v[1], face.v[2]);
        }
    }

  private:
    std::vector<Face> faces_;
    Camera camera_;
};

int main()
{
    Demo demo;

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
