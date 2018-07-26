#ifndef _CLIP_H_
#define _CLIP_H_

#include "geometry.h"

class Clipping
{
    enum
    {
        LEFT = 0x01,
        RIGHT = 0x02,
        BELOW = 0x04,
        ABOVE = 0x08,
        BACK = 0x10,
        FRONT = 0x20
    };

    static uint8_t PositionCode(Vec3f p1, Boxf box)
    {
        uint8_t code = 0;
        if (p1.x < box.xmin)
            code |= LEFT;
        else if (p1.x > box.xmax)
            code |= RIGHT;
        if (p1.y < box.ymin)
            code |= BELOW;
        else if (p1.y > box.ymax)
            code |= ABOVE;
        if (p1.z < box.zmin)
            code |= BACK;
        else if (p1.z > box.zmax)
            code |= FRONT;
        return code;
    }

public:
    static bool ClipLine(Vec3f p1, Vec3f p2, Boxf box, Vec3f &res1, Vec3f &res2)
    {
        uint8_t code1 = PositionCode(p1, box);
        uint8_t code2 = PositionCode(p2, box);

        while (true)
        {
            if ((code1 | code2) == 0) // both points inside the box
            {
                res1 = p1;
                res2 = p2;
                return true;
            }
            else if (code1 & code2) // both points on the same side from the box
            {
                return false;
            }
            else
            {
                Vec3f &point = code1 != 0 ? p1 : p2;
                uint8_t &code = code1 != 0 ? code1 : code2;
                if (code & LEFT)
                {
                    float coef = (box.xmin - p1.x) / (p2.x - p1.x);
                    point.x = box.xmin;
                    point.y = p1.y + coef * (p2.y - p1.y);
                    point.z = p1.z + coef * (p2.z - p1.z);
                }
                else if (code & RIGHT)
                {
                    float coef = (box.xmax - p1.x) / (p2.x - p1.x);
                    point.x = box.xmax;
                    point.y = p1.y + coef * (p2.y - p1.y);
                    point.z = p1.z + coef * (p2.z - p1.z);
                }
                else if (code & BELOW)
                {
                    float coef = (box.ymin - p1.y) / (p2.y - p1.y);
                    point.x = p1.x + coef * (p2.x - p1.x);
                    point.y = box.ymin;
                    point.z = p1.z + coef * (p2.z - p1.z);
                }
                else if (code & ABOVE)
                {
                    float coef = (box.ymax - p1.y) / (p2.y - p1.y);
                    point.x = p1.x + coef * (p2.x - p1.x);
                    point.y = box.ymax;
                    point.z = p1.z + coef * (p2.z - p1.z);
                }
                else if (code & BACK)
                {
                    float coef = (box.zmin - p1.z) / (p2.z - p1.z);
                    point.x = p1.x + coef * (p2.x - p1.x);
                    point.y = p1.y + coef * (p2.y - p1.y);
                    point.z = box.zmin;
                }
                else if (code & FRONT)
                {
                    float coef = (box.zmax - p1.z) / (p2.z - p1.z);
                    point.x = p1.x + coef * (p2.x - p1.x);
                    point.y = p1.y + coef * (p2.y - p1.y);
                    point.z = box.zmax;
                }
                code = PositionCode(point, box);
            }
        }
    }

    static bool TriangleClip(Vec3f p1, Vec3f p2, Vec3f p3, Boxf box,
                             Vec3f &res11, Vec3f &res12, Vec3f &res21, Vec3f &res22, Vec3f &res31, Vec3f &res32)
    {
        bool inside1 = ClipLine(p1, p2, box, res11, res12);
        bool inside2 = ClipLine(p2, p3, box, res21, res22);
        bool inside3 = ClipLine(p3, p1, box, res31, res32);
        return inside1 | inside2 | inside3;
    }

    static bool TriangleClipRect(Vec3f p1, Vec3f p2, Vec3f p3, Boxf box, Rectf &result)
    {
        Vec3f clippedVerts[6];

        bool inside = TriangleClip(p1, p2, p3, box, clippedVerts[0], clippedVerts[1], clippedVerts[2], clippedVerts[3], clippedVerts[4], clippedVerts[5]);
        if (!inside)
            return false;

        result.left = clippedVerts[0].x;
        result.right = clippedVerts[0].x;
        result.top = clippedVerts[0].y;
        result.bottom = clippedVerts[0].y;

        for (size_t i = 1; i < 6; ++i)
        {
            if (clippedVerts[i].x < result.left)
                result.left = clippedVerts[i].x;
            else if (clippedVerts[i].x > result.right)
                result.right = clippedVerts[i].x;
            if (clippedVerts[i].y < result.bottom)
                result.bottom = clippedVerts[i].y;
            else if (clippedVerts[i].y > result.top)
                result.top = clippedVerts[i].y;
        }

        return true;
    }
};

#endif
