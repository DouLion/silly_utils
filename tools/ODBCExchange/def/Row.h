/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-27
 * @file: Row.h
 * @description: Row 类声明
 * @version: v1.0.1 2024-12-27 dou li yang
 */
#ifndef SILLY_UTILS_ROW_H
#define SILLY_UTILS_ROW_H
#include <def/ColVal.h>
namespace X
{
class Row
{
  public:

    Row();
    Row(const size_t& size);
    void clear();
    void resize(const size_t& size);

    bool FromBin(const std::string& bin);
    std::string AsBin() const;
  public:
    std::vector<ColVal> items;
};
}
#endif  // SILLY_UTILS_ROW_H
