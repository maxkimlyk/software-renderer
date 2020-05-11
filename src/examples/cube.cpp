#include "../common/program.h"
#include "../renderer/camera.h"
#include "../renderer/model.h"

#include <cmath>

using namespace sr;

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Scene";
const std::string TEXTURE_PATH = "box.tga";

float angle = 0.0f;

Camera camera;
Image texture;

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

    // result.push_back({Vertex(br, norm, Vec2f{1.0f, 0.0f}),
    //                   Vertex(bl, norm, Vec2f{0.0f, 0.0f}),
    //                   Vertex(tl, norm, Vec2f{0.0f, 1.0f})});
    // result.push_back({Vertex(br, norm, Vec2f{1.0f, 0.0f}),
    //                   Vertex(tl, norm, Vec2f{0.0f, 1.0f}),
    //                   Vertex(tr, norm, Vec2f{1.0f, 1.0f})});
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

    // clang-format off
    std::vector<Face> cube;
    cube.reserve(12);
    AddQuad(cube, c, d, a, b, Vec3f{0.0f, -1.0f, 0.0f});
    AddQuad(cube, c, b, f, g, Vec3f{-1.0f, 0.0f, 0.0f});
    AddQuad(cube, b, a, e, f, Vec3f{0.0f, 0.0f, 1.0f});
    AddQuad(cube, a, d, h, e, Vec3f{1.0f, 0.0f, 0.0f});
    AddQuad(cube, d, c, g, h, Vec3f{0.0f, 0.0f, -1.0f});
    AddQuad(cube, f, e, h, g, Vec3f{0.0f, 1.0f, 0.0f});
    //clang-format on

    return cube;
}

std::vector<Face> Cube = GenCube(1.0f);

void Init(Renderer& renderer)
{
    camera.LookAt(Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{0.0f, 1.0f, 3.0f});
}

void Process(Renderer& renderer, Input& input)
{
    angle += M_PI / 200.0f;
    Mat4f model_mat = Transform::RotateY(angle);
    renderer.SetModelMatrix(model_mat);
    renderer.SetViewMatrix(camera.ViewMatrix());
}

void Draw(Renderer& renderer)
{
    // DefaultShaders::TextureShader texture_shader(texture);
    // DefaultShaders::FlatShader shader;
    // shader.corr_matrix = renderer.GetModelViewMatrix();
    // shader.light_direction = Normalize(Vec3f{-1.0f, -1.0f, -1.0f});
    // renderer.SetShader(shader);

    DefaultShaders::FlatTexture shader(texture);
    renderer.SetShader(shader);

    renderer.Clear();
    for (const auto& face : Cube)
    {
        // renderer.Triangle(face.v[0].coord, face.v[1].coord, face.v[2].coord, Color(255, 255, 255));
        renderer.Triangle(face.v[0], face.v[1], face.v[2]);
    }
}

int main()
{
    int status = LoadTGA(TEXTURE_PATH.c_str(), texture);
    if (status != 0) {
      ERROR("Could not load texture");
      return -1;
    }

    Program program(Init, Process, Draw);
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
