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

#ifndef SILLY_MATRIX_H
#define SILLY_MATRIX_H
#include <su_macro.h>
template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class suMatrix
{
    /// <summary>
    /// 这个目前是线程不安全的,使用时需要注意
    /// </summary>
  public:
    enum eIpolMethod : int
    {
        /** nearest neighbor interpolation 最临近插值 */
        INTER_NEAREST = 0,
        /** bilinear interpolation 二次性插值 */
        INTER_LINEAR = 1,
    };
    enum eBorderType : int
    {
        REFLECT,    // 镜像反射边界
        REPLICATE,  // 复制边界值
        ZERO        // 零填充
    };

  protected:
    T *m_data{nullptr};
    // 行数
    size_t m_rows{0};
    // 列数
    size_t m_cols{0};
    // 总数据量(个数) m_rows * m_cols
    size_t m_total{0};
    static constexpr  size_t MAX_RC = 100000000;

  public:
    /// <summary>
    /// 创建一个指定大小的二维数据
    /// </summary>
    /// <param name="rows">行数</param>
    /// <param name="cols">列数</param>
    /// <param name="reset">true: 重新初始化为0 false: 不做任何操作,并且返回false</param>
    /// <returns></returns>
    bool create(const size_t &rows, const size_t &cols, bool reset = false)
    {
        if (!(rows > 0 && rows < MAX_RC &&  cols > 0 && cols < MAX_RC))
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
        m_rows = rows;
        m_cols = cols;
        m_total = m_rows * m_cols;

        m_data = (T *)malloc(m_total * sizeof(T));
        if (!m_data)
        {
            return false;
        }
        memset(m_data, 0, m_total * sizeof(T));

        return true;
    }
    bool create(const size_t &rows, const size_t &cols, const std::vector<T>& data)
    {
        if (!(rows > 0 && rows < MAX_RC &&  cols > 0 && cols < MAX_RC))
        {
            return false;
        }
        if (data.size() != rows * cols)
        {
            return false;
        }
        m_rows = rows;
        m_cols = cols;
        m_total = m_rows * m_cols;

        m_data = (T *)malloc(m_total * sizeof(T));
        if (!m_data)
        {
            return false;
        }
        std::memcpy(m_data, data.data(), m_total * sizeof(T));
        return true;
    }

    /// <summary>
    /// 仅传递数据指针
    /// </summary>
    /// <param name="rh"></param>
    /// <returns></returns>
    suMatrix<T> &operator=(const suMatrix<T> &rh)
    {
        if (this != &rh)
        {  // 需添加此检查
            release();
            m_total = rh.m_total;
            m_rows = rh.m_rows;
            m_cols = rh.m_cols;
            m_data = rh.m_data;  // 仍存在浅拷贝问题
        }
        return *this;
    }

    /// <summary>
    /// 加运算符
    /// </summary>
    /// <param name="rh"></param>
    suMatrix<T> operator+=(const suMatrix<T> &rh)
    {
        if (rh.m_rows != m_rows || rh.m_cols != m_cols)
        {
            throw std::runtime_error("矩阵大小不匹配");
        }
        size_t i = 0;
        while (i < m_total)
        {
            this->m_data[i] += rh.m_data[i];
            i++;
        }

        return *this;
    }

    suMatrix<T> operator+(const suMatrix<T> &rh) const
    {
        suMatrix<T> ret;
        if (rh.m_rows != m_rows || rh.m_cols != m_cols)
        {
            throw std::runtime_error("矩阵大小不匹配");
        }
        ret.create(m_rows, m_cols);
        size_t i = 0;
        while (i < m_total)
        {
            ret.m_data[i] = this->m_data[i] + rh.m_data[i];
            i++;
        }
        return ret;
    }

    suMatrix<T> operator+=(const T &rh)
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

    suMatrix<T> operator+(const T &rh) const
    {
        suMatrix<T> ret;
        ret.create(m_rows, m_cols);
        size_t i = 0;
        while (i < m_total)
        {
            ret.m_data[i] = this->m_data[i] + rh;
            i++;
        }
        return ret;
    }

    /// <summary>
    /// 减运算符
    /// </summary>
    /// <param name="rh"></param>
    suMatrix<T> operator-=(const suMatrix<T> &rh)
    {
        if (rh.m_rows != m_rows || rh.m_cols != m_cols)
        {
            throw std::runtime_error("矩阵大小不匹配");
        }
        size_t i = 0;
        while (i < m_total)
        {
            this->m_data[i] -= rh.m_data[i];
            i++;
        }

        return *this;
    }

    suMatrix<T> operator-(const suMatrix<T> &rh)
    {
        suMatrix<T> ret;
        if (rh.m_rows != m_rows || rh.m_cols != m_cols)
        {
            throw std::runtime_error("矩阵大小不匹配");
        }
        ret.create(m_rows, m_cols);
        size_t i = 0;
        while (i < m_total)
        {
            ret.m_data[i] = this->m_data[i] - rh.m_data[i];
            i++;
        }
        return ret;
    }

    suMatrix<T> operator-=(const T &rh)
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

    suMatrix<T> operator-(const T &rh) const
    {
        suMatrix<T> ret;
        ret.create(m_rows, m_cols);
        size_t i = 0;
        while (i < m_total)
        {
            ret.m_data[i] = this->m_data[i] - rh;
            i++;
        }
        return ret;
    }

    /// <summary>
    /// 乘运算符
    /// </summary>
    /// <param name="rh"></param>

    suMatrix<T> operator*(const suMatrix<T> &rh) const
    {
        // 维度检查
        if (m_cols != rh.m_rows)
        {
            throw std::invalid_argument("矩阵维度不匹配：" + std::to_string(m_rows) + "x" + std::to_string(m_cols) + " * " + std::to_string(rh.m_rows) + "x" + std::to_string(rh.m_cols));
        }

        suMatrix<T> ret;
        if (!ret.create(m_rows, rh.m_cols))
        {
            throw std::runtime_error("结果矩阵创建失败");
        }

        // 三重循环计算
        for (size_t i = 0; i < m_rows; ++i)
        {
            for (size_t j = 0; j < rh.m_cols; ++j)
            {
                T sum = static_cast<T>(0);
                for (size_t k = 0; k < m_cols; ++k)
                {
                    sum += m_data[i * m_cols + k] * rh.m_data[k * rh.m_cols + j];
                }
                ret.m_data[i * rh.m_cols + j] = sum;
            }
        }

        return ret;
    }

    // 原地乘法运算符重载
    // matrix<T> &operator*=(const matrix<T> &rh)
    //{
    //    *this = *this * rh;  // 复用乘法实现
    //    return *this;
    //}

    suMatrix<T> operator*=(const T &rh)
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

    suMatrix<T> operator*(const T &rh) const
    {
        suMatrix<T> ret;
        ret.create(m_rows, m_cols);
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                ret.m_data[i] = this->m_data[i] * rh;
                i++;
            }
        }
        return ret;
    }

    /// <summary>
    /// 除运算符
    /// </summary>
    /// <param name="rh"></param>
    suMatrix<T> operator/=(const T &rh)
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
        return *this;
    }

    suMatrix<T> operator/(const T &rh) const
    {
        suMatrix<T> ret;
        ret.create(m_rows, m_cols);
        if (m_data)
        {
            size_t i = 0;
            while (i < m_total)
            {
                ret.m_data[i] = this->m_data[i] / rh;
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
            return m_data + (r * m_cols);
        }
        return nullptr;
    }

    T *operator[](const size_t &r) const
    {
        if (m_data)
        {
            return m_data + (r * m_cols);
        }
        return nullptr;
    }
    /// <summary>
    /// 获取值
    /// </summary>
    /// <param name="r"></param>
    /// <param name="c"></param>
    /// <returns></returns>
    T &at(size_t r, size_t c) const
    {
        return m_data[r * m_cols + c];
    }

    /// <summary>
    /// 双线性插值
    /// </summary>
    /// <param name="frow"></param>
    /// <param name="fcol"></param>
    /// <returns></returns>
    T bilinear(const double &frow, const double &fcol) const
    {
        // 边界检查，如果超出范围，可以 clamp 或 return 0.0，这里我们做简单的 clamp
        double px = std::clamp(fcol, 0.0, static_cast<double>(m_cols - 1));
        double py = std::clamp(frow, 0.0, static_cast<double>(m_rows - 1));

        const auto x1 = static_cast<size_t>(px);
        const auto y1 = static_cast<size_t>(py);

        const size_t x2 = std::min(x1 + 1, m_cols - 1);
        const size_t y2 = std::min(y1 + 1, m_rows - 1);

        const double fx = px - x1;
        const double fy = py - y1;

        // 获取四个点的值
        const double Q11 = m_data[y1 * m_cols + x1];
        const double Q21 = m_data[y1 * m_cols + x2];
        const double Q12 = m_data[y2 * m_cols + x1];
        const double Q22 = m_data[y2 * m_cols + x2];

        // 双线性插值公式
        const double R1 = Q11 * (1 - fx) + Q21 * fx;  // 在 y1 行，x 方向插值
        const double R2 = Q12 * (1 - fx) + Q22 * fx;  // 在 y2 行，x 方向插值
        double P = R1 * (1 - fy) + R2 * fy;           // 在 y 方向插值

        return static_cast<T>(P);
    }

    /// <summary>
    /// 双三次插值
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    T bicubic(const double &x, const double &y) const
    {
        // clamp 到合法范围
        double px = std::clamp(x, 0.0, static_cast<double>(m_cols - 1));
        double py = std::clamp(y, 0.0, static_cast<double>(m_rows - 1));

        size_t x0 = static_cast<size_t>(std::floor(px));
        size_t y0 = static_cast<size_t>(std::floor(py));

        double dx = px - x0;
        double dy = py - y0;

        double result = 0.0;
        double weight_sum = 0.0;

        // 遍历 4x4 邻域
        for (int m = -1; m <= 2; ++m)
        {
            for (int n = -1; n <= 2; ++n)
            {
                size_t xi = x0 + n;
                size_t yi = y0 + m;

                // 边界处理：clamp 到有效范围
                xi = std::clamp(xi, static_cast<size_t>(0), m_cols - 1);
                yi = std::clamp(yi, static_cast<size_t>(0), m_rows - 1);

                double value = m_data[yi * m_cols + xi];

                double wx = cubic_weight(n - dx);  // n - dx 是距离
                double wy = cubic_weight(m - dy);

                double w = wx * wy;
                result += value * w;
                weight_sum += w;
            }
        }

        // 避免除零（理论上 weight_sum 不会为 0）
        if (weight_sum != 0.0)
            result /= weight_sum;

        return static_cast<T>(result);
    }

    /// <summary>
    /// 复制数据内容到新的指针地址
    /// </summary>
    /// <returns></returns>
    suMatrix<T> copy() const
    {
        suMatrix<T> ret;
        ret.create(m_rows, m_cols);
        if (m_data)
        {
            memcpy(ret.m_data, m_data, m_total * sizeof(T));
        }

        return ret;
    }

    /// <summary>
    /// 矩阵掩码处理, mask为1表示有效数据，0表示无效数据
    /// </summary>
    /// <param name="mask"></param>
    /// <param name="invalid_value">设置无效数据值</param>
    bool mask(const suMatrix<uint8_t> &mask, T invalid_value)
    {
        if (mask.rows() != m_rows || mask.cols() != m_cols || !mask.data() || !m_data)
        {
            return false;
        }
        uint8_t *p_mask = mask.data();
        T *p_data = m_data;
        for (size_t i = 0; i < m_total; ++i)
        {
            if (!p_mask[i])
            {
                p_data[i] = invalid_value;
            }
        }
        return true;
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
    /// 获取矩阵中的最大值,并且返回最大值的行列号
    /// </summary>
    /// <returns></returns>
    T max(size_t &tr, size_t &tc) const
    {
        T ret = static_cast<T>(INT_MIN);
        if (m_data)
        {
            size_t i = 0;
            size_t markI = 0;
            while (i < m_total)
            {
                if (m_data[i] > ret)
                {
                    markI = i;
                    ret = m_data[i];
                }
                i++;
            }
            tr = markI / m_cols;
            tc = markI % m_cols;
            return ret;
        }
        return ret;
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
                m_data[i] = std::min(val, m_data[i]);
            }
        }
    }

    /// <summary>
    /// 相同格点取两个矩阵的最大值
    /// </summary>
    /// <param name="rh"></param>
    void max(const suMatrix<T> &rh)
    {
        if (m_data && rh.data() && rh.m_rows == m_rows && rh.m_cols == m_cols)
        {
            for (size_t i = 0; i < m_total; ++i)
            {
                m_data[i] = std::max(rh.m_data[i], m_data[i]);
            }
        }
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
    /// 获取矩阵中的最小值,并且返回最小值的行列号
    /// </summary>
    /// <returns></returns>
    T min(size_t &tr, size_t &tc) const
    {
        T ret = static_cast<T>(INT_MAX);
        if (m_data)
        {
            size_t i = 0;
            size_t markI = 0;
            while (i < m_total)
            {
                if (m_data[i] < ret)
                {
                    markI = i;
                    ret = m_data[i];
                }
                i++;
            }
            tr = markI / m_cols;
            tc = markI % m_cols;
            return ret;
        }
        return ret;
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
                m_data[i] = std::max(val, m_data[i]);
            }
        }
    }

    /// <summary>
    /// 相同格点取两个矩阵的最小值
    /// </summary>
    /// <param name="rh"></param>
    void min(const suMatrix<T> &rh)
    {
        if (m_data && rh.data() && rh.m_rows == m_rows && rh.m_cols == m_cols)
        {
            for (size_t i = 0; i < m_total; ++i)
            {
                m_data[i] = std::min(rh.m_data[i], m_data[i]);
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
            m_total = 0;
            m_rows = 0;
            m_cols = 0;
        }
    }
    const size_t rows() const
    {
        return m_rows;
    }

    const size_t cols() const
    {
        return m_cols;
    }

    /// <summary>
    /// 判断矩阵是否为空
    /// </summary>
    /// <returns></returns>
    bool empty() const
    {
        return m_data == nullptr || m_rows == 0 || m_cols == 0;
    }

    bool valid() const
    {
        return m_data != nullptr && m_rows > 0 && m_cols > 0;
    }

    /// <summary>
    /// 将U数据类型的矩阵赋值给T数据类型的矩阵, 输入的矩阵会注销掉
    /// </summary>
    /// <typeparam name="U"></typeparam>
    /// <param name="rh"></param>
    /// <returns></returns>
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    void cast(const suMatrix<U> &rh)
    {
        create(rh.rows(), rh.cols(), true);
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
    suMatrix<T> resize(const size_t &rows, const size_t &cols, const eIpolMethod &flag = eIpolMethod::INTER_NEAREST) const
    {
        if (flag == INTER_LINEAR)
        {
            return inter_bilinear_resize(rows, cols);
        }

        return inter_nearest_resize(rows, cols);
    }
    // 高斯滤波
    suMatrix<T> gaussian_blur(double sigma = 1.0, int kernel_size = -1) const
    {
        suMatrix<T> result;
        if (empty())
            return result;

        // 自动计算核大小（通常为6*sigma + 1）
        if (kernel_size <= 0)
        {
            kernel_size = static_cast<int>(std::ceil(6 * sigma)) | 1;  // 确保为奇数
        }
        if (kernel_size % 2 == 0)
            kernel_size++;  // 确保核大小为奇数

        // 创建高斯核
        std::vector<double> kernel = create_gaussian_kernel(kernel_size, sigma);

        if (!result.create(m_rows, m_cols))
        {
            return result;
        }

        // 应用高斯滤波（可分离的二维卷积）
        suMatrix<T> temp;
        temp.create(m_rows, m_cols);

        // 水平方向卷积
        const int radius = kernel_size / 2;
        for (size_t r = 0; r < m_rows; ++r)
        {
            for (size_t c = 0; c < m_cols; ++c)
            {
                double sum = 0.0;
                double weight_sum = 0.0;

                for (int k = -radius; k <= radius; ++k)
                {
                    int col_idx = static_cast<int>(c) + k;
                    if (col_idx >= 0 && col_idx < static_cast<int>(m_cols))
                    {
                        double weight = kernel[k + radius];
                        sum += weight * static_cast<double>(at(r, col_idx));
                        weight_sum += weight;
                    }
                }

                temp[r][c] = static_cast<T>(sum / weight_sum);
            }
        }

        // 垂直方向卷积
        for (size_t r = 0; r < m_rows; ++r)
        {
            for (size_t c = 0; c < m_cols; ++c)
            {
                double sum = 0.0;
                double weight_sum = 0.0;

                for (int k = -radius; k <= radius; ++k)
                {
                    int row_idx = static_cast<int>(r) + k;
                    if (row_idx >= 0 && row_idx < static_cast<int>(m_rows))
                    {
                        double weight = kernel[k + radius];
                        sum += weight * static_cast<double>(temp[row_idx][c]);
                        weight_sum += weight;
                    }
                }

                result[r][c] = static_cast<T>(sum / weight_sum);
            }
        }

        return result;
    }

    // 中值滤波
    suMatrix<T> median_filter(int kernel_size = 3) const
    {
        suMatrix<T> result;
        if (empty())
            return result;

        // 确保核大小为奇数
        if (kernel_size % 2 == 0)
            kernel_size++;
        if (kernel_size < 3)
            kernel_size = 3;

        if (!result.create(m_rows, m_cols))
        {
            return result;
        }

        const int radius = kernel_size / 2;

        for (size_t r = 0; r < m_rows; ++r)
        {
            for (size_t c = 0; c < m_cols; ++c)
            {
                // 收集邻域内的值
                std::vector<T> neighbors;
                neighbors.reserve(kernel_size * kernel_size);

                for (int i = -radius; i <= radius; ++i)
                {
                    for (int j = -radius; j <= radius; ++j)
                    {
                        int row_idx = static_cast<int>(r) + i;
                        int col_idx = static_cast<int>(c) + j;

                        if (row_idx >= 0 && row_idx < static_cast<int>(m_rows) && col_idx >= 0 && col_idx < static_cast<int>(m_cols))
                        {
                            neighbors.push_back(at(row_idx, col_idx));
                        }
                    }
                }

                // 计算中值
                if (!neighbors.empty())
                {
                    std::nth_element(neighbors.begin(), neighbors.begin() + neighbors.size() / 2, neighbors.end());
                    result[r][c] = neighbors[neighbors.size() / 2];
                }
                else
                {
                    result[r][c] = at(r, c);
                }
            }
        }

        return result;
    }

  private:
    suMatrix<T> inter_nearest_resize(const size_t &rows, const size_t &cols) const
    {
        suMatrix<T> ret;
        if (!ret.create(rows, cols))
        {
            return ret;
        }

        const double x_ratio = static_cast<double>(m_cols - 1) / cols;
        const double y_ratio = static_cast<double>(m_rows - 1) / rows;

        for (size_t r = 0; r < rows; ++r)
        {
            for (size_t c = 0; c < cols; ++c)
            {
                // 计算源矩阵坐标（四舍五入取整）
                const size_t src_x = static_cast<size_t>(std::round(c * x_ratio));
                const size_t src_y = static_cast<size_t>(std::round(r * y_ratio));

                // 边界保护（防止越界）
                const size_t x = std::min(src_x, m_cols - 1);
                const size_t y = std::min(src_y, m_rows - 1);

                ret[r][c] = m_data[y * m_cols + x];
            }
        }
        return ret;
    }

    suMatrix<T> inter_bilinear_resize(const size_t &rows, const size_t &cols) const
    {
        suMatrix<T> ret;

        // 处理空矩阵情况
        if (empty() || rows == 0 || cols == 0)
        {
            return ret;
        }

        if (!ret.create(rows, cols))
        {
            return ret;
        }

        // 处理单行/单列的特殊情况
        const double x_ratio = (m_cols > 1) ? static_cast<double>(m_cols - 1) / (cols > 1 ? cols - 1 : 1) : 0;
        const double y_ratio = (m_rows > 1) ? static_cast<double>(m_rows - 1) / (rows > 1 ? rows - 1 : 1) : 0;

        for (size_t r = 0; r < rows; ++r)
        {
            for (size_t c = 0; c < cols; ++c)
            {
                // 计算源矩阵坐标
                double src_x = (m_cols > 1) ? c * x_ratio : 0;
                double src_y = (m_rows > 1) ? r * y_ratio : 0;

                // 确保坐标在有效范围内
                src_x = std::clamp(src_x, 0.0, static_cast<double>(m_cols - 1));
                src_y = std::clamp(src_y, 0.0, static_cast<double>(m_rows - 1));

                // 获取四个邻近点坐标
                const size_t x0 = static_cast<size_t>(src_x);
                const size_t x1 = std::min(x0 + 1, m_cols - 1);
                const size_t y0 = static_cast<size_t>(src_y);
                const size_t y1 = std::min(y0 + 1, m_rows - 1);

                // 计算插值权重
                const double dx = src_x - x0;
                const double dy = src_y - y0;

                // 双线性插值（使用更精确的计算）
                double val = (1.0 - dx) * (1.0 - dy) * static_cast<double>(at(y0, x0)) + dx * (1.0 - dy) * static_cast<double>(at(y0, x1)) + (1.0 - dx) * dy * static_cast<double>(at(y1, x0)) + dx * dy * static_cast<double>(at(y1, x1));

                // 对于整数类型进行四舍五入
                if constexpr (std::is_integral_v<T>)
                {
                    ret[r][c] = static_cast<T>(std::round(val));
                }
                else
                {
                    ret[r][c] = static_cast<T>(val);
                }
            }
        }
        return ret;
    }

  private:
    // 创建高斯核
    std::vector<double> create_gaussian_kernel(int size, double sigma) const
    {
        std::vector<double> kernel(size);
        const int center = size / 2;
        double sum = 0.0;

        for (int i = 0; i < size; ++i)
        {
            double x = i - center;
            kernel[i] = std::exp(-(x * x) / (2 * sigma * sigma));
            sum += kernel[i];
        }

        // 归一化
        for (int i = 0; i < size; ++i)
        {
            kernel[i] /= sum;
        }

        return kernel;
    }

    // 边界处理函数
    int handle_border(int index, int max_index, eBorderType border_type) const
    {
        if (index >= 0 && index < max_index)
        {
            return index;
        }

        switch (border_type)
        {
            case eBorderType::REFLECT:
                if (index < 0)
                    return -index - 1;
                if (index >= max_index)
                    return 2 * max_index - index - 1;
                break;

            case eBorderType::REPLICATE:
                if (index < 0)
                    return 0;
                if (index >= max_index)
                    return max_index - 1;
                break;

            case eBorderType::ZERO:
            default:
                if (index < 0 || index >= max_index)
                    return -1;  // 特殊处理
        }

        return index;
    }

    double cubic_weight(double d) const
    {
        d = std::abs(d);
        if (d <= 1.0)
        {
            return 1.5 * d * d * d - 2.5 * d * d + 1.0;
        }
        else if (d < 2.0)
        {
            return -0.5 * d * d * d + 2.5 * d * d - 4.0 * d + 2.0;
        }
        return 0.0;
    }
};

typedef suMatrix<int> suIMatrix;
typedef suMatrix<unsigned int> suUIMatrix;
typedef suMatrix<float> suFMatrix;
typedef suMatrix<double> suDMatrix;
typedef suMatrix<short> suSMatrix;
typedef suMatrix<unsigned short> suUSMatrix;
typedef suMatrix<char> suCMatrix;
typedef suMatrix<unsigned char> suUCMatrix;
typedef suMatrix<long long> suLMatrix;
typedef suMatrix<unsigned long long> suUMatrix;

#endif  // SILLY_MATRIX_H