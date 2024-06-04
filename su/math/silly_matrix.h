/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/26 14:55
 * @version: 1.0.1
 * @software: silly_utils
 * @description:

*/
#pragma once

#ifndef SILLY_UTILS_SILLY_MATRIX_H
#define SILLY_UTILS_SILLY_MATRIX_H
#include <su_marco.h>

namespace silly_math
{
template <typename T>
class matrix_2d
{
    /// <summary>
    /// 这个目前是线程不安全的,使用时需要注意
    /// </summary>
  public:
    matrix_2d<T>() = default;

    /// <summary>
    /// 仅传递数据指针
    /// </summary>
    /// <param name="other"></param>
    /// <returns></returns>
    matrix_2d<T> &operator=(const matrix_2d<T> &other)
    {
        this->rows = other.rows;
        this->cols = other.cols;
        this->data = other.data;
        this->total = other.total;
        return *this;
    }

    /// <summary>
    /// 创建一个指定大小的二维数据
    /// </summary>
    /// <param name="r"></param>
    /// <param name="c"></param>
    /// <param name="reset">true: 如果有数据就清空并且重新初始化 false: 做任何操作,并且返回false</param>
    /// <returns></returns>
    bool create(const size_t &row, const size_t &col, bool reset = false)
    {
        if (!row || !col)
        {
            return false;
        }
        if (data && !reset)
        {
            return false;
        }
        if (data && reset)  // 重复调用此函数, 会重置
        {
            destroy();
        }
        rows = row;
        cols = col;
        total = rows * cols;

        data = (T *)malloc(total * sizeof(T));
        if (!data)
        {
            return false;
        }
        memset(data, 0, total * sizeof(T));

        return true;
    }

    void operator+=(const matrix_2d<T> &other)
    {
        if (data && other.data && rows == other.rows && cols == other.cols)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i] += other.data[i];
                i++;
            }
        }
    }

    void operator-=(const matrix_2d<T> &other)
    {
        if (data && other.data && rows == other.rows && cols == other.cols)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i] -= other.data[i];
                i++;
            }
        }
    }

    /// <summary>
    /// 等同seek_row
    /// </summary>
    /// <param name="r"></param>
    /// <returns></returns>
    T *operator[](size_t r)
    {
        if (data)
        {
            return data + (r * cols);
        }
        return nullptr;
    }

    matrix_2d<T> operator*(const matrix_2d<T> &m2)
    {
        return matrix_2d<T>();
    }

    matrix_2d<T> operator+(const matrix_2d<T> &m2)
    {
        return matrix_2d<T>();
    }

    void operator+=(const T &other)
    {
        if (data)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i] += other;
                i++;
            }
        }
    }

    void operator-=(const T &other)
    {
        if (data)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i] -= other;
                i++;
            }
        }
    }

     void operator*=(const T &other)
    {
         if (data)
         {
             size_t i = 0;
             while (i < total)
             {
                 data[i] *= other;
                 i++;
             }
         }
    }

    void operator/=(const T &other)
    {
        if (data)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i] /= other;
                i++;
            }
        }
    }

    matrix_2d<T> operator+(const T *v)
    {
        if (data)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i] += v;
                i++;
            }
        }
        return *this;
    }

    matrix_2d<T> operator-(const T *v)
    {
        if (data)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i] -= v;
                i++;
            }
        }
        return *this;
    }

    matrix_2d<T> operator*(const T *v)
    {
        if (data)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i] *= v;
                i++;
            }
        }
        return *this;
    }

    matrix_2d<T> operator/(const T *v)
    {
        if (data)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i] /= v;
                i++;
            }
        }
        return *this;
    }

    T *seek_row(const size_t &r)
    {
        if (data)
        {
            return data + (r * cols);
        }

        return nullptr;
    }

    /// <summary>
    /// 复制数据内容到新的指针地址
    /// </summary>
    /// <returns></returns>
    matrix_2d<T> copy()
    {
        matrix_2d<T> ret;
        ret.create(rows, cols);
        if (data)
        {
            memcpy(ret.data, data, total * sizeof(T));
        }

        return ret;
    }

    /// <summary>
    /// 等同at函数
    /// </summary>
    /// <param name="r"></param>
    /// <param name="c"></param>
    /// <returns></returns>
    T &at(size_t r, size_t c)
    {
        /*if (r < rows && c < cols && data)
        {
            return data[r * cols + c];
        }
        return mp;*/
        // 这个地方应该考虑如何更优雅的实现保护
        return data[r * cols + c];
    }

    /// <summary>
    /// 将矩阵设置为一个值
    /// </summary>
    /// <param name="val"></param>
    void set(T val)
    {
        if (data)
        {
            size_t i = 0;
            while (i < total)
            {
                data[i++] = val;
            }
        }
    }

    /// <summary>
    /// 释放内存,需要手动调用
    /// </summary>
    void destroy()
    {
        if (data)
        {
            free(data);
            data = nullptr;
        }
    }
    const size_t row() const
    {
        return rows;
    }

    const size_t col() const
    {
        return cols;
    }

  private:
    T *data{nullptr};
    // 行数
    size_t rows{0};
    // 列数
    size_t cols{0};
    // 总数据量
    size_t total{0};
    T mp{0};
};

