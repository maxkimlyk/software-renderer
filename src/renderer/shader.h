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
        // TODO: correct by model view
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

extern PhongShader phongShader;

class FlatShader : public Shader
{
    Color color = Color(255, 255, 255);

  public:
    Vec3f lightDirection = Vec3f{0.0f, 0.0f, 1.0f};

    virtual bool pixel(Vec3f bar, Color& resultColor) override
    {
        resultColor = color;
        return true;
    }

    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) override
    {
        Vec3f norm = Normalize(Cross(v3.coord - v1.coord, v3.coord - v2.coord));
        float cross = lightDirection * norm;
        float lightIntensity = cross > 0 ? cross : 0;
        color.r = (uint8_t)(255.0f * lightIntensity);
        color.g = (uint8_t)(255.0f * lightIntensity);
        color.b = (uint8_t)(255.0f * lightIntensity);
    }
};

extern FlatShader flatShader;

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
