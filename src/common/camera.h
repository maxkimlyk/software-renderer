#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "geometry.h"

class Camera
{
    friend class Renderer;

    Mat4f view;
    Mat4f proj;

public:
    Camera(float disposal = 1.0f)
    {
        view = Mat<4, float>::Identity();
        proj = Mat<4, float>::Identity();
        Frustum(-1, 1, -1, 1, 1.0f, 100.0f);
    }

    void Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar)
    {
        float ymax = znear * std::tan(fovyInDegrees * M_PI / 360.0);
        float xmax = ymax * aspectRatio;
        Frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
    }

    void Frustum(float left, float right, float bottom, float top, float znear, float zfar)
    {
        proj = Mat4f::Identity();
        proj[0][0] = 2.0f * znear / (right - left);
        proj[0][2] = (right + left) / (right - left);
        proj[1][1] = 2.0f * znear / (top - bottom);
        proj[1][2] = (top + bottom) / (top - bottom);
        proj[2][2] = - (zfar + znear) / (zfar - znear);
        proj[2][3] = - 2.0f * zfar * znear / (zfar - znear);
        proj[3][2] = -1.0f;
        proj[3][3] = 0.0f;
    }

    void LookAt(Vec3f eye, Vec3f center, Vec3f up)
    {
        Vec3f e3 = Normalize(center - eye);
        Vec3f e1 = Normalize(Cross(up, e3));
        Vec3f e2 = Normalize(Cross(e3, e1));

        Mat4f newBasis = Mat<4, float>::Identity();
        newBasis[0][0] = e1.x; newBasis[0][1] = e1.y; newBasis[0][2] = e1.z;
        newBasis[1][0] = e2.x; newBasis[1][1] = e2.y; newBasis[1][2] = e2.z;
        newBasis[2][0] = e3.x; newBasis[2][1] = e3.y; newBasis[2][2] = e3.z;

        Mat4f translation = Mat<4, float>::Identity();
        translation[0][3] = - center.x;
        translation[1][3] = - center.y;
        translation[2][3] = - center.z;

        view = newBasis * translation;
    }
};

class Viewport
{
    friend class Renderer;

    Mat4f mat;

public:
    Viewport()
    {
        mat = Mat<4, float>::Identity();
    }

    Viewport(float x0, float width, float y0, float height, float z0 = 0, float depth = 255)
    {
        Set(x0, width, y0, height, z0, depth);
    }

    void Set(float x0, float width, float y0, float height, float z0 = 0, float depth = 255)
    {
        mat = Mat<4, float>::Identity();
        mat[0][0] = width / 2;
        mat[1][1] = height / 2;
        mat[2][2] = depth / 2;
        mat[0][3] = x0 + width / 2;
        mat[1][3] = y0 + height / 2;
        mat[2][3] = z0 + depth / 2;
    }
};

#endif
