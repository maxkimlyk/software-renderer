#ifndef _SHADER_H_
#define _SHADER_H_

#include "../common/canvas.h"
#include "geometry.h"

namespace sr
{

class Shader
{
  public:
    virtual bool pixel(Vec3f bar, Color& resultColor) = 0;
    virtual void vertex(Vec3f p1, Vec3f p2, Vec3f p3) = 0;
};

namespace DefaultShaders
{
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

    virtual void vertex(Vec3f v1, Vec3f v2, Vec3f v3) override
    {
        Vec3f norm = Normalize(Cross(v3 - v1, v3 - v2));
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

    virtual void vertex(Vec3f v1, Vec3f v2, Vec3f v3) override
    {}
};
} // namespace DefaultShaders

} // namespace sr

#endif
