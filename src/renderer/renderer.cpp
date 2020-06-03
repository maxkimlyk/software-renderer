#include "renderer.h"

namespace sr
{

Vec4f Renderer::ProjectVertex(Vec3f vertex)
{
    Vec4f cs = Matrices.GetFullTransformMatrix() * Embed<4, float>(vertex);

    if (cs.w == 0.0f)
        cs.w = 0.0001f;

    const float invW = 1.0f / cs.w;
    // just for a beauty, use abs w in order to force points behind the camera to
    // preserve negative z in clip coordinates instead of having z > 1 (or > z_far):
    const float invAbsW = std::fabs(invW);
    const Vec4f ndc = Vec4f{cs.x * invAbsW, cs.y * invAbsW, cs.z * invAbsW, 1.0f};

    Vec4f screen4 = viewport_matrix_ * ndc;
    screen4.w = cs.w; // save z value from clip space for perspective correction
    return screen4;
}

void Renderer::SetViewport(float x0, float width, float y0, float height, float z0, float depth)
{
    viewport_matrix_ = Projection::Viewport(x0, width, y0, height, z0, depth);
    viewport_box_.xmin = x0;
    viewport_box_.xmax = x0 + width;
    viewport_box_.ymin = y0;
    viewport_box_.ymax = y0 + height;
    viewport_box_.zmin = z0;
    viewport_box_.zmax = z0 + depth;
}

Renderer::Renderer(Image& frame)
    : frame_(frame), zbuffer_(frame.width, frame.height), shader_(&default_shader_)
{
    SetViewport(0.0, (float)(frame.width), 0.0, (float)(frame.height), 0.0, 255.0);
    Matrices.SetProjection(Projection::Perspective(45.0f, (float)(frame.width) / (float)(frame.height), 0.05f, 100.0f));
    Matrices.SetView(Mat4f::Identity());
    Matrices.SetModel(Mat4f::Identity());
}

size_t Renderer::Width()
{
    return frame_.width;
}
size_t Renderer::Height()
{
    return frame_.height;
}

void Renderer::Clear(Color color)
{
    frame_.Clear(color);
    zbuffer_.Clear(UINT8_MAX);
}

void Renderer::SetPixel(uint32_t x, uint32_t y, Color color)
{
    frame_.SetPixel(x, y, color);
}

void Renderer::DrawRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
{
    RasterizeRectangle(frame_, x1, y1, x2, y2, color);
}

void Renderer::DrawSolidRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
{
    RasterizeSolidRect(frame_, x1, y1, x2, y2, color);
}

void Renderer::Line(Vec2i p1, Vec2i p2, Color color)
{
    RasterizeLine(frame_, p1, p2, color);
}

void Renderer::TriangleFrame(Vec3f p1, Vec3f p2, Vec3f p3, Color color)
{
    const Vec3f screen1 = Project<3, float>(ProjectVertex(p1));
    const Vec3f screen2 = Project<3, float>(ProjectVertex(p2));
    const Vec3f screen3 = Project<3, float>(ProjectVertex(p3));

    Vec3f clipped1, clipped2;
    if (ClipLine(screen1, screen2, viewport_box_, clipped1, clipped2))
        RasterizeLine(frame_, Project<2, float>(clipped1), Project<2, float>(clipped2), color);

    if (ClipLine(screen2, screen3, viewport_box_, clipped1, clipped2))
        RasterizeLine(frame_, Project<2, float>(clipped1), Project<2, float>(clipped2), color);

    if (ClipLine(screen3, screen1, viewport_box_, clipped1, clipped2))
        RasterizeLine(frame_, Project<2, float>(clipped1), Project<2, float>(clipped2), color);
}

void Renderer::Triangle(Vec3f p1, Vec3f p2, Vec3f p3, Color color)
{
    DefaultShaders::SolidColor solidColorShader(color);

    const Vec4f screen1 = ProjectVertex(p1);
    const Vec4f screen2 = ProjectVertex(p2);
    const Vec4f screen3 = ProjectVertex(p3);

    const Vertex v1 = p1;
    const Vertex v2 = p2;
    const Vertex v3 = p3;

    RasterizeTriangle(frame_, zbuffer_, viewport_box_.zmax, screen1, screen2, screen3, v1, v2, v3,
                      solidColorShader);
}

void Renderer::Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
    shader_->vertex(v1, v2, v3);

    const Vec4f s1 = ProjectVertex(v1.coord);
    const Vec4f s2 = ProjectVertex(v2.coord);
    const Vec4f s3 = ProjectVertex(v3.coord);

    RasterizeTriangle(frame_, zbuffer_, viewport_box_.zmax, s1, s2, s3, v1, v2, v3, *shader_);
}

void Renderer::SetShader(Shader& shader)
{
    shader_ = &shader;
}

} // namespace sr
