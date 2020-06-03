#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../common/canvas.h"
#include "clipping.h"
#include "rasterizer.h"
#include "shader.h"
#include "transforms.h"
#include "matrix_stack.h"

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
    void TriangleFrame(Vec3f p1, Vec3f p2, Vec3f p3, Color color);
    void Triangle(Vec3f p1, Vec3f p2, Vec3f p3, Color color);
    void Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);

    void SetShader(Shader& shader);

    MatrixStack Matrices;

  private:
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