typedef matrix_2d<int> IMatrix;
typedef matrix_2d<unsigned int> UIMatrix;
typedef matrix_2d<float> FMatrix;
typedef matrix_2d<double> DMatrix;
typedef matrix_2d<short> SMatrix;
typedef matrix_2d<unsigned short> USMatrix;
typedef matrix_2d<char> CMatrix;
typedef matrix_2d<unsigned char> UCMatrix;
typedef matrix_2d<long long> LMatrix;
typedef matrix_2d<unsigned long long> UMatrix;

/// 参照opencv
enum InterpolationFlags
{
    /** nearest neighbor interpolation 最临近插值 */
    INTER_NEAREST = 0,
    /** bilinear interpolation 二次性插值 */
    INTER_LINEAR = 1,
};

class matrix_tools
{
  public:
    /// <summary>
    /// 两个不同类型的矩阵相互转换
    /// </summary>
    /// <typeparam name="T1"></typeparam>
    /// <typeparam name="T2"></typeparam>
    /// <param name="src">源矩阵</param>
    /// <param name="dst">目标矩阵</param>
    /// <returns></returns>
    template <typename T1, typename T2>
    static bool convert_matrix(silly_math::matrix_2d<T1> &src, silly_math::matrix_2d<T2> &dst)
    {
        size_t rows = src.row();
        size_t cols = src.col();

        bool is_convert = dst.create(rows, cols);
        if (!is_convert)
        {
            return is_convert;
        }

        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                dst.at(i, j) = static_cast<T2>(src.at(i, j));
            }
        }
        return true;
    }

    /// <summary>
    /// 对矩阵进行缩放操作
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="src">源矩阵</param>
    /// <param name="dst">目标矩阵</param>
    /// <param name="dst_row">目标矩阵行数</param>
    /// <param name="dst_col">目标矩阵列数</param>
    /// <param name="flag">插值算法</param>
    /// <returns></returns>
    template <typename T>
    static bool resize(matrix_2d<T> &src, matrix_2d<T> &dst, const size_t &dst_row, const size_t &dst_col, const InterpolationFlags &flag = INTER_NEAREST)
    {
        switch (flag)
        {
            case INTER_LINEAR:
                return bilinear_resize(src, dst, dst_row, dst_col);
            default:
                return inter_nearest_resize(src, dst, dst_row, dst_col);
        }
    }

  private:
    template <typename T>
    static bool inter_nearest_resize(matrix_2d<T> &src, matrix_2d<T> &dst, const size_t &dst_row, const size_t &dst_col)
    {
        const size_t src_row = src.row();
        const size_t src_col = src.col();

        const float row_ratio = static_cast<float>(src_row) / dst_row;
        const float col_ratio = static_cast<float>(src_col) / dst_col;

        dst.create(dst_row, dst_col);

        for (size_t i = 0; i < dst_row; ++i)
        {
            const size_t src_i = std::min(static_cast<size_t>(i * row_ratio), src_row - 1);

            for (size_t j = 0; j < dst_col; ++j)
            {
                const size_t src_j = std::min(static_cast<size_t>(j * col_ratio), src_col - 1);
                dst.at(i, j) = src.at(src_i, src_j);
            }
        }

        return true;
    }

    template <typename T>
    static bool bilinear_resize(matrix_2d<T> &src, matrix_2d<T> &dst, const size_t &dst_row, const size_t &dst_col)
    {
        size_t src_row = src.row();
        size_t src_col = src.col();

        float row_ratio = static_cast<float>(src_row) / dst_row;
        float col_ratio = static_cast<float>(src_col) / dst_col;

        dst.create(dst_row, dst_col);

        for (size_t i = 0; i < dst_row; ++i)
        {
            float src_i_float = static_cast<float>(i) * row_ratio;
            size_t src_i_floor = static_cast<size_t>(std::floor(src_i_float));
            size_t src_i_ceil = std::min(src_i_floor + 1, src_row - 1);

            float y_diff = src_i_float - src_i_floor;

            for (size_t j = 0; j < dst_col; ++j)
            {
                float src_j_float = static_cast<float>(j) * col_ratio;
                size_t src_j_floor = static_cast<size_t>(std::floor(src_j_float));
                size_t src_j_ceil = std::min(src_j_floor + 1, src_col - 1);

                float x_diff = src_j_float - src_j_floor;

                // Pre-calculate the top-left, top-right, bottom-left, bottom-right corners
                T top_left = src.at(src_i_floor, src_j_floor);
                T top_right = src.at(src_i_floor, src_j_ceil);
                T bottom_left = src.at(src_i_ceil, src_j_floor);
                T bottom_right = src.at(src_i_ceil, src_j_ceil);

                // Perform bilinear interpolation
                T interpolated_value = static_cast<T>((1 - x_diff) * (1 - y_diff) * top_left + x_diff * (1 - y_diff) * top_right + (1 - x_diff) * y_diff * bottom_left + x_diff * y_diff * bottom_right);

                dst.at(i, j) = interpolated_value;
            }
        }

        return true;
    }
};

}  // namespace silly_math

#endif  // SILLY_UTILS_SILLY_MATRIX_H