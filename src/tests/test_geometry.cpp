#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../common/geometry.h"

TEST_CASE( "Vector", "[Vec]" )
{
    Vec3f vec1 = {0.0f, 1.0f, 2.0f};
    Vec3f vec2 = {2.0f, 1.0f, 0.0f};

    CHECK_FALSE(vec1 == vec2);
    CHECK(vec1 == vec1);
    CHECK(vec1 * vec2 == 1.0f);
    CHECK(vec1 + vec2 == Vec3f {2.0f, 2.0f, 2.0f});
    CHECK(vec1 - vec2 == Vec3f {-2.0f, 0.0f, 2.0f});
    CHECK(vec1 * 3.0f == Vec3f {0.0f, 3.0f, 6.0f});
    CHECK(3.0f * vec1 == Vec3f {0.0f, 3.0f, 6.0f});
    CHECK(vec1.Norm() == std::sqrt(5.0f));

    CHECK(Vec3i(vec1) == Vec3i {0, 1, 2});
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
    CHECK((mat2 * inv - id).MaxAbs() < 0.001);

    CHECK(Transpose(mat2) == Mat3f {{1.0f, 4.0f, 2.0f}, {2.0f, 5.0f, 2.0f}, {3.0f, 6.0f, 8.0f}});

    CHECK(Mat3i(mat1) == Mat3i {{0, 1, 2}, {0, 1, 2}, {3, 4, 5}});
}
