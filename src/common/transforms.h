#ifndef _TRANSFORMS_H_
#define _TRANSFORMS_H_

#include "geometry.h"

namespace Transform
{
    Mat4f RotateX(float angle)
    {
        return Mat4f {
            {1,  0,              0,              0},
            {0,  cosf(angle),    -sinf(angle),   0},
            {0,  sinf(angle),    cosf(angle),    0},
            {0,  0,              0,              1}
        };
    }

    Mat4f RotateY(float angle)
    {
        return Mat4f {
            {cosf(angle),    0,  -sinf(angle),   0},
            {0,              1,  0,              0},
            {sinf(angle),    0,  cosf(angle),    0},
            {0,              0,  0,              1}
        };
    }

    Mat4f RotateZ(float angle)
    {
        return Mat4f {
            {cosf(angle), -sinf(angle), 0, 0},
            {sinf(angle), cosf(angle),  0, 0},
            {0,           0,            1, 0},
            {0,           0,            0, 1}
        };
    }

    // TODO: rotate around specific axis

    Mat4f Translate(float x, float y, float z)
    {
        return Mat4f {
            {1, 0, 0, x},
            {0, 1, 0, y},
            {0, 0, 1, z},
            {0, 0, 0, 1}
        };
    }

    Mat4f LookAt(Vec3f eye, Vec3f center, Vec3f up)
    {
        Vec3f e3 = Normalize(center - eye);
        Vec3f e1 = Normalize(Cross(up, e3));
        Vec3f e2 = Normalize(Cross(e3, e1));

        Mat4f newBasis = Mat4f::Identity();
        newBasis[0][0] = e1.x; newBasis[0][1] = e1.y; newBasis[0][2] = e1.z;
        newBasis[1][0] = e2.x; newBasis[1][1] = e2.y; newBasis[1][2] = e2.z;
        newBasis[2][0] = e3.x; newBasis[2][1] = e3.y; newBasis[2][2] = e3.z;

        Mat4f translation = Mat4f::Identity();
        translation[0][3] = - center.x;
        translation[1][3] = - center.y;
        translation[2][3] = - center.z;

        return newBasis * translation;
    }
};

namespace Projection
{
    Mat4f Frustum(float left, float right, float bottom, float top, float znear, float zfar)
    {
        Mat4f proj = Mat4f::Identity();
        proj[0][0] = 2.0f * znear / (right - left);
        proj[0][2] = (right + left) / (right - left);
        proj[1][1] = 2.0f * znear / (top - bottom);
        proj[1][2] = (top + bottom) / (top - bottom);
        proj[2][2] = - (zfar + znear) / (zfar - znear);
        proj[2][3] = - 2.0f * zfar * znear / (zfar - znear);
        proj[3][2] = -1.0f;
        proj[3][3] = 0.0f;
        return proj;
    }

    Mat4f Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar)
    {
        float ymax = znear * std::tan(fovyInDegrees * M_PI / 360.0);
        float xmax = ymax * aspectRatio;
        return Frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
    }

    Mat4f Viewport(float x0, float width, float y0, float height, float z0 = 0, float depth = 255)
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
};

#endif
