#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "canvas.h"
#include "geometry.h"

class Renderer
{
    friend class Window;

    struct Rect
    {
        int left;
        int right;
        int top;
        int bottom;
    };

    template<class T>
    inline void MinMax(T a, T b, T c, T &min, T &max)
    {
        if (a > b)
            std::swap(a, b);
        if (b > c)
            std::swap(b, c);
        if (a > b)
            std::swap(a, b);
        min = a;
        max = c;
    }

    template <size_t n, class T>
    Rect BoundingBox(Vec<n, T> p1, Vec<n, T> p2, Vec<n, T> p3)
    {
        Rect rect;
        MinMax((int)p1[0], (int)p2[0], (int)p3[0], rect.left, rect.right);
        MinMax((int)p1[1], (int)p2[1], (int)p3[1], rect.top, rect.bottom);
        return rect;
    }

public:
    Canvas *canvas;

    Renderer(Canvas *canvas)
    {
        this->canvas = canvas;
    }

    size_t Width() { return canvas->width; }
    size_t Height() { return canvas->height; }

    void Clear(Color color = Color(0))
    {
        if ((uint32_t)(color) == 0)
            canvas->FillBlack();
        else
            canvas->Clear(color);
    }

    void SetPixel(uint32_t x, uint32_t y, Color color)
    {
        canvas->SetPixel(x, y, color);
    }

    void DrawRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
    {
        int inc = x2 > x1 ? 1 : -1;
        for (int i = x1; i != x2; i += inc)
        {
            canvas->SetPixel(i, y1, color);
            canvas->SetPixel(i, y2, color);
        }

        inc = y2 > y1 ? 1 : -1;
        for (int i = y1; i != y2; i += inc)
        {
            canvas->SetPixel(x1, i, color);
            canvas->SetPixel(x2, i, color);
        }
    }

    void DrawSolidRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
    {
        int xinc = x2 > x1 ? 1 : -1;
        int yinc = y2 > y1 ? 1 : -1;
        for(int y = y1; y != y2; y += yinc)
            for (int x = x1; x != x2; x += xinc)
            {
                canvas->SetPixel(x, y, color);
            }
    }

    void Line(Vec2i p1, Vec2i p2, Color color)
    {
        bool transformed = false;

        if (abs(p2[0] - p1[0]) < abs(p2[1] - p1[1]))
        {
            std::swap(p1[0], p1[1]);
            std::swap(p2[0], p2[1]);
            transformed = true;
        }

        if (p2[0] < p1[0])
            std::swap(p1, p2);

        int y = p1[1];
        int dy = (p2[1] > p1[1]) ? 1 : -1;
        int err = 0;
        int derr = 2 * abs(p2[1] - p1[1]);

        for (int x = p1[0]; x <= p2[0]; x++)
        {
            if (err > (p2[0] - p1[0]))
            {
                y += dy;
                err -= 2 * (p2[0] - p1[0]);
            }

            if (transformed)
                canvas->SetPixel(y, x, color);
            else
                canvas->SetPixel(x, y, color);
            err += derr;
        }
    }

    void Triangle(Vec2i p1, Vec2i p2, Vec2i p3, Color color)
    {
        Vec3f v1 = {(float)(p2[0] - p1[0]), (float)(p3[0] - p1[0]), 1.0f};
        Vec3f v2 = {(float)(p2[1] - p1[1]), (float)(p3[1] - p1[1]), 1.0f};

        Rect rect = BoundingBox(p1, p2, p3);
        for (int y = rect.top; y <= rect.bottom; ++y)
            for (int x = rect.left; x <= rect.right; ++x)
            {
                v1[2] = p1[0] - x;
                v2[2] = p1[1] - y;
                Vec3f tmp = Cross(v1, v2);
                tmp = tmp / tmp[2];
                Vec3f bar = Vec3f {1 - tmp[0] - tmp[1], tmp[0], tmp[1]};

                if (bar[0] >= 0.0f && bar[1] >= 0.0f && bar[2] >= 0.0f)
                    canvas->SetPixel(x, y, color);
            }
    }
};

#endif
