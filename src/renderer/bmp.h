#ifndef _BMP_H_
#define _BMP_H_

#include <fstream>
#include "canvas.h"

class Bmp
{
    std::fstream stream;

    template <class T>
    void WritePixel(T pixel)
    {
        stream.write((const char*)(&pixel), sizeof(pixel));
    }

    template <>
    void WritePixel<uint8_t>(uint8_t pixel)
    {
        // staying in RBG-like format even if we have grayscale image
        stream.write((const char*)(&pixel), sizeof(pixel));
        stream.write((const char*)(&pixel), sizeof(pixel));
        stream.write((const char*)(&pixel), sizeof(pixel));
    }

    template <class T>
    uint16_t BppFor()
    {
        // using unsupported type
        return 0;
    }

    template<>
    uint16_t BppFor<uint8_t>()
    {
        return 24;
    }

    template<>
    uint16_t BppFor<uint32_t>()
    {
        return 32;
    }

public:
    Bmp(const char *file):
        stream(file, std::ios::binary | std::ios::out)
    {}

    bool IsOpen() const
    {
        return stream.is_open();
    }

    template<class T>
    int WriteFromCanvas(Canvas<T> &canvas)
    {
        if (!stream)
            return -1;

        unsigned char signature[2] = {'B', 'M'};
        unsigned int fileSize = 14 + 40 + canvas.width * canvas.height * 3;
        unsigned int reserved = 0;
        unsigned int offset = 14 + 40;;

        unsigned int headerSize = 40;
        unsigned int dimensions[2] = {canvas.width, canvas.height};
        unsigned short colorPlanes = 1;
        unsigned short bpp = BppFor<T>();
        unsigned int compression = 0;
        // unsigned int imgSize = canvas.width * canvas.height * sizeof(T);
        unsigned int imgSize = 0;
        unsigned int resolution[2] = { 2795, 2795 };
        unsigned int pltColors = 0;
        unsigned int impColors = 0;

        stream.write((const char*)(signature), sizeof(signature));
        stream.write((const char*)(&fileSize), sizeof(fileSize));
        stream.write((const char*)(&reserved), sizeof(reserved));
        stream.write((const char*)(&offset), sizeof(offset));

        stream.write((const char*)(&headerSize), sizeof(headerSize));
        stream.write((const char*)(dimensions), sizeof(dimensions));
        stream.write((const char*)(&colorPlanes), sizeof(colorPlanes));
        stream.write((const char*)(&bpp), sizeof(bpp));
        stream.write((const char*)(&compression), sizeof(compression));
        stream.write((const char*)(&imgSize), sizeof(imgSize));
        stream.write((const char*)(resolution), sizeof(resolution));
        stream.write((const char*)(&pltColors), sizeof(pltColors));
        stream.write((const char*)(&impColors), sizeof(impColors));

        for(size_t i = 0; i < dimensions[0]; ++i)
            for(size_t j = 0; j < dimensions[1]; ++j)
            {
                T pixel = canvas.At(i, j);
                WritePixel(pixel);
            }

        return 0;
    }
};

#endif
