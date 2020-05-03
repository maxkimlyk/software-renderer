#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "geometry.h"
#include "transforms.h"

namespace sr
{

class Camera
{
    Vec3f position_;
    Vec3f direction_;
    Vec3f up_direction_;
    Vec3f right_direction_;

  public:
    Camera()
    {
        position_ = Vec3f{0.0f, 0.0f, 0.0f};
        direction_ = Vec3f{0.0f, 0.0f, 1.0f};
        up_direction_ = Vec3f{0.0f, 1.0f, 0.0f};
        right_direction_ = Cross(direction_, up_direction_);
    }

    void LookAt(const Vec3f& eye, const Vec3f& position)
    {
        this->direction_ = Normalize(eye - position);
        this->right_direction_ = Normalize(Cross(direction_, up_direction_));
        this->position_ = position;
    }

    void Yaw(float angle)
    {
        Mat3f transform = Reduce<3, float>(Transform::RotateY(angle));
        direction_ = transform * direction_;
        right_direction_ = transform * right_direction_;
    }

    void Pitch(float angle)
    {
        Mat3f transform = Reduce<3, float>(Transform::Rotate(angle, right_direction_));
        direction_ = transform * direction_;
        right_direction_ = transform * right_direction_;
    }

    void Walk(float dist)
    {
        position_ = position_ + dist * direction_;
    }

    void WalkRight(float dist)
    {
        position_ = position_ + dist * right_direction_;
    }

    void RiseUp(float dist)
    {
        position_ = position_ + dist * up_direction_;
    }

    Mat4f ViewMatrix()
    {
        return Transform::LookAt(position_ + direction_, position_, up_direction_);
    }
};

} // namespace sr

#endif
