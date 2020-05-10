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
    Mat4f view_matrix_;
    Mat4f projection_matrix_;

    Shader* shader_;

    Renderer(Image& frame);

    int SnapshotZBuffer(const char* file);


    size_t Width();
    size_t Height();

    void Clear(Color color = Color(0));

    void SetPixel(uint32_t x, uint32_t y, Color color);
    void DrawRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color);
    void DrawSolidRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color);
    void Line(Vec2i p1, Vec2i p2, Color color);
    void Triangle(Vec2i p1, Vec2i p2, Vec2i p3, Color color);
    void TriangleFrame(Vec3f p1, Vec3f p2, Vec3f p3, Color color);
    void Triangle(Vec3f p1, Vec3f p2, Vec3f p3, Color color);
    void Triangle(Vec3f p1, Vec3f p2, Vec3f p3);
    void Triangle(Vertex v1, Vertex v2, Vertex v3);

    void SetViewMatrix(const Mat4f& mat);
    void SetProjMatrix(const Mat4f& mat);

    void SetShader (Shader& shader);

  private:
    Mat4f view_proj_matrix_;
    Mat4f viewport_matrix_;
    Boxf viewport_box_;

    Canvas<float> zbuffer_;
    Image& frame_;

    Vec3f ProjectVertex(Vec3f vertex);
    void SetViewport(float x0, float width, float y0, float height, float z0, float depth);
    void UpdateMatrices();
};

} // namespace sr

#endif
