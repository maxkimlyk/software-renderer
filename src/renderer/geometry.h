#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "definitions.h"

#include <initializer_list>
#include <iostream>

namespace sr
{

template <size_t n, class T>
struct Mat;

template <class T>
struct Rect
{
    T left;
    T right;
    T top;
    T bottom;
};

typedef Rect<float> Rectf;
typedef Rect<double> Rectd;
typedef Rect<int> Recti;

template <class T>
struct Box
{
    T xmin;
    T xmax;
    T ymin;
    T ymax;
    T zmin;
    T zmax;
};

typedef Box<float> Boxf;
typedef Box<double> Boxd;
typedef Box<int> Boxi;

template <size_t n, class T>
struct Vec_
{
    T v[n];
};

template <class T>
struct Vec_<2, T>
{
    union
    {
        T v[2];
        struct
        {
            T x, y;
        };
    };
};

template <class T>
struct Vec_<3, T>
{
    union
    {
        T v[3];
        struct
        {
            T x, y, z;
        };
    };
};

template <class T>
struct Vec_<4, T>
{
    union
    {
        T v[4];
        struct
        {
            T x, y, z, w;
        };
    };
};

template <size_t n, class T>
struct Vec : Vec_<n, T>
{
    using base = Vec_<n, T>;

    Vec()
    {}

    Vec(const Vec<n, T>& other) = default;

    Vec(std::initializer_list<T> list)
    {
        size_t i = 0;
        for (auto elem = list.begin(); elem != list.end(); ++i, ++elem)
            base::v[i] = *elem;
    }

    T& operator[](size_t index)
    {
        return base::v[index];
    }

    const T& operator[](size_t index) const
    {
        return base::v[index];
    }

    template <class T2>
    operator Vec<n, T2>()
    {
        Vec<n, T2> other;
        for (size_t i = 0; i < n; ++i)
            other[i] = (T2)(base::v[i]);
        return other;
    }

    void Fill(T val)
    {
        for (size_t i = 0; i < n; ++i)
            base::v[i] = val;
    }

    T Norm() const
    {
        return std::sqrt((*this) * (*this));
    }

