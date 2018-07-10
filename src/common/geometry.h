#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <cmath>

template <size_t n, class T> class Mat;

template <size_t n, class T>
class Vec
{
    T v[n];

public:
    Vec() {}

    Vec(std::initializer_list<T> list)
    {
        size_t i = 0;
        for (auto elem = list.begin(); elem != list.end(); ++i, ++elem)
            v[i] = *elem;
    }

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
    return rhs * lhs;
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
bool operator== (const Vec<n, T> &lhs,  const Vec<n, T> &rhs)
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

template <size_t n, class T>
std::ostream& operator<< (std::ostream &os, Vec<n, T> &rhs)
{
    os << "(";
    for (size_t i = 0; i < n-1; ++i)
        os << rhs[i] << ", ";
    os << rhs[n-1] << ")";
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
struct DetWrapper
{
    static T Det(Mat<n, T> &mat)
    {
        T sum = 0;
        for (int j = 0; j < n; ++j)
        {
            Mat<n-1, T> minor = mat.CofactorMatrix(0, j);
            T determ = DetWrapper<n-1, T>::Det(minor); // workaround for partial template function specialization
            T sign = j & 1 ? T(-1) : T(1);
            sum += sign * mat[0][j] * determ;
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
class Mat
{
    Vec<n, T> rows[n];

public:
    Mat() {}

    Mat(std::initializer_list<Vec<n, T>> list)
    {
        size_t i = 0;
        for (auto elem = list.begin(); elem != list.end(); ++i, ++elem)
            rows[i] = *elem;
    }

    Vec<n, T>& operator[] (size_t row)
    {
        return rows[row];
    }

    const Vec<n, T>& operator[] (size_t row) const
    {
        return rows[row];
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

    Mat<n-1, T> CofactorMatrix(size_t row, size_t col)
    {
        Mat<n-1, T> minor;

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
        return DetWrapper<n, T>::Det(*this);
    }

    T MaxAbs()
    {
        T max = 0;
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                if (std::abs(rows[i][j]) > max)
                    max = std::abs(rows[i][j]);
        return max;
    }
};

typedef Mat<3, float> Mat3f;
typedef Mat<4, float> Mat4f;
typedef Mat<3, double> Mat3d;
typedef Mat<4, double> Mat4d;

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
            T minor = mat.CofactorMatrix(i, j).Determ();
            T sign = (i + j) & 1 ? T(-1) : T(1);
            res[j][i] = sign * minor * invDet;
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

template<size_t n, class T>
bool operator== (const Mat<n, T> &lhs, const Mat<n, T> &rhs)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            if (lhs[i][j] != rhs[i][j])
                return false;
    return true;
}

template <size_t n, class T>
std::ostream& operator<< (std::ostream &os, Mat<n, T> &rhs)
{
    for (size_t i = 0; i < n; ++i)
        os << rhs[i] << std::endl;
    return os;
}

#endif
