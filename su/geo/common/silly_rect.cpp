/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suRect.c
 * @description: suRect实现
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#include "silly_rect.h"
void suRect::correct()
{
    if (min.x > max.x)
    {
        std::swap(min.x, max.x);
    }
    if (min.y > max.y)
    {
        std::swap(min.y, max.y);
    }
}
bool suRect::valid() const
{
    return min.x < max.x && min.y < max.y;
}

bool suRect::samewith(const suRect& rh, const double& eps) const
{
    bool result = true;
    result &= std::abs(rh.min.x - this->min.x) <= eps;
    result &= std::abs(rh.max.y - this->max.y) <= eps;
    result &= std::abs(rh.max.x - this->max.x) <= eps;
    result &= std::abs(rh.max.y - this->max.y) <= eps;
    return result;
}
suRect suRect::expand(const double& scale) const
{
    suRect ret;
    double w = (max.x - min.x);
    double h = (max.y - min.y);

    ret.min.x = min.x - scale * w;
    ret.min.y = min.y - scale * h;
    ret.max.x = max.x + scale * w;
    ret.max.y = max.y + scale * h;

    return ret;
}
bool suRect::intersect(const suRect& rh, const bool& containBoundary) const
{
    // 判断两个矩形是否有交集
    bool no_overlap = false;
    if (containBoundary)
    {
        no_overlap = (max.x < rh.min.x ||  // A在B的左边
                      min.x > rh.max.x ||  // A在B的右边
                      max.y < rh.min.y ||  // A在B的下边
                      min.y > rh.max.y);   // A在B的上边
    }
    else
    {
        no_overlap = rh.max.x <= min.x ||  // rh 在左边
                     rh.min.x >= max.x ||  // rh 在右边
                     rh.max.y <= min.y ||  // rh 在上边
                     rh.min.y >= max.y;    // rh 在下边
    }

    return !no_overlap;
}
suRect suRect::intersection(const suRect& rh) const
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

std::string suRect::stringify(const int& precision, const char& delimiter) const
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << min.x << delimiter << max.y << delimiter << max.x << delimiter << min.y;
    return ss.str();
}
bool suRect::parse(const std::string& str, const char& delimiter)
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

bool suRect::contains(const suPoint& p, const bool& containBoundary) const
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
suRect suRect::MBR(const suRect& rh) const
{
    suRect ret;
    ret.min.x = std::min(min.x, rh.min.x);
    ret.max.y = std::max(max.y, rh.max.y);
    ret.max.x = std::max(max.x, rh.max.x);
    ret.min.y = std::min(min.y, rh.min.y);
    return ret;
}