    T Sum() const
    {
        T sum = 0;
        for (size_t i = 0; i < n; ++i)
            sum += base::v[i];
        return sum;
    }
};

template <size_t n, class T>
T operator*(const Vec<n, T>& lhs, const Vec<n, T>& rhs)
{
    T sum = 0;
    for (size_t i = 0; i < n; ++i)
        sum += lhs[i] * rhs[i];
    return sum;
}

template <size_t n, class T>
Vec<n, T> operator*(Vec<n, T> lhs, T rhs)
{
    for (size_t i = 0; i < n; ++i)
        lhs[i] *= rhs;
    return lhs;
}

template <size_t n, class T>
Vec<n, T> operator*(T lhs, const Vec<n, T>& rhs)
{
    return rhs * lhs;
}

template <size_t n, class T>
Vec<n, T> operator/(const Vec<n, T>& lhs, T rhs)
{
    T inv = T(1) / rhs;
    return lhs * inv;
}

template <size_t n, class T>
Vec<n, T> operator+(Vec<n, T> lhs, const Vec<n, T>& rhs)
{
    for (size_t i = 0; i < n; ++i)
        lhs[i] += rhs[i];
    return lhs;
}

template <size_t n, class T>
Vec<n, T> operator-(Vec<n, T> lhs, const Vec<n, T>& rhs)
{
    for (size_t i = 0; i < n; ++i)
        lhs[i] -= rhs[i];
    return lhs;
}

template <size_t n, class T>
bool operator==(const Vec<n, T>& lhs, const Vec<n, T>& rhs)
{
    for (size_t i = 0; i < n; ++i)
        if (lhs[i] != rhs[i])
            return false;
    return true;
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

template <class T>
Vec<3, T> Cross(const Vec<3, T>& lhs, const Vec<3, T>& rhs)
{
    return Vec<3, T>{lhs[1] * rhs[2] - lhs[2] * rhs[1], lhs[2] * rhs[0] - lhs[0] * rhs[2],
                     lhs[0] * rhs[1] - lhs[1] * rhs[0]};
}

template <size_t n, class T>
Vec<n, T> Embed(const Vec<n - 1, T>& vec, T value = 1)
{
    Vec<n, T> newVec;
    for (size_t i = 0; i < n - 1; ++i)
        newVec[i] = vec[i];
    newVec[n - 1] = value;
    return newVec;
}

template <size_t n, class T>
Vec<n, T> Project(const Vec<n + 1, T>& vec)
{
    Vec<n, T> newVec;
    for (size_t i = 0; i < n; ++i)
        newVec[i] = vec[i];
    return newVec;
}

template <size_t n, class T>
std::ostream& operator<<(std::ostream& os, const Vec<n, T>& rhs)
{
    os << "(";
    for (size_t i = 0; i < n - 1; ++i)
        os << rhs[i] << ", ";
    os << rhs[n - 1] << ")";
    return os;
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

template <size_t n, class T>
class VecView
{
  public:
    using Ptr = T*;

    Ptr ref[n];

    VecView(Vec<n, T>& vec)
    {
        for (size_t i = 0; i < n; ++i)
            ref[i] = &vec[i];
    }

    void operator=(const Vec<n, T>& vec)
    {
        for (size_t i = 0; i < n; ++i)
            *ref[i] = vec[i];
    }

    Ptr& operator[](size_t i)
    {
        return ref[i];
    }
};

// deduction guide
template <size_t n, class T>
VecView(Vec<n, T>&)->VecView<n, T>;

typedef Vec<3, float> RefVec3f;

template <size_t n, class T>
struct DetWrapper
{
    static T Det(const Mat<n, T>& mat)
    {
        T sum = 0;
        for (int j = 0; j < n; ++j)
        {
            Mat<n - 1, T> minor = mat.CofactorMatrix(0, j);
            T determ = DetWrapper<n - 1, T>::Det(minor);
            T sign = j & 1 ? T(-1) : T(1);
            sum += sign * mat[0][j] * determ;
        }
        return sum;
    }
};

template <class T>
struct DetWrapper<1, T>
{
    static T Det(const Mat<1, T>& mat)
    {
        return mat[0][0];
    }
};

template <size_t n, class T>
struct Mat
{
    Vec<n, T> rows[n];

    Mat()
    {}

    Mat(std::initializer_list<Vec<n, T>> list)
    {
        size_t i = 0;
        for (auto elem = list.begin(); elem != list.end(); ++i, ++elem)
            rows[i] = *elem;
    }

    template <class T2>
    operator Mat<n, T2>()
    {
        Mat<n, T2> other;
        for (size_t i = 0; i < n; ++i)
            other[i] = (Vec<n, T2>)(rows[i]);
        return other;
    }

    Vec<n, T>& operator[](size_t row)
    {
        return rows[row];
    }

    const Vec<n, T>& operator[](size_t row) const
    {
        return rows[row];
    }

    Vec<n, T>& Row(size_t i)
    {
        return rows[i];
    }

    const Vec<n, T>& Row(size_t i) const
    {
        return rows[i];
    }

    Vec<n, T> Column(size_t j) const
    {
        Vec<n, T> res;
        for (size_t i = 0; i < n; ++i)
            res[i] = rows[i][j];
        return res;
    }

    Mat<n - 1, T> CofactorMatrix(size_t row, size_t col) const
    {
        Mat<n - 1, T> minor;

        size_t srcI = row == 0 ? 1 : 0;

        for (int dstI = 0; dstI < n - 1; ++dstI)
        {
            size_t srcJ = col == 0 ? 1 : 0;

            for (int dstJ = 0; dstJ < n - 1; ++dstJ)
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

    T Determ() const
    {
        return DetWrapper<n, T>::Det(*this);
    }

    T MaxAbs() const
    {
        T max = 0;
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                if (std::abs(rows[i][j]) > max)
                    max = std::abs(rows[i][j]);
        return max;
    }

    static Mat<n, T> Identity()
    {
        Mat<n, T> mat;
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                mat[i][j] = i == j ? T(1) : T(0);
        return mat;
    }
};

typedef Mat<3, int> Mat3i;
typedef Mat<4, int> Mat4i;
typedef Mat<3, float> Mat3f;
typedef Mat<4, float> Mat4f;
typedef Mat<3, double> Mat3d;
typedef Mat<4, double> Mat4d;

template <size_t n, class T>
Mat<n, T> Transpose(const Mat<n, T>& mat)
{
    Mat<n, T> res;
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            res[i][j] = mat[j][i];
    return res;
}

template <size_t n, class T>
Mat<n, T> Inverse(const Mat<n, T>& mat)
{
    Mat<n, T> res;
    T det = mat.Determ();
    T invDet = (T)(1) / det; // user should ensure that the matrix have inverse
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
        {
            T minor = mat.CofactorMatrix(i, j).Determ();
            T sign = (i + j) & 1 ? T(-1) : T(1);
            res[j][i] = sign * minor * invDet;
        }
    return res;
}

template <size_t n, class T>
Mat<n, T> Reduce(const Mat<n + 1, T>& mat)
{
    Mat<n, T> reduced;
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            reduced[i][j] = mat[i][j];
    return reduced;
}

template <size_t n, class T>
Mat<n, T> operator*(const Mat<n, T>& lhs, const Mat<n, T>& rhs)
{
    Mat<n, T> res;
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            res[i][j] = lhs[i] * rhs.Column(j);
    return res;
}

template <size_t n, class T>
Vec<n, T> operator*(const Mat<n, T>& lhs, const Vec<n, T>& rhs)
{
    Vec<n, T> res;
    for (size_t i = 0; i < n; ++i)
        res[i] = lhs[i] * rhs;
    return res;
}

template <size_t n, class T>
Vec<n, T> operator*(Mat<n, T> lhs, T rhs)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            lhs[i][j] *= rhs;
    return lhs;
}

template <size_t n, class T>
Vec<n, T> operator*(T lhs, const Mat<n, T>& rhs)
{
    return rhs * lhs;
}

template <size_t n, class T>
Mat<n, T> operator+(Mat<n, T> lhs, const Mat<n, T>& rhs)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            lhs[i][j] += rhs[i][j];
    return lhs;
}

template <size_t n, class T>
Mat<n, T> operator-(Mat<n, T> lhs, const Mat<n, T>& rhs)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            lhs[i][j] -= rhs[i][j];
    return lhs;
}

template <size_t n, class T>
bool operator==(const Mat<n, T>& lhs, const Mat<n, T>& rhs)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            if (lhs[i][j] != rhs[i][j])
                return false;
    return true;
}

template <size_t n, class T>
std::ostream& operator<<(std::ostream& os, Mat<n, T>& rhs)
{
    for (size_t i = 0; i < n; ++i)
        os << rhs[i] << std::endl;
    return os;
}

} // namespace sr

#endif
