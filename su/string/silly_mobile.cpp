/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-04-18
 * @file: silly_mobile.cpp
 * @description: silly_mobile实现
 * @version: v1.0.1 2025-04-18 dou li yang
 */
#include "silly_mobile.h"

bool suMobileDB::Load(const supath& file)
{
    return false;
}

std::optional<suMobileDB::Info> suMobileDB::Query(const std::string& number) const
{
    return std::nullopt;
}

std::vector<suMobileDB::Info> suMobileDB::Query(const std::vector<std::string>& number) const
{
    return {};
}

bool suMobileDB::IsValid(const std::string& number) const
{
    return false;
}