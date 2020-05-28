#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../common/canvas.h"
#include "clipping.h"
#include "rasterizer.h"
#include "transforms.h"
// #include "bmp.h"
#include "shader.h"

namespace sr
{

class Renderer
{
  public:
    Renderer(Image& frame);

    int SnapshotZBuffer(const char* file);

    size_t Width();
    size_t Height();

    void Clear(Color color = Color(0));

    void SetPixel(uint32_t x, uint32_t y, Color color);
    void DrawRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color);
    void DrawSolidRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color);
    void Line(Vec2i p1, Vec2i p2, Color color);
    // void Triangle(Vec2i p1, Vec2i p2, Vec2i p3, Color color);
    void TriangleFrame(Vec3f p1, Vec3f p2, Vec3f p3, Color color);
    void Triangle(Vec3f p1, Vec3f p2, Vec3f p3, Color color);
    void Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);

    void SetModelMatrix(const Mat4f& mat);
    void SetViewMatrix(const Mat4f& mat);
    void SetProjMatrix(const Mat4f& mat);
    const Mat4f& GetModelViewMatrix() const;

    void SetShader(Shader& shader);

  private:
    Mat4f model_matrix_;
    Mat4f view_matrix_;
    Mat4f model_view_matrix_;
    Mat4f projection_matrix_;
    Mat4f view_proj_matrix_;
    Mat4f viewport_matrix_;
    Boxf viewport_box_;

    Canvas<float> zbuffer_;
    Image& frame_;

    DefaultShaders::FlatLight default_shader_;
    Shader* shader_;

    Vec4f ProjectVertex(Vec3f vertex);
    void SetViewport(float x0, float width, float y0, float height, float z0, float depth);
    void UpdateMatrices();
};

} // namespace sr

#endif
