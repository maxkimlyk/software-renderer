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

void AddQuad(std::vector<Face>& result, const Vertex& bl, const Vertex& br, const Vertex& tr,
             const Vertex& tl)
{
    result.push_back({bl, tr, br});
    result.push_back({bl, tl, tr});
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
    const float radius = 0.4f;
    const float height = 0.15f;

    const float alpha = 0.1f;
    const float k = 3;
    const float l = (1.0f - alpha) * radius;
    const float omega = k * 2 * M_PI / l;
    const float t0 = alpha * radius;
    const float t_inc = l / (4 * k);

    const float delta_angle = 2 * M_PI / segments;
    auto x = [delta_angle](float radius, float i) { return radius * sinf(delta_angle * i); };
    auto y = [delta_angle](float radius, float i) { return radius * cosf(delta_angle * i); };

    auto f = [omega, t0](float t) { return sinf(omega * (t - t0)); };
    auto der_f = [omega, t0](float t) { return omega * cosf(omega * (t - t0)); };

    auto calc_norm_for_curve = [&der_f](float t, const Vec3f& x_dir) {
        const Vec3f z_dir = {0.0f, 0.0f, 1.0f};
        const Vec2f norm_2d = Normalize(Vec2f{-der_f(t), 1.0f});
        const Vec3f norm_3d = x_dir * norm_2d.x + z_dir * norm_2d.y;
        return norm_3d;
    };

    std::vector<Face> model;

    for (int i = 0; i < segments; ++i)
    {
        Face f;
        f.v[0] = {Vec3f{0.0f, 0.0f, height}, Vec3f{0.0f, 0.0f, 1.0f}};
        f.v[1] = {Vec3f{x(t0, i), y(t0, i), height}, Vec3f{0.0f, 0.0f, 1.0f}};
        f.v[2] = {Vec3f{x(t0, i + 1), y(t0, i + 1), height}, Vec3f{0.0f, 0.0f, 1.0f}};
        model.push_back(f);

        const Vec3f x_dir_left = Normalize(Vec3f{x(t0, i), y(t0, i), 0.0f});
        const Vec3f x_dir_right = Normalize(Vec3f{x(t0, i + 1), y(t0, i + 1), 0.0f});

        for (float t = t0; t < radius; t += t_inc)
        {
            const float t1 = t + t_inc;
            const Vertex tl = {Vec3f{x(t, i), y(t, i), height}, //
                               calc_norm_for_curve(t, x_dir_left)};
            const Vertex tr = {Vec3f{x(t, i + 1), y(t, i + 1), height},
                               calc_norm_for_curve(t, x_dir_right)};
            const Vertex bl = {Vec3f{x(t1, i), y(t1, i), height},
                               calc_norm_for_curve(t1, x_dir_left)};
            const Vertex br = {Vec3f{x(t1, i + 1), y(t1, i + 1), height},
                               calc_norm_for_curve(t1, x_dir_right)};
            AddQuad(model, bl, br, tr, tl);
        }
    }

    for (int i = 0; i < segments; ++i)
    {
        Vec3f bl = {x(radius, i), y(radius, i), 0.0f};
        Vec3f br = {x(radius, i + 1), y(radius, i + 1), 0.0f};
        Vec3f tr = {x(radius, i + 1), y(radius, i + 1), height};
        Vec3f tl = {x(radius, i), y(radius, i), height};
        Vec3f norm = {x(radius, i + 0.5f), y(radius, i + 0.5f), 0.0f};
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

class Demo
{
  public:
    inline static const std::string Caption = "Scene";
    static const size_t Width = 800;
    static const size_t Height = 600;

    void Init(Renderer& renderer);
    void Process(Renderer& renderer, Input& input);
    void Draw(Renderer& renderer);

  private:
    void RotateLight(float angle);
    void DrawBoard(Renderer& renderer);
    void DrawBoardStencil(Renderer& renderer, const Mat4f& mat_scale, const std::vector<Face>& box);
    void DrawChecker(Renderer& renderer, int i, int j, bool is_white, bool is_under_cursor);
    std::optional<Vec2i> GetCellUnderPointer(int x, int y) const;
    bool IsCellUnderCursor(int i, int j) const;

    GameCamera camera_;
    Vec3f light_direction_ = Normalize(Vec3f{0.8f, 0.8f, -1.0f});
    Image board_stencil_;
    std::optional<Vec2i> cell_under_cursor_;
};

void Demo::Init(Renderer& renderer)
{
    board_stencil_.Resize(Width, Height);
}

void Demo::Process(Renderer& renderer, Input& input)
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

    if (input.IsHolding(KEY_RBUTTON))
    {
        const float sensitivity = 0.25f;
        const auto [deltax, deltay] = input.GetMouseMovement();
        camera_.Yaw(-deltax * sensitivity * rotate_angle);
        camera_.Pitch(-deltay * sensitivity * rotate_angle);
    }

    if (input.IsHolding(KEY_J))
        RotateLight(rotate_angle);
    if (input.IsHolding(KEY_K))
        RotateLight(-rotate_angle);

    const auto [x, y] = input.GetMousePosition();
    cell_under_cursor_ = GetCellUnderPointer(x, y);

    renderer.Matrices.SetView(camera_.GetViewMatrix());
}

void Demo::Draw(Renderer& renderer)
{
    static const Color clear_color(50, 50, 35);

    renderer.Clear(clear_color);

    renderer.Matrices.SetMode(MatrixType::MODEL);
    renderer.Matrices.Set(Mat4f::Identity());
    renderer.Matrices.Transform(Transform::Scale(1.0f, -1.0f, 1.0f));
    renderer.Matrices.Transform(Transform::Translate(-4.0f, -4.0f, 0.0f));

    DrawBoard(renderer);

    // clang-format off
        const char tmp_board[8][8] = {
            {'w', ' ', 'w', ' ', 'w', ' ', 'w', ' '},
            {' ', 'w', ' ', 'w', ' ', 'w', ' ', 'w'},
            {'w', ' ', 'w', ' ', ' ', ' ', 'w', ' '},
            {' ', ' ', ' ', 'w', ' ', ' ', ' ', ' '},
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
                const bool is_under_cursor = IsCellUnderCursor(i, j);
                DrawChecker(renderer, i, j, is_white, is_under_cursor);
            }
        }
    }
}

