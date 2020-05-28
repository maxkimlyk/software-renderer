#include "rasterizer.h"
#include "shader.h"

#include <tuple>

namespace sr
{

namespace
{

template <class T>
std::pair<T, T> MinMax(T v1, T v2, T v3)
{
    T min, max;

    if (v1 < v2)
    {
        min = v1;
        max = v2;
    }
    else
    {
        min = v2;
        max = v1;
    }

    if (v3 < min)
        min = v3;
    else if (v3 > max)
        max = v3;

    return std::make_pair(min, max);
}

int Round(float val)
{
    return (int)(val + 0.5f);
}

template <class T>
std::pair<T, T> MinMax(T v1, T v2)
{
    if (v1 <= v2)
        return std::make_pair(v1, v2);
    return std::make_pair(v2, v1);
}

template <size_t n, class T>
static Rect<T> BoundingBox(Vec<n, T> p1, Vec<n, T> p2, Vec<n, T> p3)
{
    Rect<T> rect;
    std::tie(rect.left, rect.right) = MinMax(p1[0], p2[0], p3[0]);
    std::tie(rect.bottom, rect.top) = MinMax(p1[1], p2[1], p3[1]);
    return rect;
}

Vec3f DoBarPerspectiveCorrection(const Vec3f& bar, const Vec3f& corr)
{
    const Vec3f corrected_bar = Vec3f{bar[0] * corr[0], bar[1] * corr[1], bar[2] * corr[2]};
    return corrected_bar / corrected_bar.Sum();
}

void PutShaderedPixel(Image& canvas, Canvas<float>& z_buffer, int x, int y, float z, Vec3f bar,
                      Shader& shader)
{
    Color color;

    if (z >= 0 && z < z_buffer.At(x, y) && shader.pixel(bar, color))
    {
        z_buffer.SetPixel(x, y, z);
        canvas.SetPixel(x, y, color);
    }
}

void RasterizeHorizontalDegenerateTriangle(Image& canvas, Canvas<float>& z_buffer, Vec4f screen1,
                                           Vec4f screen2, Vec4f screen3, Vec3f bar_corr, Vertex v1,
                                           Vertex v2, Vertex v3, Shader& shader)
{
    const float width = (float)canvas.width;
    const float height = (float)canvas.height;

    const Vec3f zs = {screen1.z, screen2.z, screen3.z};

    Vec3f bar;
    VecView bar_view(bar);

    const int y = Round(screen1.y);

    int x1 = Round(screen1.x);
    int x2 = Round(screen2.x);
    int x3 = Round(screen3.x);

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(bar_view[0], bar_view[1]);
    }
    if (x2 > x3)
    {
        std::swap(x2, x3);
        std::swap(bar_view[1], bar_view[2]);
    }
    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(bar_view[0], bar_view[1]);
    }

    if (x1 == x3)
    {
        bar_view = Vec3f{1.0f, 0.0f, 0.0f};
        PutShaderedPixel(canvas, z_buffer, x1, y, zs * bar, bar, shader);
        bar_view = Vec3f{0.0f, 1.0f, 0.0f};
        PutShaderedPixel(canvas, z_buffer, x1, y, zs * bar, bar, shader);
        bar_view = Vec3f{0.0f, 0.0f, 1.0f};
        PutShaderedPixel(canvas, z_buffer, x1, y, zs * bar, bar, shader);
        return;
    }

    const float invLength = 1.0f / (x3 - x1);
    const float invRightLength = x3 - x2 != 0 ? 1.0f / (x3 - x2) : 0.0f;
    const float invLeftLength = x2 - x1 != 0 ? 1.0f / (x2 - x1) : 0.0f;

    for (int x = x1; x <= x3; ++x)
    {
        const bool rightSegment = x >= x2;

        const float t = (x - x1) * invLength;
        const float u = rightSegment ? (x - x2) * invRightLength : (x - x1) * invLeftLength;

        // first side
        bar_view = Vec3f{(1.0f - t), 0.0f, t};
        Vec3f corrected_bar = DoBarPerspectiveCorrection(bar, bar_corr);
        PutShaderedPixel(canvas, z_buffer, x, y, zs * corrected_bar, corrected_bar, shader);

        // second side
        if (rightSegment)
            bar_view = Vec3f{0.0f, (1.0f - u), u};
        else
            bar_view = Vec3f{(1.0f - u), u, 0.0f};

        corrected_bar = DoBarPerspectiveCorrection(bar, bar_corr);
        PutShaderedPixel(canvas, z_buffer, x, y, zs * corrected_bar, corrected_bar, shader);
    }
}

} // namespace

