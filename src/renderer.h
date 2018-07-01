#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "canvas.h"

class Renderer
{
    Canvas *canvas;

public:
    void AssignCanvas(Canvas *canvas)
    {
        this->canvas = canvas;
    }

    void SetPixel(int32_t x, int32_t y, Color color)
    {
        canvas->SetPixel(x, y, color);
    }
};

#endif
