#ifndef _CLIPPING_H_
#define _CLIPPING_H_

#include "geometry.h"
#include <algorithm>

namespace Clipping
{
bool ClipLine(Vec3f p1, Vec3f p2, Boxf box, Vec3f& res1, Vec3f& res2);
bool TriangleClip(Vec3f p1, Vec3f p2, Vec3f p3, Boxf box, Vec3f& res11, Vec3f& res12, Vec3f& res21,
                  Vec3f& res22, Vec3f& res31, Vec3f& res32);
bool TriangleClipRect(Vec3f p1, Vec3f p2, Vec3f p3, Boxf box, Rectf& result);
} // namespace Clipping

#endif
