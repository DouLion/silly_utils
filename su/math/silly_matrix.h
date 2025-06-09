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
    enum IpolMethod : int
    {
        /** nearest neighbor interpolation 最临近插值 */
        INTER_NEAREST = 0,
        /** bilinear interpolation 二次性插值 */
        INTER_LINEAR = 1,
    };

  private:
    T *m_data{nullptr};
    // 行数
    size_t m_row{0};
    // 列数
    size_t m_col{0};
    // 总数据量(个数) m_row * m_col
    size_t m_total{0};

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
        if (this != &rh)
        {  // 需添加此检查
            release();
            m_row = rh.m_row;
            m_col = rh.m_col;
            m_data = rh.m_data;  // 仍存在浅拷贝问题
        }
        return *this;
    }

    /// <summary>
    /// 加运算符
    /// </summary>
    /// <param name="rh"></param>
    matrix<T> operator+=(const matrix<T> &rh)
    {
        if (rh.m_row != m_row || rh.m_col != m_col)
        {
            throw std::runtime_error("矩阵大小不匹配");
        }
        size_t i = 0;
        while (i < m_total)
        {
            thi->m_data[i] += rh.m_data[i];
            i++;
        }

        return *this;
    }

    matrix<T> operator+(const matrix<T> &rh) const
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

    matrix<T> operator+=(const T &rh)
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
        return *this;
    }

    matrix<T> operator+(const T &rh) const
    {
        matrix<T> ret;
        ret.create(m_row, m_col);
        size_t i = 0;
        while (i < m_total)
        {
            ret.m_data[i] = this->m_data[i] + rh;
        }
        return ret;
    }

    /// <summary>
    /// 减运算符
    /// </summary>
    /// <param name="rh"></param>
    matrix<T> operator-=(const matrix<T> &rh)
    {
        if (rh.m_row != m_row || rh.m_col != m_col)
        {
            throw std::runtime_error("矩阵大小不匹配");
        }
        size_t i = 0;
        while (i < m_total)
        {
            thi->m_data[i] -= rh.m_data[i];
            i++;
        }

        return *this;
    }

    matrix<T> operator-(const matrix<T> &rh)
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
            *p = *pl - *pr;
            p++;
            pl++;
            pr++;
        }
        return ret;
    }

    matrix<T> operator-=(const T &rh)
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
        return *this;
    }

    matrix<T> operator-(const T &rh) const
    {
        matrix<T> ret;
        ret.create(m_row, m_col);
        size_t i = 0;
        while (i < m_total)
        {
            ret.m_data[i] = this->m_data[i] - rh;
        }
        return ret;
    }

    /// <summary>
    /// 乘运算符
    /// </summary>
    /// <param name="rh"></param>

    matrix<T> operator*(const matrix<T> &rh) const
    {
        // 维度检查
        if (m_col != rh.m_row)
        {
            throw std::invalid_argument("矩阵维度不匹配：" + std::to_string(m_row) + "x" + std::to_string(m_col) + " * " + std::to_string(rh.m_row) + "x" + std::to_string(rh.m_col));
        }

        matrix<T> result;
        if (!result.create(m_row, rh.m_col))
        {
            throw std::runtime_error("结果矩阵创建失败");
        }

        // 三重循环计算
        for (size_t i = 0; i < m_row; ++i)
        {
            for (size_t j = 0; j < rh.m_col; ++j)
            {
                T sum = static_cast<T>(0);
                for (size_t k = 0; k < m_col; ++k)
                {
                    sum += m_data[i * m_col + k] * rh.m_data[k * rh.m_col + j];
                }
                result.m_data[i * rh.m_col + j] = sum;
            }
        }

        return result;
    }

    // 原地乘法运算符重载
    // matrix<T> &operator*=(const matrix<T> &rh)
    //{
    //    *this = *this * rh;  // 复用乘法实现
    //    return *this;
    //}

    matrix<T> operator*=(const T &rh)
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
        return *this;
    }

    matrix<T> operator*(const T &rh) const
    {
        matrix<T> ret;
        ret.create(m_row, m_col);
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                ret.m_data[i] = this->data[i] * rh;
                i++;
            }
        }
        return ret;
    }

    /// <summary>
    /// 除运算符
    /// </summary>
    /// <param name="rh"></param>
    matrix<T> operator/=(const T &rh)
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

    matrix<T> operator/(const T &v) const
    {
        matrix<T> ret;
        ret.create(m_row, m_col);
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                ret.m_data[i] = this->data[i] * rh;
                i++;
            }
        }
        return ret;
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

    /// <summary>
    /// 复制数据内容到新的指针地址
    /// </summary>
    /// <returns></returns>
    matrix<T> copy() const
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

    /// <summary>
    /// 判断矩阵是否为空
    /// </summary>
    /// <returns></returns>
    bool empty() const
    {
        return m_data == nullptr || m_row == 0 || m_col == 0;
    }

    bool valid() const
    {
        return m_data != nullptr && m_row > 0 && m_col > 0;
    }

    /// <summary>
    /// 将U数据类型的矩阵赋值给T数据类型的矩阵, 输入的矩阵会注销掉
    /// </summary>
    /// <typeparam name="U"></typeparam>
    /// <param name="rh"></param>
    /// <returns></returns>
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    void cast(const matrix<U> &rh)
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
    }

        /// <summary>
    /// 返回data的首地址
    /// </summary>
    /// <returns></returns>
    T *data() const
    {
        return m_data;
    }

    /// <summary>
    /// 缩放
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <param name="dst_row"></param>
    /// <param name="dst_col"></param>
    /// <param name="flag"></param>
    /// <returns></returns>
    template <typename T>
    matrix<T> resize(const size_t &row, const size_t &col, const IpolMethod &flag = IpolMethod::INTER_NEAREST) const
    {
        if (flag == INTER_LINEAR)
        {
            return inter_bilinear(row, col);
        }

        return inter_nearest(row, col);
    }

  private:
    template <typename T>
    matrix<T> inter_nearest(const size_t &row, const size_t &col) const
    {
        matrix<T> ret;
        if (!ret.create(row, col))
        {
            return ret;
        }

        const double x_ratio = static_cast<double>(m_col - 1) / col;
        const double y_ratio = static_cast<double>(m_row - 1) / row;

        for (size_t r = 0; r < row; ++r)
        {
            for (size_t c = 0; c < col; ++c)
            {
                // 计算源矩阵坐标（四舍五入取整）
                const size_t src_x = static_cast<size_t>(std::round(c * x_ratio));
                const size_t src_y = static_cast<size_t>(std::round(r * y_ratio));

                // 边界保护（防止越界）
                const size_t x = std::min(src_x, m_col - 1);
                const size_t y = std::min(src_y, m_row - 1);

                ret[r][c] = m_data[y * m_col + x];
            }
        }
        return ret;
    }

    template <typename T>
    matrix<T> inter_bilinear(const size_t &row, const size_t &col) const
    {
        matrix<T> ret;
        if (!ret.create(row, col))
        {
            return ret;
        }

        const double x_ratio = (m_col > 1) ? (m_col - 1) / static_cast<double>(col) : 0;
        const double y_ratio = (m_row > 1) ? (m_row - 1) / static_cast<double>(row) : 0;

        for (size_t r = 0; r < row; ++r)
        {
            for (size_t c = 0; c < col; ++c)
            {
                // 计算源矩阵坐标（浮点插值）
                const double src_x = c * x_ratio;
                const double src_y = r * y_ratio;

                // 获取四个邻近点坐标
                const size_t x0 = static_cast<size_t>(std::floor(src_x));
                const size_t x1 = std::min(x0 + 1, m_col - 1);
                const size_t y0 = static_cast<size_t>(std::floor(src_y));
                const size_t y1 = std::min(y0 + 1, m_row - 1);

                // 计算插值权重
                const double dx = src_x - x0;
                const double dy = src_y - y0;

                // 双线性插值公式
                T val = static_cast<T>((1 - dx) * (1 - dy) * at(y0, x0) + dx * (1 - dy) * at(y0, x1) + (1 - dx) * dy * at(y1, x0) + dx * dy * at(y1, x1));

                ret[r][c] = val;
            }
        }
        return ret;
    }


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

}  // namespace su

#endif  // SILLY_UTILS_SILLY_MATRIX_H