#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "canvas.h"

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

    void SetPixel(uint32_t x, uint32_t y, Color color)
    {
        canvas->SetPixel(x, y, color);
    }
};

#endif
