#ifndef __MATRIX_STACK_H__
#define __MATRIX_STACK_H__

#include "geometry.h"

#include <list>

namespace sr
{

enum class MatrixType
{
    PROJECTION,
    VIEW,
    MODEL
};

class MatrixStack
{
    static const MatrixType default_type_ = MatrixType::MODEL;

  public:
    MatrixStack()
        : model_matrix_(Mat4f::Identity()), view_matrix_(Mat4f::Identity()),
          projection_matrix_(Mat4f::Identity()), is_full_transform_calculated_(false)
    {
        SetMode(default_type_);
    }

    void SetMode(MatrixType type)
    {
        current_type_ = type;
        current_ = &GetMatrixByType(type);
    }

    void Set(const Mat4f& matrix)
    {
        *current_ = matrix;
        is_full_transform_calculated_ = false;
    }

    void Transform(const Mat4f& matrix)
    {
        *current_ = *current_ * matrix;
        is_full_transform_calculated_ = false;
    }

    void Push()
    {
        if (is_full_transform_calculated_)
        {
            saved_.push_back(Saved{current_type_, *current_, full_transform_matrix_,
                                   is_full_transform_calculated_});
        }
        else
        {
            saved_.push_back(Saved{current_type_, *current_});
        }
    }

    void Pop()
    {
        const Saved& saved = saved_.back();
        GetMatrixByType(saved.type) = saved.current_matrix;
        if (saved.full_transform_calculated)
            full_transform_matrix_ = saved.full_transform_matrix;
        is_full_transform_calculated_ = saved.full_transform_calculated;

        saved_.pop_back();
    }

    const Mat4f& GetFullTransformMatrix()
    {
        if (!is_full_transform_calculated_)
            CalculateTransform();
        return full_transform_matrix_;
    }

    Mat4f GetModelView() const
    {
        return view_matrix_ * model_matrix_;
    }

    const Mat4f GetModel() const
    {
        return model_matrix_;
    }

    void SetProjection(const Mat4f& mat)
    {
        projection_matrix_ = mat;
        is_full_transform_calculated_ = false;
    }

    void SetView(const Mat4f& mat)
    {
        view_matrix_ = mat;
        is_full_transform_calculated_ = false;
    }

    void SetModel(const Mat4f& mat)
    {
        model_matrix_ = mat;
        is_full_transform_calculated_ = false;
    }

  private:
    struct Saved
    {
        MatrixType type;
        Mat4f current_matrix;
        Mat4f full_transform_matrix;
        bool full_transform_calculated;

        Saved(MatrixType type, const Mat4f& current_matrix)
            : type(type), current_matrix(current_matrix), full_transform_calculated(false)
        {}

        Saved(MatrixType type, const Mat4f& current_matrix, const Mat4f& full_transform_matrix,
              bool full_transform_calculated)
            : type(type), current_matrix(current_matrix),
              full_transform_matrix(full_transform_matrix),
              full_transform_calculated(full_transform_calculated)
        {}
    };

    void CalculateTransform()
    {
        full_transform_matrix_ = projection_matrix_ * view_matrix_ * model_matrix_;
        is_full_transform_calculated_ = true;
    }

    Mat4f& GetMatrixByType(MatrixType type)
    {
        switch (type)
        {
        case MatrixType::PROJECTION:
            return projection_matrix_;
        case MatrixType::VIEW:
            return view_matrix_;
        case MatrixType::MODEL:
            return model_matrix_;
        }
    }

    Mat4f model_matrix_;
    Mat4f view_matrix_;
    Mat4f projection_matrix_;

    Mat4f full_transform_matrix_;
    bool is_full_transform_calculated_;

    Mat4f* current_;
    MatrixType current_type_;

    std::list<Saved> saved_;
};

} // namespace sr

#endif
