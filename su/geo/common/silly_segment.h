/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suSegment.cpp
 * @description: suSegment 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_SEGMENT_H
#define SILLY_SEGMENT_H
#include <geo/common/silly_point.h>

class suSegment
{
  public:
    suSegment() = default;
    ~suSegment() = default;
    suSegment(suPoint _p0, suPoint _p1)
    {
        p0 = _p0;
        p1 = _p1;
    }

  public:
    suPoint p0;
    suPoint p1;
};

class suSegmentZ
{
  public:
    suSegmentZ() = default;
    ~suSegmentZ() = default;
    suSegmentZ(const suPointZ& _p0, const suPointZ& _p1)
    {
        p0 = _p0;
        p1 = _p1;
    }

    suSegmentZ& operator=(const suSegmentZ& rh)
    {
        this->p0 = rh.p0;
        this->p1 = rh.p1;
        return *this;
    }

  public:
    suPointZ p0;
    suPointZ p1;
};

#endif  // SILLY_SEGMENT_H
