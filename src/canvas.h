#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "utils.h"

union Color
{
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
    uint32_t value;

    Color() {}

    Color(uint8_t r, uint8_t g, uint8_t b)
    {
        this->b = b;
        this->g = g;
        this->r = r;
        this->a = 0xff;
    }

    Color(uint32_t value)
    {
        this->value = value;
    }

    operator uint32_t()
    {
        return value;
    }
};

class Canvas
{
    uint32_t *ptr;

public:
    size_t width;
    size_t height;

    Canvas(size_t width, size_t height)
    {
        this->width = width;
        this->height = height;
        ptr = new uint32_t [width * height];
    }

    ~Canvas()
    {
        delete [] ptr;
    }

    void Fill(Color color)
    {
        for (size_t i = 0; i < width * height; ++i)
            ptr[i] = uint32_t(color);
    }

    void SetPixel(int32_t x, int32_t y, Color color)
    {
        ptr[x + y * height] = uint32_t(color);
    }

    void Clear()
    {
        Fill(Color(0));
    }

    void Randomize()
    {
        for (size_t i = 0; i < width * height; ++i)
        {
            Color color(rand() % 0xff, rand() % 0xff, rand() % 0xff);
            ptr[i] = uint32_t(color);
        }
    }

    void CopyTo(uint32_t *dstptr, size_t size)
    {
        if (!dstptr)
        {
            WARNING("Canvas::CopyTo gets null dstptr\n");
            return;
        }

        if (size < sizeof(uint32_t) * width * height)
        {
            WARNING("Canvas::CopyTo gets invalid size\n");
            return;
        }

        memcpy(dstptr, ptr, sizeof(uint32_t) * width * height);
    }
};

#endif
