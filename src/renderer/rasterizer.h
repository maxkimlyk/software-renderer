#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "../common/canvas.h"
#include "geometry.h"
#include "shader.h"
#include "vertex.h"

namespace sr
{
void RasterizeRectangle(Image& canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color);
void RasterizeSolidRect(Image& canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color);
void RasterizeLine(Image& canvas, Vec2i p1, Vec2i p2, Color color);
void RasterizeTriangle(Image& canvas, Canvas<float>& z_buffer, float farZ, Vec4f screen1,
                       Vec4f screen2, Vec4f screen3, const Vertex& v1, const Vertex& v2,
                       const Vertex& v3, Shader& shader);
void RasterizeTriangle(Image& canvas, Vec2f p1, Vec2f p2, Vec2f p3, Color color);
} // namespace sr

#endif
