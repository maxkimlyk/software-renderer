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
    virtual bool pixel(Vec3f bar, Color& result_color) = 0;
    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) = 0;
};

namespace impl
{
class SupportsNormalCorrection
{
  public:
    SupportsNormalCorrection() : is_norm_corr_identity_(true)
    {}

    void SetNormCorrection(const Mat4f& mat)
    {
        if (mat == Mat4f::Identity())
        {
            is_norm_corr_identity_ = true;
        }
        else
        {
            norm_corr_matrix_ = mat;
            is_norm_corr_identity_ = false;
        }
    }

  protected:
    Vec3f CorrectNormal(const Vec3f& norm) const
    {
        if (is_norm_corr_identity_)
            return norm;
        Vec4f transformed = norm_corr_matrix_ * Embed<4>(norm, 0.0f);
        return Project<3>(transformed);
    };

  private:
    Mat4f norm_corr_matrix_;
    bool is_norm_corr_identity_;
};

class SupportsGlobalLight
{
  public:
    void SetLightDirection(const Vec3f& dir)
    {
        minus_light_direction_ = -dir;
    }

    Vec3f GetLightDirection() const
    {
        return -minus_light_direction_;
    }

  protected:
    Vec3f minus_light_direction_ = Vec3f{0.0f, 0.0f, 1.0f};
};
}; // namespace impl

namespace DefaultShaders
{
class SmoothLight : public Shader,
                    public impl::SupportsNormalCorrection,
                    public impl::SupportsGlobalLight
{
    Vec3f norm1_, norm2_, norm3_;

  public:
    Color color = Color(255, 255, 255);
    float ambient_light_intensity = 0.0f;

    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) override
    {
        norm1_ = CorrectNormal(v1.norm);
        norm2_ = CorrectNormal(v2.norm);
        norm3_ = CorrectNormal(v3.norm);
    }

    virtual bool pixel(Vec3f bar, Color& result_color) override
    {
        const Vec3f norm = bar[0] * norm1_ + bar[1] * norm2_ + bar[2] * norm3_;
        const float dot = minus_light_direction_ * norm;
        const float intensity = dot > 0 ? dot : 0;
        const float coef =
            ambient_light_intensity + (1.0f - ambient_light_intensity) * intensity;
        result_color = Color(color.r * coef, color.g * coef, color.b * coef);
        return true;
    }
};

class SmoothTexture : public Shader,
                      public impl::SupportsNormalCorrection,
                      public impl::SupportsGlobalLight
{
    const Image& texture;

    Vec3f us;
    Vec3f vs;
    Vec3f norm1, norm2, norm3;

  public:
    SmoothTexture(const Image& texture) : texture(texture)
    {}

    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) override
    {
        us = Vec3f{v1.tex.x, v2.tex.x, v3.tex.x};
        vs = Vec3f{v1.tex.y, v2.tex.y, v3.tex.y};
        norm1 = v1.norm;
        norm2 = v2.norm;
        norm3 = v3.norm;
    }

    virtual bool pixel(Vec3f bar, Color& result_color) override
    {
        const Vec3f norm = bar[0] * norm1 + bar[1] * norm2 + bar[2] * norm3;
        const float dot = minus_light_direction_ * norm;
        const float intensity = dot > 0 ? dot : 0;

        const float u = bar * us;
        const float v = bar * vs;
        const Color color = texture.AtSafe(std::round(u * (texture.width - 1)),
                                           std::round(v * (texture.height - 1)));

        result_color = Color(color.r * intensity, color.g * intensity, color.b * intensity);
        return true;
    }
};

class FlatLight : public Shader,
                  public impl::SupportsNormalCorrection,
                  public impl::SupportsGlobalLight
{
  public:
    Color color = Color(255, 255, 255);
    float ambient_light_intensity = 0.1f;

    virtual bool pixel(Vec3f bar, Color& result_color) override
    {
        result_color = result_color_;
        return true;
    }

    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) override
    {
        const Vec3f model_norm = Cross(v3.coord - v1.coord, v2.coord - v1.coord);
        const Vec3f norm = Normalize(CorrectNormal(model_norm));
        const float cross = minus_light_direction_ * norm;
        const float light_intensity = cross > 0 ? cross : 0;
        const float coef =
            ambient_light_intensity + (1.0f - ambient_light_intensity) * light_intensity;
        result_color_ = Color(color.r * coef, color.g * coef, color.b * coef);
    }

  private:
    Color result_color_;
};

class FlatTexture : public Shader
{
    Vec3f us;
    Vec3f vs;
    const Image& texture;

  public:
    FlatTexture(const Image& texture) : texture(texture)
    {}

    virtual bool pixel(Vec3f bar, Color& result_color) override
    {
        const float u = bar * us;
        const float v = bar * vs;
        result_color = texture.AtSafe(std::round(u * (texture.width - 1)),
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

    virtual bool pixel(Vec3f bar, Color& result_color) override
    {
        result_color = color;
        return true;
    }

    virtual void vertex(const Vertex& v1, const Vertex& v2, const Vertex& v3) override
    {}
};
} // namespace DefaultShaders

} // namespace sr

#endif
