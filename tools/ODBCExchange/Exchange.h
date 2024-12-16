/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-14
 * @file: Exchange.h
 * @description: Exchange 类声明
 * @version: v1.0.1 2024-12-14 dou li yang
 */
#ifndef SILLY_UTILS_EXCHANGE_H
#define SILLY_UTILS_EXCHANGE_H
#include "def/Table.h"

namespace X
{

class PullOpt
{
  public:
    std::string table;
    std::string sql;
    silly_otl  otl;
};

class PushOpt
{
  public:
    std::string table;
    std::string sql;
    silly_otl  otl;
};

class Exchange
{
  public:
    Exchange();
    ~Exchange();

    // 读取数据库服务器数据
    bool Pull(PullOpt& opt);

    // 推送数据到数据库服务器
    bool Push(PushOpt& opt);


  private:
};

}  // namespace X

#endif  // SILLY_UTILS_EXCHANGE_H
