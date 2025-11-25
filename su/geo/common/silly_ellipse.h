/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suEllipse.cpp
 * @description: 椭圆定义, 还有部分未实现
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_ELLIPSE_H
#define SILLY_ELLIPSE_H
#include <geo/common/silly_point.h>

class suEllipse
{
  public:
    suEllipse() = default;
    ~suEllipse() = default;

    suEllipse(const suPoint& p1, const suPoint& p2, const double& longAxis);

    /**
     * 有效性验证, 长轴和短轴的限制
     * @return
     */
    bool valid() const;

    /**
     * 计算面积
     * @return
     */
    double area() const;

    /**
     * 计算周长 拉马努金近似公式
     * @return
     */
    double circumference() const;

    /**
     * 短轴长 TODO:
     * @return
     */
    double MinorAxis();

    /**
     * 焦距 TODO:
     * @return
     */
    double FocalDist();

  public:
    suPoint f1;     // 焦点1
    suPoint f2;     // 焦点2
    double majorAxis = 0.0;   // 长轴长度（焦点间距离 + 椭圆上一点到两焦点距离之和）
private:
    double m_MinorAxis = 0.0;
    double m_FocalDist = 0.0;
    // mutable suPoint m_Center;
};

#endif  // SILLY_ELLIPSE_H
