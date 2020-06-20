#include "canvas.h"

namespace sr
{

int LoadTGA(const char* path, Image& result)
{
    TGA::Canvas tga;

    bool status = tga.read_tga_file(path);
    if (!status)
        return -1;

    tga.flip_vertically();

    const size_t width = (size_t)(tga.get_width());
    const size_t height = (size_t)(tga.get_height());

    result.Resize(width, height);

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

void DumpTga(const char* path, const Image& image)
{
    const bool use_rle = false;

    TGA::TGAImage tga(image.width, image.height, TGA::TGAImage::RGB);

    for (size_t y = 0; y < image.height; ++y)
    {
        for (size_t x = 0; x < image.width; ++x)
        {
            const Color color = Color(image.At(x, y));
            tga.set(x, y, TGA::TGAColor(color.r, color.g, color.b));
        }
    }

    tga.flip_vertically();
    tga.write_tga_file(path, use_rle);
}

} // namespace sr
