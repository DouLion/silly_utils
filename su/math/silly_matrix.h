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
namespace su
{
template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class matrix
{
    /// <summary>
    /// 这个目前是线程不安全的,使用时需要注意
    /// </summary>
  public:
    enum IpolMethod:int
    {
        /** nearest neighbor interpolation 最临近插值 */
        INTER_NEAREST = 0,
        /** bilinear interpolation 二次性插值 */
        INTER_LINEAR = 1,
    };

  public:
    /// <summary>
    /// 创建一个指定大小的二维数据
    /// </summary>
    /// <param name="row">行数</param>
    /// <param name="col">列数</param>
    /// <param name="reset">true: 重新初始化为0 false: 不做任何操作,并且返回false</param>
    /// <returns></returns>
    bool create(const size_t &row, const size_t &col, bool reset = false)
    {
        if (!row || !col)
        {
            return false;
        }
        if (m_data && !reset)
        {
            return false;
        }
        if (m_data && reset)  // 重复调用此函数, 会重置
        {
            release();
        }
        m_row = row;
        m_col = col;
        m_total = m_row * m_col;

        m_data = (T *)malloc(m_total * sizeof(T));
        if (!m_data)
        {
            return false;
        }
        memset(m_data, 0, m_total * sizeof(T));

        return true;
    }

    /// <summary>
    /// 仅传递数据指针
    /// </summary>
    /// <param name="rh"></param>
    /// <returns></returns>
    matrix<T> &operator=(const matrix<T> &rh)
    {
        this->m_row = rh.m_row;
        this->m_col = rh.m_col;
        this->m_data = rh.m_data;
        this->m_total = rh.m_total;
        return *this;
    }

    void operator+=(const matrix<T> &rh)
    {
        if (m_data && rh.m_data && m_row == rh.m_row && m_col == rh.m_col)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i] += rh.m_data[i];
                i++;
            }
        }
    }

    matrix<T> operator+(const matrix<T> &rh)
    {
        matrix<T> ret;
        if (rh.m_row != m_row || rh.m_col != m_col)
        {
            throw std::runtime_error("矩阵大小不匹配");
        }
        matrix<T> ret.create(m_row, m_col);
        size_t i = 0;
        T *p = ret.m_data;
        T *pl = this->m_data;
        T *pr = rh.m_data;
        while (i++ < m_total)
        {
            *p = *pl + *pr;
            p++;
            pl++;
            pr++;
        }
        return ret;
    }

    void operator+=(const T &rh)
    {
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i] += rh;
                i++;
            }
        }
    }

    matrix<T> operator+(const T &rh)
    {
        size_t i = 0;
        while (i < m_total)
        {
            this->m_data[i++] = rh;
        }
    }

    void operator-=(const matrix<T> &rh)
    {
        if (m_data && rh.m_data && m_row == rh.m_row && m_col == rh.m_col)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i] -= rh.m_data[i];
                i++;
            }
        }
    }

    /// <summary>
    /// 等同seek_row
    /// </summary>
    /// <param name="r"></param>
    /// <returns></returns>
    T *operator[](const size_t &r)
    {
        if (m_data)
        {
            return m_data + (r * m_col);
        }
        return nullptr;
    }

    T *operator[](const size_t &r) const
    {
        if (m_data)
        {
            return m_data + (r * m_col);
        }
        return nullptr;
    }

    matrix<T> operator*(const matrix<T> &rh)
    {
        throw std::runtime_error("未实现");
        // return matrix<T>();
    }

    void operator-=(const T &rh)
    {
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i] -= rh;
                i++;
            }
        }
    }

    void operator*=(const T &rh)
    {
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i] *= rh;
                i++;
            }
        }
    }

    matrix<T> operator*(const T &v)
    {
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i] *= v;
                i++;
            }
        }
        return *this;
    }

    void operator/=(const T &rh)
    {
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i] /= rh;
                i++;
            }
        }
    }

    matrix<T> operator/(const T &v)
    {
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i] /= v;
                i++;
            }
        }
        return *this;
    }

    matrix<T> operator-(const T&v)
    {
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i] -= v;
                i++;
            }
        }
        return *this;
    }

    T *seek_row(const size_t &r)
    {
        if (m_data)
        {
            return m_data + (r * m_col);
        }

        return nullptr;
    }

    /// <summary>
    /// 复制数据内容到新的指针地址
    /// </summary>
    /// <returns></returns>
    matrix<T> copy()
    {
        matrix<T> ret;
        ret.create(m_row, m_col);
        if (m_data)
        {
            memcpy(ret.m_data, m_data, m_total * sizeof(T));
        }

        return ret;
    }

    /// <summary>
    /// 等同at函数
    /// </summary>
    /// <param name="r"></param>
    /// <param name="c"></param>
    /// <returns></returns>
    T &at(size_t r, size_t c) const
    {
        /*if (r < m_row && c < m_col && m_data)
        {
            return m_data[r * m_col + c];
        }
        return m_mp;*/
        // 这个地方应该考虑如何更优雅的实现保护
        return m_data[r * m_col + c];
    }

    /// <summary>
    /// 将矩阵设置为一个值
    /// </summary>
    /// <param name="val"></param>
    void set(T val)
    {
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                m_data[i++] = val;
            }
        }
    }

    /// <summary>
    /// 获取矩阵中的最大值
    /// </summary>
    /// <returns></returns>
    T max() const
    {
        if (m_data)
        {
            return *std::max_element(m_data, m_data + m_total);
        }
        return static_cast<T>(INT_MIN);
    }

    /// <summary>
    /// 获取矩阵中最小值
    /// </summary>
    /// <returns></returns>
    T min() const
    {
        if (m_data)
        {
            return *std::min_element(m_data, m_data + m_total);
        }
        return static_cast<T>(INT_MAX);
    }

    /// <summary>
    ///  限制所有值不能大于val
    /// </summary>
    /// <param name="val"></param>
    void max(const T &val)
    {
        if (m_data)
        {
            for (size_t i = 0; i < m_total; ++i)
            {
                m_data[i] = std::max(val, m_data[i]);
            }
        }
    }

    /// <summary>
    /// 限制所有值不能小于val
    /// </summary>
    /// <param name="val"></param>
    void min(const T &val)
    {
        if (m_data)
        {
            for (size_t i = 0; i < m_total; ++i)
            {
                m_data[i] = std::min(val, m_data[i]);
            }
        }
    }

    /// <summary>
    /// 释放内存,需要手动调用
    /// </summary>
    void release()
    {
        if (m_data)
        {
            free(m_data);
            m_data = nullptr;
        }
    }
    const size_t row() const
    {
        return m_row;
    }

    const size_t col() const
    {
        return m_col;
    }

    bool empty() const
    {
        return m_data == nullptr || m_row == 0 || m_col == 0;
    }

    /// <summary>
    /// 将U数据类型的矩阵赋值给T数据类型的矩阵, 输入的矩阵会注销掉
    /// </summary>
    /// <typeparam name="U"></typeparam>
    /// <param name="rh"></param>
    /// <returns></returns>
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    matrix<T> cast(const matrix<U> &rh)
    {
        create(rh.row(), rh.col(), true);
        if (!m_data)
        {
            throw std::bad_alloc();  // or handle allocation failure
        }
        for (size_t i = 0; i < m_total; ++i)
        {
            m_data[i] = static_cast<T>(rh.m_data[i]);
        }
        return *this;
    }

    template <typename T>
    static bool resize(matrix<T> &src, matrix<T> &dst, const size_t &dst_row, const size_t &dst_col, const IpolMethod &flag = IpolMethod::INTER_NEAREST)
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
    static bool inter_nearest_resize(matrix<T> &src, matrix<T> &dst, const size_t &dst_row, const size_t &dst_col)
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
    static bool bilinear_resize(matrix<T> &src, matrix<T> &dst, const size_t &dst_row, const size_t &dst_col)
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

    /// <summary>
    /// 返回data的首地址
    /// </summary>
    /// <returns></returns>
    T *get_data() const
    {
        return m_data;
    }

  private:
    T *m_data{nullptr};
    // 行数
    size_t m_row{0};
    // 列数
    size_t m_col{0};
    // 总数据量(个数) m_row * m_col
    size_t m_total{0};
};

typedef matrix<int> IMatrix;
typedef matrix<unsigned int> UIMatrix;
typedef matrix<float> FMatrix;
typedef matrix<double> DMatrix;
typedef matrix<short> SMatrix;
typedef matrix<unsigned short> USMatrix;
typedef matrix<char> CMatrix;
typedef matrix<unsigned char> UCMatrix;
typedef matrix<long long> LMatrix;
typedef matrix<unsigned long long> UMatrix;

/// 参照opencv

class matrix_tools
{
  public:
    

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
    
};

}  // namespace silly_math

#endif  // SILLY_UTILS_SILLY_MATRIX_H