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
    result.push_back({Vertex(bl, norm), Vertex(tr, norm), Vertex(br, norm)});
    result.push_back({Vertex(bl, norm), Vertex(tl, norm), Vertex(tr, norm)});
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

std::vector<Face> GenChecker()
{
    const int segments = 16;
    const float radius = 0.45f;
    const float height = 0.15f;

    const float delta_angle = 2 * M_PI / segments;

    auto x = [radius, delta_angle](float i) { return radius * sinf(delta_angle * i); };
    auto y = [radius, delta_angle](float i) { return radius * cosf(delta_angle * i); };

    std::vector<Face> model;

    for (int i = 0; i < segments; ++i)
    {
        Face f;
        f.v[0] = {Vec3f{0.0f, 0.0f, height}, Vec3f{0.0f, 0.0f, 1.0f}};
        f.v[1] = {Vec3f{x(i), y(i), height}, Vec3f{0.0f, 0.0f, 1.0f}};
        f.v[2] = {Vec3f{x(i + 1), y(i + 1), height}, Vec3f{0.0f, 0.0f, 1.0f}};
        model.push_back(f);
    }

    for (int i = 0; i < segments; ++i)
    {
        Vec3f bl = {x(i), y(i), 0.0f};
        Vec3f br = {x(i + 1), y(i + 1), 0.0f};
        Vec3f tr = {x(i + 1), y(i + 1), height};
        Vec3f tl = {x(i), y(i), height};
        Vec3f norm = {x(i + 0.5f), y(i + 0.5f), 0.0f};
        AddQuad(model, bl, br, tr, tl, norm);
    }

    return model;
}

void DrawModel(Renderer& renderer, const std::vector<Face>& faces)
{
    for (const auto& face : faces)
    {
        renderer.Triangle(face.v[0], face.v[1], face.v[2]);
    }
}

class GameCamera
{
    inline static Vec3f center_ = {0.0f, 0.0f, 0.0f};
    inline static Vec3f up_direction_ = {0.0f, 0.0f, 1.0f};

  public:
    GameCamera(float distance = 7.0f, float phi = 0.0f, float theta = 2.0f * M_PI / 3.0f)
        : distance_(distance), phi_(phi), theta_(theta)
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

        if (input.IsHolding(KEY_J))
            RotateLight(rotate_angle);
        if (input.IsHolding(KEY_K))
            RotateLight(-rotate_angle);

        renderer.Matrices.SetView(camera_.GetViewMatrix());
    }

    void RotateLight(float angle)
    {
        light_direction_ = TransformVector(light_direction_, Transform::RotateZ(angle));
        std::cout << light_direction_ << std::endl;
    }

    void DrawBoard(Renderer& renderer)
    {
        static const std::vector<Face> box = GenBox();
        const Mat4f mat_scale = Transform::Scale(1.0f, 1.0f, -0.2f);

        static const Color white_color = Color(210, 210, 210);
        static const Color black_color = Color(100, 100, 100);
        static const float ambient_light_intensity = 0.3f;

        DefaultShaders::FlatLight shader;
        shader.ambient_light_intensity = ambient_light_intensity;
        shader.light_direction = light_direction_;
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
                renderer.Matrices.Transform(mat_scale *
                                            Transform::Translate(i * 1.0f, j * 1.0f, 0.0f));

                const Mat4f model_mat = renderer.Matrices.GetModel();
                std::cout << "M: " << model_mat << std::endl;
                const Mat4f norm_corr_mat = Inverse(Transpose(model_mat));
                std::cout << "(M^T)^-1: " << norm_corr_mat << std::endl;
                shader.SetNormCorrection(norm_corr_mat);

                DrawModel(renderer, box);
                renderer.Matrices.Pop();
            }
        }
    }

    void DrawChecker(Renderer& renderer, int i, int j, bool is_white)
    {
        static const std::vector<Face> model = GenChecker();

        static const Color checker_white_color = Color(230, 230, 230);
        static const Color checker_black_color = Color(60, 60, 60);
        static const float ambient_light_intensity = 0.3f;

        DefaultShaders::FlatLight shader;
        shader.ambient_light_intensity = ambient_light_intensity;
        shader.color = is_white ? checker_white_color : checker_black_color;
        shader.light_direction = light_direction_;
        renderer.SetShader(shader);

        renderer.Matrices.SetMode(MatrixType::MODEL);

        renderer.Matrices.Push();
        renderer.Matrices.Transform(
            Transform::Translate((i + 0.5f) * 1.0f, (j + 0.5f) * 1.0f, 0.0f));

        const Mat4f model_mat = renderer.Matrices.GetModel();
        // std::cout << "M: " << model_mat << std::endl;
        const Mat4f norm_corr_mat = Inverse(Transpose(model_mat));
        // std::cout << "(M^T)^-1: " << norm_corr_mat << std::endl;
        shader.SetNormCorrection(norm_corr_mat);

        DrawModel(renderer, model);
        renderer.Matrices.Pop();
    }

    void Draw(Renderer& renderer)
    {
        renderer.Clear();

        renderer.Matrices.SetMode(MatrixType::MODEL);
        renderer.Matrices.Set(Mat4f::Identity());
        renderer.Matrices.Transform(Transform::Scale(1.0f, -1.0f, 1.0f));
        renderer.Matrices.Transform(Transform::Translate(-4.0f, -4.0f, 0.0f));

        DrawBoard(renderer);

        // clang-format off
        const char tmp_board[8][8] = {
            {'w', ' ', 'w', ' ', 'w', ' ', 'w', ' '},
            {' ', 'w', ' ', 'w', ' ', 'w', ' ', 'w'},
            {'w', ' ', 'w', ' ', 'w', ' ', 'w', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', 'b', ' ', 'b', ' ', 'b', ' ', 'b'},
            {'b', ' ', 'b', ' ', 'b', ' ', 'b', ' '},
            {' ', 'b', ' ', 'b', ' ', 'b', ' ', 'b'}
        };
        // clang-format on
        for (size_t i = 0; i < 8; ++i)
        {
            for (size_t j = 0; j < 8; ++j)
            {
                if (tmp_board[i][j] != ' ')
                {
                    const bool is_white = tmp_board[i][j] == 'w';
                    DrawChecker(renderer, i, j, is_white);
                }
            }
        }
    }

  private:
    GameCamera camera_;
    Vec3f light_direction_ = Normalize(Vec3f{0.2f, 0.2f, -1.0f});
};

int main()
{
    Demo demo;

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
