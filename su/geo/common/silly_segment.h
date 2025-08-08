/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_segment.cpp
 * @description: silly_segment 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_SEGMENT_H
#define SILLY_SEGMENT_H
#include <geo/common/silly_point.h>

class silly_segment
{
  public:
    silly_segment() = default;
    ~silly_segment() = default;
    silly_segment(silly_point _p0, silly_point _p1)
    {
        p0 = _p0;
        p1 = _p1;
    }

  public:
    silly_point p0;
    silly_point p1;
};

class silly_segmentZ
{
  public:
    silly_segmentZ() = default;
    ~silly_segmentZ() = default;
    silly_segmentZ(const silly_pointZ& _p0, const silly_pointZ& _p1)
    {
        p0 = _p0;
        p1 = _p1;
    }

    silly_segmentZ& operator=(const silly_segmentZ& rh)
    {
        this->p0 = rh.p0;
        this->p1 = rh.p1;
        return *this;
    }

  public:
    silly_pointZ p0;
    silly_pointZ p1;
};

#endif  // SILLY_SEGMENT_H
