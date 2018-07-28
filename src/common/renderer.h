#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "canvas.h"
#include "rasterizer.h"
#include "transforms.h"
#include "clipping.h"

class Renderer
{
    Mat4f viewProjMatrix;
    Mat4f viewportMatrix;
    Boxf viewportBox;

    Vec3f ProjectVertex(Vec3f vertex)
    {
        Vec4f cs = viewMatrix * Embed<4, float>(vertex);
        Vec4f tmp = projectionMatrix * cs;

        float invW = 1.0f / tmp.w;
        float invAbsW = cs.z < 0.0f ? invW : -invW;
        Vec4f ndc = Vec4f {
            tmp.x * invAbsW,
            tmp.y * invAbsW,
            tmp.z * invW,
            1.0f
        };

        Vec4f screen4 = viewportMatrix * ndc;
        return Project<3, float>(screen4);
    }

    void SetViewport(float x0, float width, float y0, float height, float z0, float depth)
    {
        viewportMatrix = Projection::Viewport(0, canvas->width, 0, canvas->height, 0, 255);
        viewportBox.xmin = x0;
        viewportBox.xmax = x0 + width;
        viewportBox.ymin = y0;
        viewportBox.ymax = y0 + height;
        viewportBox.zmin = z0;
        viewportBox.zmax = z0 + depth;
    }

public:
    Image *canvas;

    Mat4f viewMatrix;
    Mat4f projectionMatrix;

    Renderer(Image *canvas)
    {
        this->canvas = canvas;
        SetViewport(0, canvas->width, 0, canvas->height, 0, 255);
        projectionMatrix = Projection::Perspective(45.0f, (float)(canvas->width) / (float)(canvas->height), 0.01f, 100.0f);
        viewMatrix = Mat4f::Identity();
        UpdateMatrices();
    }

    void UpdateMatrices()
    {
        viewProjMatrix = projectionMatrix * viewMatrix;
    }

    size_t Width() { return canvas->width; }
    size_t Height() { return canvas->height; }

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
        Vec3f screen1 = ProjectVertex(p1);
        Vec3f screen2 = ProjectVertex(p2);
        Vec3f screen3 = ProjectVertex(p3);

        Vec3f clipped1, clipped2;
        if (Clipping::ClipLine(screen1, screen2, viewportBox, clipped1, clipped2))
            Rasterizer::Line(*canvas, Project<2, float>(clipped1), Project<2, float>(clipped2), color);

        if (Clipping::ClipLine(screen2, screen3, viewportBox, clipped1, clipped2))
            Rasterizer::Line(*canvas, Project<2, float>(clipped1), Project<2, float>(clipped2), color);

        if (Clipping::ClipLine(screen3, screen1, viewportBox, clipped1, clipped2))
            Rasterizer::Line(*canvas, Project<2, float>(clipped1), Project<2, float>(clipped2), color);
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
