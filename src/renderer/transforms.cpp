#include "transforms.h"

namespace sr
{

namespace Transform
{
Mat4f RotateX(float angle)
{
    return Mat4f{{1, 0, 0, 0},
                 {0, cosf(angle), -sinf(angle), 0},
                 {0, sinf(angle), cosf(angle), 0},
                 {0, 0, 0, 1}};
}

Mat4f RotateY(float angle)
{
    return Mat4f{{cosf(angle), 0, sinf(angle), 0},
                 {0, 1, 0, 0},
                 {-sinf(angle), 0, cosf(angle), 0},
                 {0, 0, 0, 1}};
}

Mat4f RotateZ(float angle)
{
    return Mat4f{{cosf(angle), -sinf(angle), 0, 0},
                 {sinf(angle), cosf(angle), 0, 0},
                 {0, 0, 1, 0},
                 {0, 0, 0, 1}};
}

Mat4f NewBasis(const Vec3f& newx, const Vec3f& newy, const Vec3f& newz)
{
    Mat4f mat = Mat4f::Identity();
    mat[0][0] = newx.x;
    mat[0][1] = newx.y;
    mat[0][2] = newx.z;
    mat[1][0] = newy.x;
    mat[1][1] = newy.y;
    mat[1][2] = newy.z;
    mat[2][0] = newz.x;
    mat[2][1] = newz.y;
    mat[2][2] = newz.z;
    return mat;
}

Mat4f NewBasis(const Vec3f& newx, const Vec3f& newy, const Vec3f& newz, const Vec3f& origin)
{
    Mat4f translation = Mat4f::Identity();
    translation[0][3] = -origin.x;
    translation[1][3] = -origin.y;
    translation[2][3] = -origin.z;
    return NewBasis(newx, newy, newz) * translation;
}

Mat4f Rotate(float angle, const Vec3f& axis)
{
    if (axis.x == 0.0f && axis.y == 0.0f) // rotation around z axis
        return RotateZ(angle);

    float norm = axis.Norm();
    Vec3f naxis = axis;
    if (norm != 1.0f && norm != 0.0f)
        naxis = axis / norm;

    Vec3f ortogonal = Normalize(
        Vec3f{naxis.y, -naxis.x, 0}); // orthogonal * naxis == 0 && ortogonal != Vec3f {0, 0, 0}
    Vec3f third = Normalize(Cross(naxis, ortogonal));

    Mat4f toNewBasis = NewBasis(naxis, ortogonal, third);
    Mat4f rotate = RotateX(angle);
    Mat4f toOldBasis = Transpose(toNewBasis); // toNewBasis is orthogonal matrix

    return toOldBasis * rotate * toNewBasis;
}

Mat4f Scale(float x, float y, float z)
{
    return Mat4f{{x, 0, 0, 0}, {0, y, 0, 0}, {0, 0, z, 0}, {0, 0, 0, 1}};
}

Mat4f Translate(float x, float y, float z)
{
    return Mat4f{{1, 0, 0, x}, {0, 1, 0, y}, {0, 0, 1, z}, {0, 0, 0, 1}};
}

Mat4f LookAt(const Vec3f& eye, const Vec3f& center, const Vec3f& up)
{
    const Vec3f e3 = Normalize(center - eye);
    const Vec3f e1 = Normalize(Cross(up, e3));
    const Vec3f e2 = Normalize(Cross(e3, e1));
    return NewBasis(e1, e2, e3, center);
}
}; // namespace Transform

namespace Projection
{
Mat4f Frustum(float left, float right, float bottom, float top, float znear, float zfar)
{
    Mat4f proj = Mat4f::Identity();
    proj[0][0] = 2.0f * znear / (right - left);
    proj[0][2] = (right + left) / (right - left);
    proj[1][1] = 2.0f * znear / (top - bottom);
    proj[1][2] = (top + bottom) / (top - bottom);
    proj[2][2] = (-zfar - znear) / (zfar - znear);
    proj[2][3] = -2.0f * zfar * znear / (zfar - znear);
    proj[3][2] = -1.0f;
    proj[3][3] = 0.0f;
    return proj;
}

Mat4f Ortho(float left, float right, float bottom, float top, float znear, float zfar)
{
    Mat4f proj = Mat4f::Identity();
    proj[0][0] = 2.0f / (right - left);
    proj[0][3] = (-right - left) / (right - left);
    proj[1][1] = 2.0f / (top - bottom);
    proj[1][3] = (-top - bottom) / (top - bottom);
    proj[2][2] = -2.0f / (zfar - znear);
    proj[2][3] = (-zfar - znear) / (zfar - znear);
    return proj;
}

Mat4f Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar)
{
    float ymax = znear * std::tan(fovyInDegrees * M_PI / 180.0f);
    float xmax = ymax * aspectRatio;
    return Frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

Mat4f Viewport(float x0, float width, float y0, float height, float z0, float depth)
{
    Mat4f mat = Mat4f::Identity();
    mat[0][0] = width / 2;
    mat[1][1] = height / 2;
    mat[2][2] = depth / 2;
    mat[0][3] = x0 + width / 2;
    mat[1][3] = y0 + height / 2;
    mat[2][3] = z0 + depth / 2;
    return mat;
}
}; // namespace Projection

} // namespace sr
