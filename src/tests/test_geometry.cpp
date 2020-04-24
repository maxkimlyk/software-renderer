#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../renderer/geometry.h"
#include "../renderer/transforms.h"

TEST_CASE( "Vector", "[Vec]" )
{
    Vec3f vec1 = {0.0f, 1.0f, 2.0f};
    Vec3f vec2 = {2.0f, 1.0f, 0.0f};
    Vec2f vec3 = {1.0f, 2.0f};

    CHECK_FALSE(vec1 == vec2);
    CHECK(vec1 == vec1);
    CHECK(vec1 * vec2 == 1.0f);
    CHECK(vec1 + vec2 == Vec3f {2.0f, 2.0f, 2.0f});
    CHECK(vec1 - vec2 == Vec3f {-2.0f, 0.0f, 2.0f});
    CHECK(vec1 * 3.0f == Vec3f {0.0f, 3.0f, 6.0f});
    CHECK(3.0f * vec1 == Vec3f {0.0f, 3.0f, 6.0f});
    CHECK(vec1.Norm() == std::sqrt(5.0f));
    CHECK(Cross(Vec3f {1.0f, 0.0f, 0.0f}, Vec3f {0.0f, 1.0f, 0.0f}) == Vec3f {0.0f, 0.0f, 1.0f} );

    CHECK(Vec3i(vec1) == Vec3i {0, 1, 2});

    CHECK(vec3.x == 1.0f);
    CHECK(vec3.y == vec3[1]);
    CHECK(vec2.z == 0.0f);
}

TEST_CASE( "Matrix", "[Mat]" )
{
    Mat3f mat1 = {{0.0f, 1.0f, 2.0f}, {0.0f, 1.0f, 2.0f}, {3.0f, 4.0f, 5.0f}};
    CHECK(mat1[1][1] == 1.0f);
    CHECK(mat1.Determ() == 0.0f);

    Mat3f id = Mat3f {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
    CHECK(mat1 * id == mat1);

    Mat3f mat2 = {{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}, {2.0f, 2.0f, 8.0f}};
    Mat3f inv = Inverse(mat2);
    CHECK((mat2 * inv - id).MaxAbs() < 0.001f);

    CHECK(Transpose(mat2) == Mat3f {{1.0f, 4.0f, 2.0f}, {2.0f, 5.0f, 2.0f}, {3.0f, 6.0f, 8.0f}});

    CHECK(Mat3i(mat1) == Mat3i {{0, 1, 2}, {0, 1, 2}, {3, 4, 5}});
}

TEST_CASE( "Rotation", "[Transform]" )
{
    const float eps = 0.001f;

    Mat4f testRotate0 = Transform::Rotate(0.0f, Vec3f {3.0f, 2.0f, 1.0f});
    Mat4f refRotate0 = Mat4f::Identity();
    CHECK((refRotate0 - testRotate0).MaxAbs() < eps);

    const float angle = 1.0f;
    Mat4f testRotateX = Transform::Rotate(angle, Vec3f {1.0f, 0.0f, 0.0f});
    Mat4f testRotateY = Transform::Rotate(angle, Vec3f {0.0f, 1.0f, 0.0f});
    Mat4f testRotateZ = Transform::Rotate(angle, Vec3f {0.0f, 0.0f, 1.0f});
    Mat4f refRotateX = Transform::RotateX(angle);
    Mat4f refRotateY = Transform::RotateY(angle);
    Mat4f refRotateZ = Transform::RotateZ(angle);

    CHECK((refRotateX - testRotateX).MaxAbs() < eps);
    CHECK((refRotateY - testRotateY).MaxAbs() < eps);
    CHECK((refRotateZ - testRotateZ).MaxAbs() < eps);
}
