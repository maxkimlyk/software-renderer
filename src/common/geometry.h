#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <cmath>

template <size_t n, class T>
class Vec
{
    T v[n];

public:
    T& operator[] (size_t index)
    {
        return v[index];
    }

    const T& operator[] (size_t index) const
    {
        return v[index];
    }

    void Fill(T val)
    {
        for (size_t i = 0; i < n; i++)
            v[i] = val;
    }

    T Norm()
    {
        return std::sqrt( (*this) * (*this) );
    }
};

template <size_t n, class T>
T operator* (Vec<n, T> &lhs,  Vec<n, T> &rhs)
{
    T sum = 0;
    for (size_t i = 0; i < n; ++i)
        sum += lhs[i] * rhs[i];
    return sum;
}

template <size_t n, class T>
Vec<n, T> operator* (Vec<n, T> lhs, T rhs)
{
    for (size_t i = 0; i < n; ++i)
        lhs[i] *= rhs;
    return lhs;
}

template <size_t n, class T>
Vec<n, T> operator* (T lhs, Vec<n, T> &rhs)
{
    return rhs * lsh;
}

template <size_t n, class T>
Vec<n, T> operator/ (Vec<n, T> &lhs, T rhs)
{
    T inv = T(1) / rhs;
    return lhs * inv;
}

template <size_t n, class T>
Vec<n, T> operator+ (Vec<n, T> lhs,  Vec<n, T> &rhs)
{
    for (size_t i = 0; i < n; ++i)
        lhs[i] += rhs[i];
    return lhs;
}

template <size_t n, class T>
Vec<n, T> operator- (Vec<n, T> lhs,  Vec<n, T> &rhs)
{
    for (size_t i = 0; i < n; ++i)
        lhs[i] -= rhs[i];
    return lhs;
}

template <size_t n, class T>
Vec<n, T> Normalize(Vec<n, T> vec)
{
    T norm = vec.Norm();
    if (norm != 0)
        return vec / norm;
    else
        return vec;
}

template <size_t n, class T>
Vec<n+1, T> Embed(Vec<n, T> &vec, T value = 1)
{
    Vec<n+1, T> newVec;
    for (size_t i = 0; i < n; ++i)
        newVec[i] = vec[i];
    newVec[n] = value;
    return newVec;
}

template <size_t n, class T>
Vec<n, T> Project(Vec<n+1, T> &vec)
{
    Vec<n, T> newVec;
    for (size_t i = 0; i < n; ++i)
        newVec[i] = vec[i];
    return newVec;
}

typedef Vec<2, int> Vec2i;
typedef Vec<3, int> Vec3i;
typedef Vec<4, int> Vec4i;
typedef Vec<2, float> Vec2f;
typedef Vec<3, float> Vec3f;
typedef Vec<4, float> Vec4f;
typedef Vec<2, double> Vec2d;
typedef Vec<3, double> Vec3d;
typedef Vec<4, double> Vec4d;

#endif
