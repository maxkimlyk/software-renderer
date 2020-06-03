#include "../common/program.h"
#include "../renderer/camera.h"
#include "../renderer/model.h"

#include <cmath>

using namespace sr;

namespace
{
void AddQuad(std::vector<Face>& result, const Vec3f& bl, const Vec3f& br, const Vec3f& tr,
             const Vec3f& tl, const Vec3f& norm)
{
    // clang-format off
    result.push_back({Vertex(bl, norm, Vec2f{0.0f, 0.0f}),
                      Vertex(br, norm, Vec2f{1.0f, 0.0f}),
                      Vertex(tr, norm, Vec2f{1.0f, 1.0f})});
    result.push_back({Vertex(bl, norm, Vec2f{0.0f, 0.0f}),
                      Vertex(tr, norm, Vec2f{1.0f, 1.0f}),
                      Vertex(tl, norm, Vec2f{0.0f, 1.0f})});
    // clang-format on
}

std::vector<Face> GenCube(float sz)
{
    sz = 0.5f * sz;
    const Vec3f a = {sz, -sz, sz};
    const Vec3f b = {-sz, -sz, sz};
    const Vec3f c = {-sz, -sz, -sz};
    const Vec3f d = {sz, -sz, -sz};
    const Vec3f e = {sz, sz, sz};
    const Vec3f f = {-sz, sz, sz};
    const Vec3f g = {-sz, sz, -sz};
    const Vec3f h = {sz, sz, -sz};

    std::vector<Face> cube;
    cube.reserve(12);
    AddQuad(cube, c, d, a, b, Vec3f{0.0f, -1.0f, 0.0f});
    AddQuad(cube, c, b, f, g, Vec3f{-1.0f, 0.0f, 0.0f});
    AddQuad(cube, b, a, e, f, Vec3f{0.0f, 0.0f, 1.0f});
    AddQuad(cube, a, d, h, e, Vec3f{1.0f, 0.0f, 0.0f});
    AddQuad(cube, d, c, g, h, Vec3f{0.0f, 0.0f, -1.0f});
    AddQuad(cube, f, e, h, g, Vec3f{0.0f, 1.0f, 0.0f});

    return cube;
}

void PrintControls()
{
    std::cout << "Use ARROWS to rotate cube.\n";
}
} // namespace

class Demo
{
    inline static const std::string TEXTURE_PATH = "box.tga";
    inline static const float CUBE_SIZE = 1.0f;
    inline static const float ROTATE_SPEED = M_PI / 200.0f;
    inline static const float ANGLE_CHANGE_SPEED = 0.05f;

  public:
    inline static const std::string Caption = "Cube";
    static const size_t Width = 800;
    static const size_t Height = 600;

    Demo() : cube_(GenCube(CUBE_SIZE))
    {}

    int Load()
    {
        int status = LoadTGA(TEXTURE_PATH.c_str(), texture_);
        if (status != 0)
            ERROR("Could not load texture: %s\n", TEXTURE_PATH.c_str());

        PrintControls();

        return status;
    }

    void Init(Renderer& renderer)
    {
        camera_.LookAt(Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{0.0f, 1.0f, 1.5f});
    }

    void Process(Renderer& renderer, Input& input)
    {
        if (input.IsHolding(KEY_UP))
            x_angle_ += ANGLE_CHANGE_SPEED;
        if (input.IsHolding(KEY_DOWN))
            x_angle_ -= ANGLE_CHANGE_SPEED;
        if (input.IsHolding(KEY_LEFT))
            z_angle_ += ANGLE_CHANGE_SPEED;
        if (input.IsHolding(KEY_RIGHT))
            z_angle_ -= ANGLE_CHANGE_SPEED;

        angle_ += ROTATE_SPEED;
        Mat4f model_mat = Transform::RotateY(angle_) * Transform::RotateX(x_angle_) *
                          Transform::RotateZ(z_angle_);
        renderer.Matrices.SetModel(model_mat);
        renderer.Matrices.SetView(camera_.ViewMatrix());
    }

    void Draw(Renderer& renderer)
    {
        DefaultShaders::FlatTexture shader(texture_);
        renderer.SetShader(shader);

        renderer.Clear();
        for (const auto& face : cube_)
        {
            renderer.Triangle(face.v[0], face.v[1], face.v[2]);
        }
    }

  private:
    const std::vector<Face> cube_;
    Camera camera_;
    Image texture_;

    float angle_ = 0.0f;
    float x_angle_ = 0.0f;
    float z_angle_ = 0.0f;
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
