#ifndef _SHADER_H_
#define _SHADER_H_

#include "../common/canvas.h"
#include "geometry.h"
#include "vertex.h"

namespace sr
{

class Shader
{
  public:
    virtual bool pixel(Vec3f bar, Color& resultColor) = 0;
    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) = 0;
};

namespace DefaultShaders
{
class PhongShader : public Shader
{
    Vec3f norm1, norm2, norm3;

  public:
    Vec3f lightDirection = Vec3f{0.0f, 0.0f, 1.0f};

    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) override
    {
        norm1 = v1.norm;
        norm2 = v2.norm;
        norm3 = v3.norm;
    }

    virtual bool pixel(Vec3f bar, Color& resultColor) override
    {
        const Vec3f norm = bar[0] * norm1 + bar[1] * norm2 + bar[2] * norm3;
        const float dot = lightDirection * norm;
        const float intensity = dot > 0 ? dot : 0;
        resultColor = Color(255 * intensity, 255 * intensity, 255 * intensity);
        return true;
    }
};

class FlatShader : public Shader
{
    Color color = Color(255, 255, 255);

  public:
    float ambient_light_intensity = 0.1f;
    Vec3f light_direction = Vec3f{0.0f, 0.0f, -1.0f};
    Mat4f corr_matrix = Mat4f::Identity();

    virtual bool pixel(Vec3f bar, Color& resultColor) override
    {
        resultColor = color;
        return true;
    }

    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) override
    {
        const Vec3f norm = Normalize(Cross(v3.coord - v1.coord, v2.coord - v1.coord));
        const Vec4f norm4 = Embed<4>(norm, 0.0f);
        const Vec4f corr_norm4 = corr_matrix * norm4;
        const Vec3f corr_norm = Normalize(Project<3>(corr_norm4));
        const float cross = light_direction * corr_norm;
        const float lightIntensity = cross > 0 ? cross : 0;
        const float value =
            255.0f * (ambient_light_intensity + (1.0f - ambient_light_intensity) * lightIntensity);
        color = Color(value, value, value);
    }
};

class FlatTexture : public Shader
{
    Vec3f us;
    Vec3f vs;
    const Image& texture;

  public:
    FlatTexture(const Image& texture) : texture(texture)
    {}

    virtual bool pixel(Vec3f bar, Color& resultColor) override
    {
        const float u = bar * us;
        const float v = bar * vs;
        resultColor = texture.AtSafe(std::round(u * (texture.width - 1)),
                                     std::round(v * (texture.height - 1)));
        return true;
    }

    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) override
    {
        us = Vec3f{v1.tex.x, v2.tex.x, v3.tex.x};
        vs = Vec3f{v1.tex.y, v2.tex.y, v3.tex.y};
    }
};

class SolidColor : public Shader
{
    Color color;

  public:
    SolidColor(const Color& color) : color(color)
    {}

    virtual bool pixel(Vec3f bar, Color& resultColor) override
    {
        resultColor = color;
        return true;
    }

    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) override
    {}
};
} // namespace DefaultShaders

} // namespace sr

#endif
