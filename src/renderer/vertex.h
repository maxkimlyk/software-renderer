#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "geometry.h"

namespace sr
{

struct Vertex
{
    Vec3f coord;
    Vec3f norm;
    Vec2f tex;

    Vertex() = default;
    Vertex(const Vertex&) = default;
    Vertex(const Vec3f& vec) : coord(vec), norm({0.0f, 0.0f, 0.0f}), tex({0.0f, 0.0f, 0.0f})
    {}
    Vertex(const Vec3f& coord, const Vec3f& norm, const Vec2f tex)
        : coord(coord), norm(norm), tex(tex)
    {}

    operator Vec3f() const
    {
        return coord;
    }
};

} // namespace sr

#endif
