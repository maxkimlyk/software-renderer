#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "geometry.h"
#include "transforms.h"

// class Camera
// {
//     friend class Renderer;

//     Mat4f view;
//     Mat4f proj;

// public:
//     float updownAngle;
//     float leftrightAngle;
//     Vec3f position;

//     Camera(float disposal = 1.0f)
//     {
//         updownAngle = 0.0f;
//         leftrightAngle = 0.0f;

//         view = Mat4f::Identity();
//         proj = Mat4f::Identity();
//         Frustum(-1, 1, -1, 1, 1.0f, 100.0f);
//     }

//     void UpdateView()
//     {
//         view = Transform::RotateX(-updownAngle) *
//             Transform::RotateY(-leftrightAngle) *
//             Transform::Translate(-position.x, -position.y, -position.z);
//     }
// };

#endif
