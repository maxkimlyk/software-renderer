#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "canvas.h"
#include "rasterizer.h"
#include "transforms.h"

class Renderer
{
    Mat4f transformMatrix;

    template<size_t n>
    inline bool IsInsideIdentityBox(Vec<n, float> p)
    {
        for (size_t i = 0; i < 3; ++i)
            if (p[i] < -1.0f || p[i] > 1.0f)
                return false;
        return true;
    }

    bool ProjectVertex(Vec3f vertex, Vec3f &screen)
    {
        Vec4f tmp = projViewMatrix * Embed<4, float>(vertex);
        Vec4f ndc = tmp / tmp.w;

        Vec4f screen4 = viewportMatrix * ndc;
        screen = Project<3, float>(screen4);

        if (!IsInsideIdentityBox(ndc))
            return false;

        return true;
    }

    Mat4f projViewMatrix;

public:
    Image *canvas;

    Mat4f viewportMatrix;
    Mat4f projectionMatrix;
    Mat4f viewMatrix;

    Renderer(Image *canvas)
    {
        this->canvas = canvas;

        viewportMatrix = Projection::Viewport(0, canvas->width, 0, canvas->height, 0, 255);
        projectionMatrix = Projection::Perspective(45.0f, (float)(canvas->width) / (float)(canvas->height), 0.01f, 100.0f);
        viewMatrix = Mat4f::Identity();
        UpdateMatrices();
    }

    void UpdateMatrices()
    {
        projViewMatrix = projectionMatrix * viewMatrix;
    }

    size_t Width() { return canvas->width; }
    size_t Height() { return canvas->height; }

    // inline Vec3f Transform(Vec3f &p)
    // {
    //     // Vec4f ext = Embed<3, float>(p, 1);
    //     // Vec4f tr = transformMatrix * ext;
    //     // float wInv = 1.0f / tr.w;
    //     // return Vec3f {tr.x * wInv, tr.y * wInv, tr.z * wInv};
    // }

    void Clear(Color color = Color(0))
    {
        canvas->Clear(color);
    }

    void SetPixel(uint32_t x, uint32_t y, Color color)
    {
        canvas->SetPixel(x, y, color);
    }

    void DrawRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
    {
        Rasterizer::Rectangle(*canvas, x1, y1, x2, y2, color);
    }

    void DrawSolidRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
    {
        Rasterizer::SolidRect(*canvas, x1, y1, x2, y2, color);
    }

    void Line(Vec2i p1, Vec2i p2, Color color)
    {
        Rasterizer::Line(*canvas, p1, p2, color);
    }

    void Triangle(Vec2i p1, Vec2i p2, Vec2i p3, Color color)
    {
        Rasterizer::Triangle(*canvas, p1, p2, p3, color);
    }

    void TriangleMesh(Vec3f p1, Vec3f p2, Vec3f p3, Color color)
    {
        Vec3f screen1, screen2, screen3;
        bool visible1 = ProjectVertex(p1, screen1);
        bool visible2 = ProjectVertex(p2, screen2);
        bool visible3 = ProjectVertex(p3, screen3);

        if (!visible1 && !visible2 && !visible3)
        {
            // std::cout << "Triangle " << v1 << ", " << v2 << ", " << v3 << " was clipped.\n";
            return;
        }

        Vec2i s1 = Vec2i( Project<2, float>(screen1) );
        Vec2i s2 = Vec2i( Project<2, float>(screen2) );
        Vec2i s3 = Vec2i( Project<2, float>(screen3) );
        Rasterizer::Line(*canvas, s1, s2, color);
        Rasterizer::Line(*canvas, s2, s3, color);
        Rasterizer::Line(*canvas, s3, s1, color);
    }

    void Triangle(Vec3f p1, Vec3f p2, Vec3f p3, Color color)
    {
        // Vec3f v1 = Transform(p1);
        // Vec3f v2 = Transform(p2);
        // Vec3f v3 = Transform(p3);

        // Mat4f cameraMat = camera.proj * camera.view;

        // Vec3f v1 = cameraMat * Embed<4, float>(p1);
        // Vec3f v2 = cameraMat * Embed<4, float>(p2);
        // Vec3f v3 = cameraMat * Embed<4, float>(p3);

        // if (!IsInsideIdentityBox(v1) || !IsInsideIdentityBox(v2) | !IsInsideIdentityBox(v3))
        // {
        //     std::cout << "Trienalge " << v1 << ", " << v2 << ", " << v3 << " was clipped.\n";
        //     return;
        // }

        // v1 = viewport.mat * v1;
        // v2 = viewport.mat * v2;
        // v3 = viewport.mat * v3;

        // Rasterizer::Triangle(*canvas, v1, v2, v3, color);
    }
};

#endif
