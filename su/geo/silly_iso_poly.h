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
#ifndef SILLY_UTILS_SILLY_ISO_POLY_H
#define SILLY_UTILS_SILLY_ISO_POLY_H
#include <geo/silly_geo.h>
namespace silly
{
namespace geo
{
class iso_poly
{
  public:
    iso_poly() = default;
    ~iso_poly() = default;
    bool read(const std::string& u8file, silly_multi_poly& mpoly);
    bool write(const std::string& u8file, const silly_multi_poly& mpoly, const int& precision = 8);

  private:
};
}  // namespace geo
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_ISO_POLY_H
