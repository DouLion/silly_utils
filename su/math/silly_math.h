/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-03-05
 * @file: silly_math.h
 * @description: silly_math 类声明
 * @version: v1.0.1 2025-03-05 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_MATH_H
#define SILLY_UTILS_SILLY_MATH_H
#include <su_marco.h>

class suMath
{
public:
    template <typename T>
    static T median(std::vector<T> data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Vector is empty");
        }
        std::sort(data.begin(), data.end());
        if (data.size() % 2 == 0)
        {
            return (data[data.size() / 2 - 1] + data[data.size() / 2]) / 2;
        }
        return data[data.size() / 2];
    }

    template <typename T>
    static T max(const std::vector<T>& data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Vector is empty");
        }
        return *std::max_element(data.begin(), data.end());
    }

    template <typename T>
    static T min(const std::vector<T>& data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Vector is empty");
        }
        return *std::min_element(data.begin(), data.end());
    }

    template <typename T>
    static T average(const std::vector<T>& data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Vector is empty");
        }
        T sum = std::accumulate(data.begin(), data.end(), 0);
        return sum / data.size();
    }

    template <typename T>
    static T variance(const std::vector<T>& data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Vector is empty");
        }
        T mean = average(data);
        T sq_sum = std::inner_product(data.begin(),
                                      data.end(),
                                      data.begin(),
                                      T(0.0)  // 显式初始化为浮点类型
        );
        return sq_sum / data.size();
    }

    template <typename T>
    static T std_deviation(std::vector<T> data)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Vector is empty");
        }
        T mean = average(data);
        T sq_sum = std::inner_product(data.begin(),
                                      data.end(),
                                      data.begin(),
                                      T(0.0)  // 显式初始化为浮点类型
        );
        T variance = sq_sum / data.size();  // 先计算方差
        return std::sqrt(variance);
    }

    template <typename T>
    static T MSE(std::vector<T> data)
    {
        throw std::runtime_error("Not implemented");
        /*if (data.empty())
        {
            throw std::invalid_argument("Vector is empty");
        }
        T mean = average(data);
        T sq_sum = std::inner_product(
            data.begin(), data.end(),
            data.begin(),
            T(0.0)  // 显式初始化为浮点类型
        );
        return sq_sum / data.size();*/
    }

    template <typename T>
    static T RMSE(std::vector<T> data)
    {
        throw std::runtime_error("Not implemented");
        /*if (data.empty())
        {
            throw std::invalid_argument("Vector is empty");
        }
        T mean = average(data);
        T sq_sum = std::inner_product(
            data.begin(), data.end(),
            data.begin(),
            T(0.0)  // 显式初始化为浮点类型
        );
        return std::sqrt(sq_sum / data.size());*/
    }
};
#endif  // SILLY_UTILS_SILLY_MATH_H
