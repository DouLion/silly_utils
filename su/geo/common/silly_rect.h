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
    suPoint min = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
    suPoint max = {-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()};

    suRect() = default;
    suRect(const suPoint& bl, const suPoint& tr)
        : min(bl), max(tr) {
        correct();
    }

    suRect(const double& l, const double& t, const double& r, const double& b)
    {
        min.x = l;
        max.y = t;
        max.x = r;
        min.y = b;
    }

    double area() const
    {
        if (!valid())
        {
            return  0;
        }

        return (max.x - min.x) * (max.y - min.y);
    }

    /**
     * 简单矫正, 保证 min < max
     */
    void correct();

    /**
     * 是否有效, min.x < max.x && min.y < max.y;
     * @return
     */
    bool valid() const;

    /**
     * 两个矩形框是否相似
     * @param rh
     * @param eps 差异容差
     * @return
     */
    bool samewith(const suRect& rh, const double& eps = SU_EPSILON) const;

    /**
     * 外扩一定比例
     * 如scale = 0.1, 上下左右均扩充现有宽高的0.1
     * @param scale 比例
     * @return
     */
    suRect expand(const double& scale) const;

    /**
     * 两个矩形框是否相交
     * @param rh
     * @param containBoundary 边重合仍认为是相交
     * @return
     */
    bool intersect(const suRect& rh, const bool& containBoundary = false) const;

    /**
     * 两个矩形的相交rect
     * @param rh
     * @return
     */
    suRect intersection(const suRect& rh) const;

    /**
     * 是否包含点
     * @param p
     * @param containBoundary
     * @return
     */
    bool contains(const suPoint& p, const bool& containBoundary = true) const;

    /**
     * 转为字符串
     * @param precision 保留小数位
     * @param delimiter 分隔符号
     * @return
     */
    std::string stringify(const int& precision = 8, const char& delimiter = ',') const;

    /**
     * 从字符串加载
     * @param str 输入字符串, 默认格式 left, top, right, bottm
     * @param delimiter 分隔符号
     * @return
     */
    bool parse(const std::string& str, const char& delimiter = ',');


    /**
     * 两个矩形的最小包围框
     * @param rh 比例
     * @return
     */
    suRect MBR(const suRect& rh) const;
};

#endif  // SILLY_RECT_H
