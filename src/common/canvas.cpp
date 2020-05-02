#include "canvas.h"

namespace sr
{

int LoadTGA(const char* path, Image& result)
{
    TGA::Canvas tga;

    bool status = tga.read_tga_file(path);
    if (!status)
        return -1;

    size_t width = (size_t)(tga.get_width());
    size_t height = (size_t)(tga.get_height());
    unsigned char* buffer = tga.buffer();

    result.Reserve(width, height);

    size_t i = 0;
    for (size_t y = 0; y < height; ++y)
        for (size_t x = 0; x < width; ++x)
        {
            Color color;
            color.r = buffer[i++];
            color.g = buffer[i++];
            color.b = buffer[i++];
            result.At(x, y) = color;
        }

    return 0;
}

} // namespace sr
