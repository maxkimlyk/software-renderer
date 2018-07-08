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

template<size_t n, class T>
class Mat
{
    Vec<n, T> rows[n];

public:
    Vec<n, T>& operator[] (size_t row)
    {
        return rows[row];
    }

    const Vec<n, T>& operator[] (size_t row) const
    {
        return row[row];
    }

    Vec<n, T> Row(size_t i)
    {
        return rows[i];
    }

    Vec<n, T> Column(size_t j)
    {
        Vec<n, T> res;
        for (size_t i = 0; i < n; ++i)
            res[i] = rows[i][j];
        return res;
    }

    Vec<n-1, T> Minor(size_t row, size_t col) // TODO: rename it. Minor is determinant of adjective matrix
    {
        Vec<n-1, T> minor;

        size_t srcI = row == 0 ? 1 : 0;

        for (int dstI = 0; dstI < n-1; ++dstI)
        {
            size_t srcJ = col == 0 ? 1 : 0;

            for (int dstJ = 0; dstJ < n-1; ++dstJ)
            {
                minor[dstI][dstJ] = rows[srcI][srcJ];

                if (++srcJ == col)
                    ++srcJ;
            }

            if (++srcI == row)
                ++srcI;
        }

        return minor;
    }

    T Determ()
    {
        return DetWrapper<n, T>::det(*this);
    }
};

template <size_t n, class T>
struct DetWrapper
{
    static inline T Sign(size_t index)
    {
        return 1 - 2 * (index & 1);
    }

    static T Det(Mat<n, T> &mat)
    {
        sum = 0;
        for (int j = 0; j < n; ++j)
        {
            Mat<n-1, T> minor = mat.Minor(0, j);
            T determ = DetWrapper<n-1, T>::Det(minor); // workaround for partial template function specialization
            sum += Sign(0, j) * mat[0][j] * determ;
        }
        return sum;
    }
};

template <class T>
struct DetWrapper<1, T>
{
    static T Det(Mat<1, T> &mat)
    {
        return mat[0][0];
    }
};

template<size_t n, class T>
Mat<n, T> Transpose(Mat<n, T> &mat)
{
    Mat<n, T> res;
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            res[i][j] = mat[j][i];
    return res;
}

template<size_t n, class T>
Mat<n, T> Inverse(Mat<n, T> &mat)
{
    Mat<n, T> res;
    T det = mat.Determ();
    T invDet = (T)(1) / det; // user should ensure that the matrix have inverse
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
        {
            T minorDet = mat.Minor(i, j).Determ();
            res[i][j] = minorDet * invDet;
        }
    return res;
}

template<size_t n, class T>
Mat<n, T> operator* (Mat<n, T> &lhs, Mat<n, T> &rhs)
{
    Mat<n, T> res;
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            res[i][j] = lhs[i] * rhs.Column(j);
    return res;
}

template<size_t n, class T>
Vec<n, T> operator* (Mat<n, T> &lhs, Vec<n, T> &rhs)
{
    Vec<n, T> res;
    for (size_t i = 0; i < n; ++i)
        res[i] = lhs[i] * rhs;
}

template<size_t n, class T>
Vec<n, T> operator* (Mat<n, T> lhs, T rhs)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            lhs[i][j] *= rhs;
    return lhs;
}

template<size_t n, class T>
Vec<n, T> operator* (T lhs, Mat<n, T> &rhs)
{
    return rhs * lhs;
}

template<size_t n, class T>
Mat<n, T> operator+ (Mat<n, T> lhs, Mat<n, T> &rhs)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            lhs[i][j] += rhs[i][j];
    return lhs;
}

template<size_t n, class T>
Mat<n, T> operator- (Mat<n, T> lhs, Mat<n, T> &rhs)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            lhs[i][j] -= rhs[i][j];
    return lhs;
}

#endif
