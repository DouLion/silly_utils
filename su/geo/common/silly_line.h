/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suLine.cpp
 * @description: suLine 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_LINE_H
#define SILLY_LINE_H
#include <geo/common/silly_point.h>
#include <geo/common/silly_segment.h>

class suLine : public suVecTemplate<suPoint>
{
  public:
    /**
     * 构建等间距的折线
     * @param dist
     * @return
     */
    suLine equidistant(const double& dist) const;

    /**
     * 总线长度
     * @return
     */
    double length() const;

    /**
     * 折线是否与另一条折线相交
     * @param rh
     * @return
     */
    bool intersect(const suLine& rh) const;

    /**
     * 与另一条折线的交点
     * @param rh
     * @return
     */
    std::vector<suPoint> intersection(const suLine& rh) const;
};

/****************************************/
/// 多线
/****************************************/
class suMultiLine : public suVecTemplate<suLine>
{
  public:

};
#endif  // SILLY_LINE_H