void RasterizeRectangle(Image& canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
{
    int inc = x2 > x1 ? 1 : -1;
    for (int i = x1; i != x2; i += inc)
    {
        canvas.SetPixel(i, y1, color);
        canvas.SetPixel(i, y2, color);
    }

    inc = y2 > y1 ? 1 : -1;
    for (int i = y1; i != y2; i += inc)
    {
        canvas.SetPixel(x1, i, color);
        canvas.SetPixel(x2, i, color);
    }
}

void RasterizeSolidRect(Image& canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
{
    int xinc = x2 > x1 ? 1 : -1;
    int yinc = y2 > y1 ? 1 : -1;
    for (int y = y1; y != y2; y += yinc)
        for (int x = x1; x != x2; x += xinc)
        {
            canvas.SetPixel(x, y, color);
        }
}

void RasterizeLine(Image& canvas, Vec2i p1, Vec2i p2, Color color)
{
    bool transformed = false;

    if (abs(p2.x - p1.x) < abs(p2.y - p1.y))
    {
        std::swap(p1.x, p1.y);
        std::swap(p2.x, p2.y);
        transformed = true;
    }

    if (p2.x < p1.x)
        std::swap(p1, p2);

    int y = p1.y;
    int dy = (p2.y > p1.y) ? 1 : -1;
    int err = 0;
    int derr = 2 * abs(p2.y - p1.y);

    for (int x = p1.x; x <= p2.x; x++)
    {
        if (err > (p2.x - p1.x))
        {
            y += dy;
            err -= 2 * (p2.x - p1.x);
        }

        if (transformed)
            canvas.SetPixel(y, x, color);
        else
            canvas.SetPixel(x, y, color);
        err += derr;
    }
}

void PutPixel(Image& canvas, Canvas<float>& z_buffer, int x, int y, float z, Color color)
{
    if (z >= 0 && z < z_buffer.At(x, y))
    {
        z_buffer.SetPixel(x, y, z);
        canvas.SetPixel(x, y, color);
    }
}

void RasterizeTriangle(Image& canvas, Canvas<float>& z_buffer, float far_z, Vec4f screen1,
                       Vec4f screen2, Vec4f screen3, const Vertex& v1, const Vertex& v2,
                       const Vertex& v3, Shader& shader)
{
    const float width = (float)canvas.width;
    const float height = (float)canvas.height;

    const Vec3f zs = {screen1.z, screen2.z, screen3.z};
    const Vec3f bar_corr = {1.0f / screen1.w, 1.0f / screen2.w, 1.0f / screen3.w};

    Vec3f bar;
    VecView bar_view(bar);

    Vec2i i1 = {Round(screen1.x), Round(screen1.y)};
    Vec2i i2 = {Round(screen2.x), Round(screen2.y)};
    Vec2i i3 = {Round(screen3.x), Round(screen3.y)};

    if (i1.y > i2.y)
    {
        std::swap(i1, i2);
        std::swap(bar_view[0], bar_view[1]);
    }
    if (i2.y > i3.y)
    {
        std::swap(i2, i3);
        std::swap(bar_view[1], bar_view[2]);
    }
    if (i1.y > i2.y)
    {
        std::swap(i1, i2);
        std::swap(bar_view[0], bar_view[1]);
    }

    if (i1.y >= height || i3.y < 0)
        return;

    const auto [min_x, max_x] = MinMax(i1.x, i2.x, i3.x);
    if (max_x < 0 || min_x >= width)
        return;

    const auto [min_z, max_z] = MinMax(screen1.z, screen2.z, screen3.z);
    if (min_z < 0 || max_z >= far_z)
        return;

    if (i1.y == i3.y)
    {
        RasterizeHorizontalDegenerateTriangle(canvas, z_buffer, screen1, screen2, screen3, bar_corr,
                                              v1, v2, v3, shader);
        return;
    }

    const int start_y = std::max(0, i1.y);
    const int end_y = std::min(static_cast<int>(height - 1), i3.y);

    const float triangle_height = (float)(i3.y - i1.y);

    for (int y = start_y; y <= end_y; ++y)
    {
        const bool upper_segment = y >= i2.y;
        const float t = (y - i1.y) / triangle_height;
        const float u =
            upper_segment ? (y - i2.y) / (float)(i3.y - i2.y) : (y - i1.y) / (float)(i2.y - i1.y);

        const int x1 = (int)(i1.x + t * (i3.x - i1.x));
        const int x2 = (int)(upper_segment ? i2.x + u * (i3.x - i2.x) : i1.x + u * (i2.x - i1.x));

        auto [start_x, end_x] = MinMax(x1, x2);

        if (end_x < 0 || start_x > width - 1)
            continue;

        if (start_x == end_x)
        {
            bar_view = Vec3f{1.0f - t, 0.0f, t};
            const Vec3f corrected_bar = DoBarPerspectiveCorrection(bar, bar_corr);
            PutShaderedPixel(canvas, z_buffer, x1, y, corrected_bar * zs, corrected_bar, shader);
        }
        else
        {
            start_x = start_x < 0 ? 0 : start_x;
            end_x = end_x >= width ? width - 1 : end_x;

            const float inv_x_length = 1.0f / (x2 - x1);
            for (int x = start_x; x <= end_x; ++x)
            {
                const float s = (x - x1) * inv_x_length;

                if (upper_segment)
                {
                    const float b1 = (1.0f - t) * (1.0f - s);
                    const float b2 = (1.0f - u) * s;
                    bar_view = Vec3f{b1, b2, 1.0f - b1 - b2};
                }
                else
                {
                    const float b3 = t * (1.0f - s);
                    const float b2 = u * s;
                    bar_view = Vec3f{1.0f - b2 - b3, b2, b3};
                }

                const Vec3f corrected_bar = DoBarPerspectiveCorrection(bar, bar_corr);
                PutShaderedPixel(canvas, z_buffer, x, y, corrected_bar * zs, corrected_bar, shader);
            }
        }
    }
}

} // namespace sr
