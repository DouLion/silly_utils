/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-11-04
 * @file: silly_cairo_fill_pattern.h
 * @description: silly_cairo_fill_pattern 类声明
 * @version: v1.0.1 2024-11-04 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_CAIRO_PATTERN_H
#define SILLY_UTILS_SILLY_CAIRO_PATTERN_H
#include <cairo/cairo.h>
class silly_cairo_fill_pattern
{
  public:
    silly_cairo_fill_pattern();
    ~silly_cairo_fill_pattern();

    cairo_pattern_t* pattern();

  private:

    cairo_pattern_t* m_pattern = nullptr;
};

#endif  // SILLY_UTILS_SILLY_CAIRO_PATTERN_H
