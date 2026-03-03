/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-19
 * @file: silly_iso_poly.h
 * @description: 等值面使用的一种txt文件格式的矢量面
 * @version: v1.0.1 2024-12-19 dou li yang
 */
#ifndef SILLY_ISO_POLY_H
#define SILLY_ISO_POLY_H
#include <geo/silly_geo.h>
#include <files/silly_file.h>
class suIsoPoly
{
  public:
    suIsoPoly() = default;
    ~suIsoPoly() = default;
    static bool read(const suPath& file, suMultiPoly& mpoly);
    static bool write(const suPath& file, const suMultiPoly& mpoly, const int& precision = 8);

  private:
};

#endif  // SILLY_ISO_POLY_H
