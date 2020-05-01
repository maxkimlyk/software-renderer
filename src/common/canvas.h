#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <string.h>

#include "../external/tgaimage/tgaimage.h"
#include "logging.h"

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

    Color()
    {}

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

    operator uint32_t() const
    {
        return value;
    }
};

template <class T> class Canvas
{
    T* ptr;

  public:
    size_t width;
    size_t height;

    Canvas() : width(0), height(0), ptr(nullptr)
    {}

    Canvas(size_t width, size_t height) : width(width), height(height), ptr(new T[width * height])
    {}

    ~Canvas()
    {
        if (ptr != nullptr)
            delete[] ptr;
    }

    T& At(size_t x, size_t y)
    {
        return ptr[x + (height - 1 - y) * width];
    }

    T& AtSafe(size_t x, size_t y)
    {
        if ((uint32_t)(x) < width && (uint32_t)(y) < height)
            return At(x, y);

        static T stub = 0;
        return stub;
    }

    void Fill(T value)
    {
        for (size_t i = 0; i < width * height; ++i)
            ptr[i] = value;
    }

    void FillBlack()
    {
        memset(ptr, 0, sizeof(T) * width * height);
    }

    void Clear(T value)
    {
        if (value == T(0))
            FillBlack();
        else
            Fill(value);
    }

    void SetPixel(int32_t x, int32_t y, T value)
    {
        if ((uint32_t)(x) < width && (uint32_t)(y) < height)
            At(x, y) = value;
    }

    void CopyTo(uint32_t* dstptr, size_t size) const
    {
        memcpy(dstptr, ptr, sizeof(T) * width * height);
    }

    void CopyWithStride(uint32_t* dstptr, size_t stride) const
    {
        for (size_t i = 0; i < height; ++i)
        {
            memcpy(dstptr + i * stride, ptr + i * width, sizeof(T) * width);
        }
    }

    void Reserve(size_t width, size_t height)
    {
        if (ptr != nullptr)
            delete[] ptr;

        ptr = new T[width * height];
    }
};

typedef Canvas<uint32_t> Image;

int LoadTGA(const char* path, Image& result);

#endif
