/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: ColDesc.h
 * @description: ColDesc 类声明
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#ifndef SILLY_UTILS_COLDESC_H
#define SILLY_UTILS_COLDESC_H

#include <def/ColType.h>
namespace X
{

class ColDesc
{
  public:
    uint32_t index = 0;
    ColType type;
    std::string name;
    uint32_t len = 0;  // 0表示不限制长度
    bool key = false; // 是否为主键
};
}  // namespace X
#endif  // SILLY_UTILS_COLDESC_H
