/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_rect.cpp
 * @description: silly_rect 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_RECT_H
#define SILLY_RECT_H

#include <geo/common/silly_point.h>
class silly_rect  // 普通坐标点
{
  public:
    silly_rect() = default;
    ~silly_rect() = default;

    silly_rect(const double& l, const double& t, const double& r, const double& b)
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

    silly_rect& operator=(const silly_rect& rect) = default;

    bool operator==(const silly_rect& rect) const
    {
        bool result = true;
        result &= std::abs(rect.min.x - this->min.x) <= SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) <= SU_EPSILON;
        result &= std::abs(rect.max.x - this->max.x) <= SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) <= SU_EPSILON;
        return result;
    }

    bool operator!=(const silly_rect& rect) const
    {
        bool result = true;
        result &= std::abs(rect.min.x - this->min.x) > SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) > SU_EPSILON;
        result &= std::abs(rect.max.x - this->max.x) > SU_EPSILON;
        result &= std::abs(rect.max.y - this->max.y) > SU_EPSILON;
        return result;
    }

    /// 两个矩形框是否相交
    bool intersect(const silly_rect& rh) const
    {
        // 判断两个矩形是否有交集
        bool no_overlap = rh.max.x <= min.x ||  // rh 在左边
                          rh.min.x >= max.x ||  // rh 在右边
                          rh.max.y <= min.y ||  // rh 在上边
                          rh.min.y >= max.y;    // rh 在下边

        return !no_overlap;
    }

    /// 相交区域
    silly_rect intersection(const silly_rect& rh) const
    {
        silly_rect ret;
        if (!this->intersect(rh))
        {
            // 返回空矩形或抛出异常
            return silly_rect{0, 0, 0, 0};
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

    bool contain(const silly_point& p, const bool& containBoundary = true) const
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

    silly_rect MBR(const silly_rect& rh) const
    {
        silly_rect ret;
        ret.min.x = std::min(min.x, rh.min.x);
        ret.max.y = std::max(max.y, rh.max.y);
        ret.max.x = std::max(max.x, rh.max.x);
        ret.min.y = std::min(min.y, rh.min.y);
        return ret;
    }

  public:
    silly_point min;
    silly_point max;
};
using silly_geo_rect = silly_rect;

#endif //SILLY_RECT_H
