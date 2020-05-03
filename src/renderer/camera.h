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
    Vec3f upDirection_;
    Vec3f rightDirection_;

  public:
    Camera()
    {
        position_ = Vec3f{0.0f, 0.0f, 0.0f};
        direction_ = Vec3f{0.0f, 0.0f, 1.0f};
        upDirection_ = Vec3f{0.0f, 1.0f, 0.0f};
        rightDirection_ = Cross(direction_, upDirection_);
    }

    void LookAt(Vec3f eye, Vec3f position)
    {
        this->direction_ = eye - position;
        this->rightDirection_ = Cross(direction_, upDirection_);
        this->position_ = position;
    }

    void Yaw(float angle)
    {
        Mat3f transform = Reduce<3, float>(Transform::RotateY(angle));
        direction_ = transform * direction_;
        rightDirection_ = transform * rightDirection_;
    }

    void Pitch(float angle)
    {
        Mat3f transform = Reduce<3, float>(Transform::Rotate(angle, rightDirection_));
        direction_ = transform * direction_;
        rightDirection_ = transform * rightDirection_;
    }

    void Walk(float dist)
    {
        position_ = position_ + dist * direction_;
    }

    void WalkRight(float dist)
    {
        position_ = position_ + dist * rightDirection_;
    }

    Mat4f ViewMatrix()
    {
        return Transform::LookAt(position_ + direction_, position_, upDirection_);
    }
};

} // namespace sr

#endif
