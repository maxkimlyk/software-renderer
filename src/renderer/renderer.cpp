#include "renderer.h"

namespace sr
{

Vec3f Renderer::ProjectVertex(Vec3f vertex)
{
    Vec4f cs = view_matrix_ * Embed<4, float>(vertex);
    Vec4f tmp = projection_matrix_ * cs;

    if (tmp.w == 0.0f)
        tmp.w = 0.0001f;

    float invW = 1.0f / tmp.w;
    float invAbsW = cs.z < 0.0f ? invW : -invW;
    Vec4f ndc = Vec4f{tmp.x * invAbsW, tmp.y * invAbsW, tmp.z * invAbsW, 1.0f};

    Vec4f screen4 = viewport_matrix_ * ndc;
    return Project<3, float>(screen4);
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
    : frame_(frame), zbuffer_(frame.width, frame.height), shader(DefaultShaders::flatShader)
{
    SetViewport(0.0, (float)(frame.width), 0.0, (float)(frame.height), 0.0, 255.0);
    projection_matrix_ =
        Projection::Perspective(45.0f, (float)(frame.width) / (float)(frame.height), 0.01f, 10.0f);
    view_matrix_ = Mat4f::Identity();
    UpdateMatrices();
}

// int Renderer::SnapshotZBuffer(const char *file)
// {
//     Bmp bmp(file);
//     return bmp.WriteFromCanvas(zbuffer_);
// }

void Renderer::SetViewMatrix(const Mat4f& mat)
{
    view_matrix_ = mat;
    UpdateMatrices();
}

void Renderer::SetProjMatrix(const Mat4f& mat)
{
    projection_matrix_ = mat;
    UpdateMatrices();
}

void Renderer::UpdateMatrices()
{
    view_proj_matrix_ = projection_matrix_ * view_matrix_;
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

void Renderer::Triangle(Vec2i p1, Vec2i p2, Vec2i p3, Color color)
{
    RasterizeTriangle(frame_, p1, p2, p3, color);
}

void Renderer::TriangleFrame(Vec3f p1, Vec3f p2, Vec3f p3, Color color)
{
    Vec3f screen1 = ProjectVertex(p1);
    Vec3f screen2 = ProjectVertex(p2);
    Vec3f screen3 = ProjectVertex(p3);

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

    Vec3f screen1 = ProjectVertex(p1);
    Vec3f screen2 = ProjectVertex(p2);
    Vec3f screen3 = ProjectVertex(p3);

    RasterizeTriangle(frame_, zbuffer_, viewport_box_.zmax, screen1, screen2, screen3,
                      solidColorShader);
}

void Renderer::Triangle(Vec3f p1, Vec3f p2, Vec3f p3)
{
    shader.vertex(p1, p2, p3);

    Vec3f screen1 = ProjectVertex(p1);
    Vec3f screen2 = ProjectVertex(p2);
    Vec3f screen3 = ProjectVertex(p3);

    RasterizeTriangle(frame_, zbuffer_, viewport_box_.zmax, screen1, screen2, screen3, shader);
}

} // namespace sr
