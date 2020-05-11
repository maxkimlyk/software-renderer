#include "canvas.h"

namespace sr
{

int LoadTGA(const char* path, Image& result)
{
    TGA::Canvas tga;

    bool status = tga.read_tga_file(path);
    if (!status)
        return -1;

    const size_t width = (size_t)(tga.get_width());
    const size_t height = (size_t)(tga.get_height());

    result.width = width;
    result.height = height;
    result.Reserve(width, height);

    size_t i = 0;
    for (size_t y = 0; y < height; ++y)
        for (size_t x = 0; x < width; ++x)
        {
            TGA::TGAColor tga_color = tga.get(x, y);

            Color color;
            color.r = tga_color.bgra[2];
            color.g = tga_color.bgra[1];
            color.b = tga_color.bgra[0];
            result.At(x, y) = color;
        }

    return 0;
}

} // namespace sr
