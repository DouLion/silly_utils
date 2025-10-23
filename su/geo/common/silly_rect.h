/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suRect.cpp
 * @description: suRect 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_RECT_H
#define SILLY_RECT_H

#include <geo/common/silly_point.h>
class suRect  // 普通坐标点
{
public:
    suPoint min;
    suPoint max;
  public:
    suRect() = default;
    ~suRect() = default;

    suRect(const double& l, const double& t, const double& r, const double& b)
    {
        min.x = l;
        max.y = t;
        max.x = r;
        min.y = b;
    }

    void correct()
    {
        if (min.x > max.x)
        {
            std::swap(min.x, max.x);  // 使用 std::swap 简化交换操作
        }
        if (min.y > max.y)
        {
            std::swap(min.y, max.y);  // 使用 std::swap 简化交换操作
        }
    }

    suRect& operator=(const suRect& rect) = default;

    bool operator==(const suRect& rect) const
    {
        bool result = true;
        result &= std::abs(rect.min.x - this->min.x) <= SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) <= SU_EPSILON;
        result &= std::abs(rect.max.x - this->max.x) <= SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) <= SU_EPSILON;
        return result;
    }

    bool operator!=(const suRect& rect) const
    {
        bool result = true;
        result &= std::abs(rect.min.x - this->min.x) > SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) > SU_EPSILON;
        result &= std::abs(rect.max.x - this->max.x) > SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) > SU_EPSILON;
        return result;
    }

    /**
     * 外扩一定比例
     * 如scale = 0.1, 上下左右均扩充现有宽高的0.1
     * @param scale 比例
     * @return
     */
    suRect expand(const double& scale) const
    {
        suRect ret;
        double w = (max.x - min.x);
        double h = (max.x - min.x);
        ret.min.x = min.x + scale * w;
        ret.min.y = min.y + scale * h;
        ret.max.x = max.x + scale * w;
        ret.max.y = max.y + scale * h;
        return ret;
    }
    /// 两个矩形框是否相交
    bool intersect(const suRect& rh) const
    {
        // 判断两个矩形是否有交集
        bool no_overlap = rh.max.x <= min.x ||  // rh 在左边
                          rh.min.x >= max.x ||  // rh 在右边
                          rh.max.y <= min.y ||  // rh 在上边
                          rh.min.y >= max.y;    // rh 在下边

        return !no_overlap;
    }

    /// 相交区域
    suRect intersection(const suRect& rh) const
    {
        suRect ret;
        if (!this->intersect(rh))
        {
            // 返回空矩形或抛出异常
            return suRect{0, 0, 0, 0};
        }
        ret.min.x = std::max(min.x, rh.min.x);
        ret.min.y = std::max(min.y, rh.min.y);
        ret.max.x = std::min(max.x, rh.max.x);
        ret.max.y = std::min(max.y, rh.max.y);

        return ret;
    }

    std::string stringify(const int precision = 8, const char& delimiter = ',') const
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << min.x << delimiter << max.y << delimiter << max.x << delimiter << min.y;
        return ss.str();
    }

    bool destringify(const std::string& str, const char& delimiter = ',')
    {
        auto split = [&delimiter](const std::string& str) {
            std::vector<std::string> tokens;
            std::string token;
            std::stringstream ss(str);
            while (std::getline(ss, token, delimiter))
            {
                tokens.push_back(token);
            }
            return tokens;
        };
        std::vector<std::string> tokens = split(str);
        if (tokens.size() == 4)
        {
            min.x = std::stod(tokens[0]);
            max.y = std::stod(tokens[1]);
            max.x = std::stod(tokens[2]);
            min.y = std::stod(tokens[3]);
            return true;
        }
        return false;
    }

    bool contain(const suPoint& p, const bool& containBoundary = true) const
    {
        if (!containBoundary)
        {
            if (p.x > min.x && p.x < max.x && p.y > min.y && p.y < max.y)
            {
                return true;
            }
        }
        else
        {
            if (p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y)
            {
                return true;
            }
        }
        return false;
    }

    suRect MBR(const suRect& rh) const
    {
        suRect ret;
        ret.min.x = std::min(min.x, rh.min.x);
        ret.max.y = std::max(max.y, rh.max.y);
        ret.max.x = std::max(max.x, rh.max.x);
        ret.min.y = std::min(min.y, rh.min.y);
        return ret;
    }


};
using silly_geo_rect = suRect;
using silly_rect = suRect;

#endif  // SILLY_RECT_H
