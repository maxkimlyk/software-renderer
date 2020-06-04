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
    result.push_back({Vertex(bl, norm), Vertex(br, norm), Vertex(tr, norm)});
    result.push_back({Vertex(bl, norm), Vertex(tr, norm), Vertex(tl, norm)});
}

std::vector<Face> GenBox()
{
    const Vec3f a = {1, 0, 1};
    const Vec3f b = {0, 0, 1};
    const Vec3f c = {0, 0, 0};
    const Vec3f d = {1, 0, 0};
    const Vec3f e = {1, 1, 1};
    const Vec3f f = {0, 1, 1};
    const Vec3f g = {0, 1, 0};
    const Vec3f h = {1, 1, 0};

    std::vector<Face> box;
    box.reserve(12);
    AddQuad(box, c, d, a, b, Vec3f{0.0f, -1.0f, 0.0f});
    AddQuad(box, c, b, f, g, Vec3f{-1.0f, 0.0f, 0.0f});
    AddQuad(box, b, a, e, f, Vec3f{0.0f, 0.0f, 1.0f});
    AddQuad(box, a, d, h, e, Vec3f{1.0f, 0.0f, 0.0f});
    AddQuad(box, d, c, g, h, Vec3f{0.0f, 0.0f, -1.0f});
    AddQuad(box, f, e, h, g, Vec3f{0.0f, 1.0f, 0.0f});

    return box;
}

void DrawObject(Renderer& renderer, const std::vector<Face>& faces)
{
    for (const auto& face : faces)
    {
        renderer.Triangle(face.v[0], face.v[1], face.v[2]);
    }
}

void DrawBoard(Renderer& renderer)
{
    const std::vector<Face> box = GenBox();
    const Mat4f mat_scale = Transform::Scale(1.0f, 1.0f, 0.2f);

    static const Color white_color = Color(210, 210, 210);
    static const Color black_color = Color(80, 80, 80);
    static const float ambient_light_intensity = 0.3f;

    DefaultShaders::FlatLight shader;
    shader.ambient_light_intensity = ambient_light_intensity;
    renderer.SetShader(shader);

    renderer.Matrices.SetMode(MatrixType::MODEL);

    for (size_t i = 0; i < 8; ++i)
    {
        for (size_t j = 0; j < 8; ++j)
        {
            if ((i + j) % 2 == 0)
                shader.color = black_color;
            else
                shader.color = white_color;

            renderer.Matrices.Push();
            renderer.Matrices.ApplyTransform(mat_scale *
                                             Transform::Translate(i * 1.0f, j * 1.0f, 0.0f));
            DrawObject(renderer, box);
            renderer.Matrices.Pop();
        }
    }
}

class GameCamera
{
  public:
    GameCamera(float distance = 7.0f, float phi = 0.0f, float theta = M_PI / 4.0f)
        : center_({0.0f, 0.0f, 0.0f}), up_direction_({0.0f, 0.0f, 1.0f}), distance_(distance),
          phi_(phi), theta_(theta)
    {}

    void Yaw(float delta)
    {
        phi_ += delta;
    }

    void Pitch(float delta)
    {
        theta_ += delta;
    }

    Mat4f GetViewMatrix() const
    {
        const Vec3f direction = {cosf(phi_) * cosf(theta_), sinf(phi_) * cosf(theta_),
                                 sinf(theta_)};
        const Vec3f position = distance_ * direction;
        return Transform::LookAt(center_, position, up_direction_);
    }

  private:
    const Vec3f center_;
    const Vec3f up_direction_;

    float distance_;
    float phi_;
    float theta_;
};

} // namespace

class Demo
{
  public:
    inline static const std::string Caption = "Scene";
    static const size_t Width = 800;
    static const size_t Height = 600;

    void Init(Renderer& renderer)
    {}

    void Process(Renderer& renderer, Input& input)
    {
        const float rotate_angle = 0.01f;

        if (input.IsHolding(KEY_LEFT))
            camera_.Yaw(-rotate_angle);
        if (input.IsHolding(KEY_RIGHT))
            camera_.Yaw(rotate_angle);
        if (input.IsHolding(KEY_UP))
            camera_.Pitch(rotate_angle);
        if (input.IsHolding(KEY_DOWN))
            camera_.Pitch(-rotate_angle);

        renderer.Matrices.SetView(camera_.GetViewMatrix());
    }

    void Draw(Renderer& renderer)
    {
        DefaultShaders::FlatLight shader;
        shader.color = Color(200, 200, 200);
        renderer.SetShader(shader);

        renderer.Clear();

        renderer.Matrices.SetModel(Transform::Translate(-4.0f, -4.0f, 0.0f));

        DrawBoard(renderer);
    }

  private:
    GameCamera camera_;
};

int main()
{
    Demo demo;

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
