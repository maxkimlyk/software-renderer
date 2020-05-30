#ifndef _TRANSFORMS_H_
#define _TRANSFORMS_H_

#include "geometry.h"

namespace sr
{

namespace Transform
{
Mat4f RotateX(float angle);
Mat4f RotateY(float angle);
Mat4f RotateZ(float angle);
Mat4f NewBasis(const Vec3f& newx, const Vec3f& newy, const Vec3f& newz);
Mat4f NewBasis(const Vec3f& newx, const Vec3f& newy, const Vec3f& newz, const Vec3f& origin);
Mat4f Rotate(float angle, const Vec3f& axis);
Mat4f Scale(float x, float y, float z);
Mat4f Translate(float x, float y, float z);
Mat4f LookAt(const Vec3f& eye, const Vec3f& center, const Vec3f& up);
}; // namespace Transform

namespace Projection
{
Mat4f Frustum(float left, float right, float bottom, float top, float znear, float zfar);
Mat4f Ortho(float left, float right, float bottom, float top, float znear, float zfar);
Mat4f Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar);
Mat4f Viewport(float x0, float width, float y0, float height, float z0 = 0, float depth = 255);
}; // namespace Projection

} // namespace sr

#endif