void Demo::RotateLight(float angle)
{
    light_direction_ = TransformVector(light_direction_, Transform::RotateZ(angle));
    std::cout << light_direction_ << std::endl;
}

void Demo::DrawBoard(Renderer& renderer)
{
    static const std::vector<Face> box = GenBox();

    static const Color color_table[2][2] = {
        // not under cursor
        {Color(100, 100, 100), Color(210, 210, 210)},
        // under cursor
        {Color(120, 120, 120), Color(230, 230, 230)},
    };

    static const float ambient_light_intensity = 0.3f;

    const Mat4f mat_scale = Transform::Scale(1.0f, 1.0f, -0.2f);

    DefaultShaders::FlatLight shader;
    shader.ambient_light_intensity = ambient_light_intensity;
    shader.SetLightDirection(light_direction_);
    renderer.SetShader(shader);

    renderer.Matrices.SetMode(MatrixType::MODEL);

    for (size_t i = 0; i < 8; ++i)
    {
        for (size_t j = 0; j < 8; ++j)
        {
            const bool is_white = (i + j) % 2 == 1;
            const bool is_under_cursor = IsCellUnderCursor(i, j);
            shader.color = color_table[int(is_under_cursor)][int(is_white)];

            renderer.Matrices.Push();
            renderer.Matrices.Transform(mat_scale * Transform::Translate(i * 1.0f, j * 1.0f, 0.0f));

            const Mat4f model_mat = renderer.Matrices.GetModel();
            shader.SetNormCorrection(Inverse(Transpose(model_mat)));

            DrawModel(renderer, box);
            renderer.Matrices.Pop();
        }
    }

    DrawBoardStencil(renderer, mat_scale, box);
}

void Demo::DrawBoardStencil(Renderer& renderer, const Mat4f& mat_scale,
                            const std::vector<Face>& box)
{
    renderer.SetDrawTarget(board_stencil_);
    renderer.Clear(Color(255, 0, 0));

    DefaultShaders::SolidColor shader;
    renderer.SetShader(shader);

    for (size_t i = 0; i < 8; ++i)
    {
        for (size_t j = 0; j < 8; ++j)
        {
            if ((i + j) % 2 == 0)
            {
                const uint8_t code = i * 8 + j;
                shader.color = Color(code, 0, 0);

                renderer.Matrices.Push();
                renderer.Matrices.Transform(mat_scale *
                                            Transform::Translate(i * 1.0f, j * 1.0f, 0.0f));

                DrawModel(renderer, box); // TODO: draw only upper side
                renderer.Matrices.Pop();
            }
        }
    }

    renderer.ResetDrawTarget();
}

void Demo::DrawChecker(Renderer& renderer, int i, int j, bool is_white, bool is_under_cursor)
{
    static const std::vector<Face> model = GenChecker();

    static const Color color_table[2][2] = {
        // not under cursor
        {Color(80, 80, 80), Color(230, 230, 230)},
        // under cursor
        {Color(100, 100, 100), Color(250, 250, 250)},
    };

    static const float ambient_light_intensity = 0.3f;

    DefaultShaders::SmoothLight shader;
    shader.ambient_light_intensity = ambient_light_intensity;
    shader.color = color_table[int(is_under_cursor)][int(is_white)];
    shader.SetLightDirection(light_direction_);
    renderer.SetShader(shader);

    renderer.Matrices.SetMode(MatrixType::MODEL);

    const Mat4f model_mat = renderer.Matrices.GetModel();
    const Mat4f norm_corr_mat = Inverse(Transpose(model_mat));
    shader.SetNormCorrection(norm_corr_mat);

    renderer.Matrices.Push();
    renderer.Matrices.Transform(Transform::Translate((i + 0.5f) * 1.0f, (j + 0.5f) * 1.0f, 0.0f));

    DrawModel(renderer, model);
    renderer.Matrices.Pop();
}

std::optional<Vec2i> Demo::GetCellUnderPointer(int x, int y) const
{
    if (x < 0 || x >= board_stencil_.width)
        return std::nullopt;

    if (y < 0 || y >= board_stencil_.height)
        return std::nullopt;

    y = board_stencil_.height - 1 - y;

    const Color color = board_stencil_.At(x, y);
    const uint8_t value = color.r;

    if (value == 255)
        return std::nullopt;

    const int i = value / 8;
    const int j = value % 8;

    return Vec2i{j, i};
}

bool Demo::IsCellUnderCursor(int i, int j) const
{
    if (cell_under_cursor_ != std::nullopt)
        return (i == cell_under_cursor_->y && j == cell_under_cursor_->x);
    return false;
}

} // namespace

int main()
{
    Demo demo;

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
