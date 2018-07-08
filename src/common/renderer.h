#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "canvas.h"
#include "geometry.h"

class Renderer
{
    friend class Window;

public:
    Canvas *canvas;

    Renderer(Canvas *canvas)
    {
        this->canvas = canvas;
    }

    size_t Width() { return canvas->width; }
    size_t Height() { return canvas->height; }

    void DrawRect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Color color)
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

    void DrawSolidRect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Color color)
    {
        int xinc = x2 > x1 ? 1 : -1;
        int yinc = y2 > y1 ? 1 : -1;
        for(int y = y1; y != y2; y += yinc)
            for (int x = x1; x != x2; x += xinc)
            {
                canvas->SetPixel(x, y, color);
            }
    }

    void SetPixel(uint32_t x, uint32_t y, Color color)
    {
        canvas->SetPixel(x, y, color);
    }

    void Clear(Color color = Color(0))
    {
        canvas->Clear(color);
    }
};

#endif
