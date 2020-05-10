#include "rasterizer.h"
#include "shader.h"

#include <tuple>

namespace sr
{

template <class T>
static void MinMax(T a, T b, T c, T& min, T& max)
{
    if (a > b)
        std::swap(a, b);
    if (b > c)
        std::swap(b, c);
    if (a > b)
        std::swap(a, b);
    min = a;
    max = c;
}

template <size_t n, class T>
static Rect<T> BoundingBox(Vec<n, T> p1, Vec<n, T> p2, Vec<n, T> p3)
{
    Rect<T> rect;
    MinMax(p1[0], p2[0], p3[0], rect.left, rect.right);
    MinMax(p1[1], p2[1], p3[1], rect.bottom, rect.top);
    return rect;
}

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

void PutPixel(Image& canvas, Canvas<float>& zBuffer, int x, int y, float z, Color color)
{
    if (z >= 0 && z < zBuffer.At(x, y))
    {
        zBuffer.SetPixel(x, y, z);
        canvas.SetPixel(x, y, color);
    }
}

void PutShaderedPixel(Image& canvas, Canvas<float>& zBuffer, int x, int y, float z, Vec3f bar,
                      Shader& shader)
{
    Color color;

    if (z >= 0 && z < zBuffer.At(x, y) && shader.pixel(bar, color))
    {
        zBuffer.SetPixel(x, y, z);
        canvas.SetPixel(x, y, color);
    }
}

void RasterizeHorizontalDegenerateTriangle(Image& canvas, Canvas<float>& zBuffer, Vertex v1,
                                           Vertex v2, Vertex v3, Shader& shader)
{
    const Vec3f zs = {v1.coord.z, v2.coord.z, v3.coord.z};

    const float width = (float)canvas.width;
    const float height = (float)canvas.height;

    Vec3f bar;
    float* bar1 = &bar[0];
    float* bar2 = &bar[1];
    float* bar3 = &bar[2];

    if (v1.coord.x > v2.coord.x)
    {
        std::swap(v1, v2);
        std::swap(bar1, bar2);
    }
    if (v2.coord.x > v3.coord.x)
    {
        std::swap(v2, v3);
        std::swap(bar2, bar3);
    }
    if (v1.coord.x > v2.coord.x)
    {
        std::swap(v1, v2);
        std::swap(bar1, bar2);
    }

    float x1 = (float)((int)v1.coord.x);
    float x2 = (float)((int)v2.coord.x);
    float x3 = (float)((int)v3.coord.x);

    int y = (int)v1.coord.y;

    if (x1 == x3)
    {
        int x = (int)v1.coord.x;
        std::tie(*bar1, *bar2, *bar3) = std::make_tuple(1.0f, 0.0f, 0.0f);
        PutShaderedPixel(canvas, zBuffer, x, y, v1.coord.z, bar, shader);
        std::tie(*bar1, *bar2, *bar3) = std::make_tuple(0.0f, 1.0f, 0.0f);
        PutShaderedPixel(canvas, zBuffer, x, y, v2.coord.z, bar, shader);
        std::tie(*bar1, *bar2, *bar3) = std::make_tuple(0.0f, 0.0f, 1.0f);
        PutShaderedPixel(canvas, zBuffer, x, y, v3.coord.z, bar, shader);
        return;
    }

    float invLength = 1.0f / (x3 - x1);
    float invRightLength = x3 - x2 != 0 ? 1.0f / (x3 - x2) : 0.0f;
    float invLeftLength = x2 - x1 != 0 ? 1.0f / (x2 - x1) : 0.0f;

    for (int x = x1; x <= x3; ++x)
    {
        bool rightSegment = x >= x2;

        float t = (x - x1) * invLength;
        float u = rightSegment ? (x - x2) * invRightLength : (x - x1) * invLeftLength;

        // first side
        // Vec3f bar = {(1.0f - t), 0.0f, t};
        std::tie(*bar1, *bar2, *bar3) = std::make_tuple((1.0f - t), 0.0f, t);
        float z = zs * bar;
        PutShaderedPixel(canvas, zBuffer, x, y, z, bar, shader);

        // sedond side
        if (rightSegment)
            // bar = Vec3f{0.0f, (1.0f - u), u};
            std::tie(*bar1, *bar2, *bar3) = std::make_tuple(0.0f, (1.0f - u), u);
        else
            // bar = Vec3f{(1.0f - u), u, 0.0f};
            std::tie(*bar1, *bar2, *bar3) = std::make_tuple((1.0f - u), u, 0.0f);

        z = zs * bar;
        PutShaderedPixel(canvas, zBuffer, x, y, z, bar, shader);
    }
}

template <class T>
std::pair<T, T> minmax(T v1, T v2, T v3)
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

void RasterizeTriangle(Image& canvas, Canvas<float>& zBuffer, float farZ, Vertex v1, Vertex v2,
                       Vertex v3, Shader& shader)
{
    float width = (float)canvas.width;
    float height = (float)canvas.height;

    const Vec3f zs = {v1.coord.z, v2.coord.z, v3.coord.z};

    Vec3f bar;
    float* bar1 = &bar[0];
    float* bar2 = &bar[1];
    float* bar3 = &bar[2];

    // TODO: we don't actualy need to swap entire Vertex. (x,y) should be enough.
    if (v1.coord.y > v2.coord.y)
    {
        std::swap(v1, v2);
        std::swap(bar1, bar2);
    }
    if (v2.coord.y > v3.coord.y)
    {
        std::swap(v2, v3);
        std::swap(bar2, bar3);
    }
    if (v1.coord.y > v2.coord.y)
    {
        std::swap(v1, v2);
        std::swap(bar1, bar2);
    }

    Vec2i i1 = {(int)(v1.coord.x), (int)(v1.coord.y)};
    Vec2i i2 = {(int)(v2.coord.x), (int)(v2.coord.y)};
    Vec2i i3 = {(int)(v3.coord.x), (int)(v3.coord.y)};

    if (i1.y >= height || i3.y < 0)
        return;

    auto [minX, maxX] = minmax(i1.x, i2.x, i3.x);
    if (maxX < 0 || minX >= width)
        return;

    auto [minZ, maxZ] = minmax(v1.coord.z, v2.coord.z, v3.coord.z);
    if (minZ < 0 || maxZ >= farZ)
        return;

    if (i1.y == i3.y)
    {
        RasterizeHorizontalDegenerateTriangle(canvas, zBuffer, v1, v2, v3, shader);
        return;
    }

    int startY = std::max(0, i1.y);
    int endY = std::min(static_cast<int>(height - 1), i3.y);

    float triangle_height = (float)(i3.y - i1.y);

    for (int y = startY; y <= endY; ++y)
    {
        bool upperSegment = y > i2.y;
        float t = (y - i1.y) / triangle_height;
        float u =
            upperSegment ? (y - i2.y) / (float)(i3.y - i2.y) : (y - i1.y) / (float)(i2.y - i1.y);

        int x1 = (int)(i1.x + t * (i3.x - i1.x));
        int x2 = (int)(upperSegment ? i2.x + u * (i3.x - i2.x) : i1.x + u * (i2.x - i1.x));

        if (x1 > x2)
            std::swap(x1, x2);

        if (x2 < 0 || x1 > width - 1)
            continue;

        if (x1 == x2)
        {
            // Vec3f bar = {1.0f - t, 0.0f, t};
            std::tie(*bar1, *bar2, *bar3) = std::make_tuple(1.0f - t, 0.0f, t);
            PutShaderedPixel(canvas, zBuffer, x1, y, bar * zs, bar, shader);
        }
        else
        {
            int startX = x1 < 0 ? 0 : x1;
            int endX = x2 >= width ? width - 1 : x2;

            float invXLength = 1.0f / (x2 - x1);
            for (int x = startX; x <= endX; ++x)
            {
                float s = (x - x1) * invXLength;

                if (upperSegment)
                {
                    float b2 = (1.0f - s) * u;
                    float b3 = s * t;
                    // bar = Vec3f{1.0f - b2 - b3, b2, b3};
                    std::tie(*bar1, *bar2, *bar3) = std::make_tuple(1.0f - b2 - b3, b2, b3);
                }
                else
                {
                    float b1 = s * (1.0f - t);
                    float b2 = (1.0f - s) * (1.0f - u);
                    // bar = Vec3f{b1, b2, 1.0f - b1 - b2};
                    std::tie(*bar1, *bar2, *bar3) = std::make_tuple(b1, b2, 1.0f - b1 - b2);
                }

                PutShaderedPixel(canvas, zBuffer, x, y, bar * zs, bar, shader);
            }
        }
    }
}

void RasterizeTriangle(Image& canvas, Vec2f p1, Vec2f p2, Vec2f p3, Color color)
{
    Vec3f v1 = {p2.x - p1.x, p3.x - p1.x, 1.0f};
    Vec3f v2 = {p2.y - p1.y, p3.y - p1.y, 1.0f};

    Rect<float> rect = BoundingBox(p1, p2, p3);
    for (float y = rect.bottom; y <= rect.top; ++y)
        for (float x = rect.left; x <= rect.right; ++x)
        {
            v1.z = p1.x - x;
            v2.z = p1.y - y;
            Vec3f tmp = Cross(v1, v2);
            tmp.x = tmp.x / tmp.z;
            tmp.y = tmp.y / tmp.z;
            Vec3f bar = Vec3f{1 - tmp.x - tmp.y, tmp.x, tmp.y};

            if (bar.x >= 0.0f && bar.y >= 0.0f && bar.z >= 0.0f)
                canvas.SetPixel(x, y, color);
        }
}

} // namespace sr
