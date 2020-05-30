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
    const float z_offset = 0.5f;
    const size_t count = 3;

    std::vector<Face> result;
    const Vec3f norm = {0.0f, 0.0f, 1.0f};

    const float min_y = -offset * (count / 2);

    for (size_t i = 0; i < count; ++i)
    {
        // clang-format off
        AddQuad(result,
                Vec3f{-0.5f, min_y + i * offset - 0.5f, 0.0f + i * z_offset},
                Vec3f{0.5f, min_y + i * offset + 0.5f, 0.0f + i * z_offset},
                norm);
        // clang-format off
    }

    return result;
}

void PrintHelp() {
    std::cout << "Use 'S' and 'W' to move backward and forward.\n"
              << "If projection and clipping work right, "
              << "quads should disappear when you moving backward.\n"
              << "The lowest quad should disappear first, then the quad just above.";
}
} // namespace

class Demo
{
    inline static float Z_NEAR = 0.05f;
    inline static float Z_FAR = 10.0f;
    inline static float CAM_START_Z = Z_FAR - 0.01f;

  public:
    inline static const std::string Caption = "Scene";
    static const size_t Width = 800;
    static const size_t Height = 600;

    void Init(Renderer& renderer)
    {
        faces_ = InitModel();
        camera_.LookAt(Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{0.0f, 0.0f, CAM_START_Z});

        const Mat4f proj_mat = Projection::Perspective(45.0f, (float)(Width) / (float)(Height), Z_NEAR, Z_FAR);
        renderer.SetProjMatrix(proj_mat);

        PrintHelp();
    }

    void Process(Renderer& renderer, Input& input)
    {
        const float walk_distance = 0.01f;
        const float rotate_angle = 0.05f;

        if (input.IsHolding(KEY_W))
            camera_.Walk(walk_distance);
        if (input.IsHolding(KEY_S))
            camera_.Walk(-walk_distance);

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
