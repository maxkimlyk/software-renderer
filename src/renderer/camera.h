#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "geometry.h"
#include "transforms.h"

class Camera
{
    Vec3f position;
    Vec3f direction;
    Vec3f upDirection;
    Vec3f rightDirection;

  public:
    Camera()
    {
        position = Vec3f{0.0f, 0.0f, 0.0f};
        direction = Vec3f{0.0f, 0.0f, 1.0f};
        upDirection = Vec3f{0.0f, 1.0f, 0.0f};
        rightDirection = Cross(direction, upDirection);
    }

    void LookAt(Vec3f eye, Vec3f position)
    {
        this->direction = eye - position;
        this->rightDirection = Cross(direction, upDirection);
        this->position = position;
    }

    void Yaw(float angle)
    {
        Mat3f transform = Reduce<3, float>(Transform::RotateY(angle));
        direction = transform * direction;
        rightDirection = transform * rightDirection;
    }

    void Pitch(float angle)
    {
        Mat3f transform = Reduce<3, float>(Transform::Rotate(angle, rightDirection));
        direction = transform * direction;
        rightDirection = transform * rightDirection;
    }

    void Walk(float dist)
    {
        position = position + dist * direction;
    }

    void WalkRight(float dist)
    {
        position = position + dist * rightDirection;
    }

    Mat4f ViewMatrix()
    {
        return Transform::LookAt(position + direction, position, upDirection);
    }
};

#endif
