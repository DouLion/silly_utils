/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: ColType.h
 * @description: ColType 类声明
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#ifndef SILLY_UTILS_COLTYPE_H
#define SILLY_UTILS_COLTYPE_H

#include <database/otl/silly_otl.h>
namespace X
{

typedef otl_var_enum ColType;

class Binary
{
  public:
    Binary();
    ~Binary(){
        m_blob.~basic_string();
    }

  public:
    std::string m_blob;
};

class LString
{
  public:
    LString();
    ~LString()
    {
        m_str.~basic_string();
    }

  public:
    std::string m_str;
};
}  // namespace X

#endif  // SILLY_UTILS_COLTYPE_H
