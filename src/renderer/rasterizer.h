#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "../common/canvas.h"
#include "geometry.h"
#include "shader.h"

namespace sr
{
void RasterizeRectangle(Image& canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color);
void RasterizeSolidRect(Image& canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color);
void RasterizeLine(Image& canvas, Vec2i p1, Vec2i p2, Color color);
void RasterizeTriangle(Image& canvas, Canvas<float>& zBuffer, float farZ, Vec3f p1, Vec3f p2,
                       Vec3f p3, Shader& shader);
void RasterizeTriangle(Image& canvas, Vec2f p1, Vec2f p2, Vec2f p3, Color color);
} // namespace sr

#endif
