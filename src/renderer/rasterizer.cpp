#include "rasterizer.h"
#include "shader.h"

namespace Rasterizer
{

template <class T> static inline void MinMax(T a, T b, T c, T& min, T& max)
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

template <size_t n, class T> static Rect<T> BoundingBox(Vec<n, T> p1, Vec<n, T> p2, Vec<n, T> p3)
{
    Rect<T> rect;
    MinMax(p1[0], p2[0], p3[0], rect.left, rect.right);
    MinMax(p1[1], p2[1], p3[1], rect.bottom, rect.top);
    return rect;
}

void Rectangle(Image& canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
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

void SolidRect(Image& canvas, int32_t x1, int32_t y1, int32_t x2, int32_t y2, Color color)
{
    int xinc = x2 > x1 ? 1 : -1;
    int yinc = y2 > y1 ? 1 : -1;
    for (int y = y1; y != y2; y += yinc)
        for (int x = x1; x != x2; x += xinc)
        {
            canvas.SetPixel(x, y, color);
        }
}

void Line(Image& canvas, Vec2i p1, Vec2i p2, Color color)
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

inline void PutPixel(Image& canvas, Canvas<float>& zBuffer, int x, int y, float z, Color color)
{
    if (z >= 0 && z < zBuffer.At(x, y))
    {
        zBuffer.SetPixel(x, y, z);
        canvas.SetPixel(x, y, color);
    }
}

inline void PutShaderedPixel(Image& canvas, Canvas<float>& zBuffer, int x, int y, float z,
                             Vec3f bar, Shader& shader)
{
    Color color;

    if (z >= 0 && z < zBuffer.At(x, y) && shader.pixel(bar, color))
    {
        zBuffer.SetPixel(x, y, z);
        canvas.SetPixel(x, y, color);
    }
}

inline void HorizontalDegenerateTriangle(Image& canvas, Canvas<float>& zBuffer, Vec3f p1, Vec3f p2,
                                         Vec3f p3, Shader& shader)
{
    // TODO: test this code

    // TODO: remove this later
    Vec3f zs = {p1.z, p2.z, p3.z};

    float width = (float)canvas.width;
    float height = (float)canvas.height;

    if (p1.x > p2.x)
        std::swap(p1, p2);
    if (p2.x > p3.x)
        std::swap(p2, p3);
    if (p1.x > p2.x)
        std::swap(p1, p2);

    float x1 = (float)((int)p1.x);
    float x2 = (float)((int)p2.x);
    float x3 = (float)((int)p3.x);

    int y = (int)p1.y;

    if (x1 == x3)
    {
        int x = (int)p1.x;
        PutShaderedPixel(canvas, zBuffer, x, y, p1.z, Vec3f{1.0f, 0.0f, 0.0f}, shader);
        PutShaderedPixel(canvas, zBuffer, x, y, p2.z, Vec3f{0.0f, 1.0f, 0.0f}, shader);
        PutShaderedPixel(canvas, zBuffer, x, y, p3.z, Vec3f{0.0f, 0.0f, 1.0f}, shader);
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
        Vec3f bar = {(1.0f - t), 0.0f, t};
        float z = zs * bar;
        PutShaderedPixel(canvas, zBuffer, x, y, z, bar, shader);

        // sedond side
        if (rightSegment)
            bar = Vec3f{0.0f, (1.0f - u), u};
        else
            bar = Vec3f{(1.0f - u), u, 0.0f};

        z = zs * bar;
        PutShaderedPixel(canvas, zBuffer, x, y, z, bar, shader);
    }
}

template <class T> inline void minmax(T v1, T v2, T v3, T& min, T& max)
{
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
}

void Triangle(Image& canvas, Canvas<float>& zBuffer, float farZ, Vec3f p1, Vec3f p2, Vec3f p3,
              Shader& shader)
{
    // remove this later
    Vec3f zs = {p1.z, p2.z, p3.z};

    float width = (float)canvas.width;
    float height = (float)canvas.height;

    if (p1.y > p2.y)
        std::swap(p1, p2);
    if (p2.y > p3.y)
        std::swap(p2, p3);
    if (p1.y > p2.y)
        std::swap(p1, p2);

    Vec2i i1 = {(int)(p1.x), (int)(p1.y)};
    Vec2i i2 = {(int)(p2.x), (int)(p2.y)};
    Vec2i i3 = {(int)(p3.x), (int)(p3.y)};

    if (i1.y >= height || i3.y < 0)
        return;

    int minX, maxX;
    minmax(i1.x, i2.x, i3.x, minX, maxX);

    if (maxX < 0 || minX >= width)
        return;

    float minZ, maxZ;
    minmax(p1.z, p2.z, p3.z, minZ, maxZ);

    if (minZ < 0 || maxZ >= farZ)
        return;

    if (i1.y == i3.y)
    {
        HorizontalDegenerateTriangle(canvas, zBuffer, p1, p2, p3, shader);
        return;
    }

    int startY = i1.y < 0 ? 0 : i1.y;
    int endY = i3.y >= height ? height - 1 : i3.y;

    float totalHeight = (float)(i3.y - i1.y);

    for (int y = startY; y <= endY; ++y)
    {
        bool upperSegment = y > i2.y;
        float t = (y - i1.y) / totalHeight;
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
            Vec3f bar = {1.0f - t, 0.0f, t};
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

                Vec3f bar;

                if (upperSegment)
                {
                    float b2 = (1.0f - s) * u;
                    float b3 = s * t;
                    bar = Vec3f{1.0f - b2 - b3, b2, b3};
                }
                else
                {
                    float b1 = s * (1.0f - t);
                    float b2 = (1.0f - s) * (1.0f - u);
                    bar = Vec3f{b1, b2, 1.0f - b1 - b2};
                }

                PutShaderedPixel(canvas, zBuffer, x, y, bar * zs, bar, shader);
            }
        }
    }
}

void Triangle(Image& canvas, Vec2f p1, Vec2f p2, Vec2f p3, Color color)
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

} // namespace Rasterizer
