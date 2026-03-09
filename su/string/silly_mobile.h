/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-04-18
 * @file: silly_mobile.h
 * @description: silly_mobile 类声明
 * @version: v1.0.1 2025-04-18 dou li yang
 */
#ifndef SILLY_MOBILE_H
#define SILLY_MOBILE_H
#include <su_macro.h>
#include <files/silly_path.h>

class suMobileDB
{
public:
    struct Info
    {
        // 代码,号段,省区,城市,服务商,区号,邮编,区划代码
        std::string prefix;   // 前缀代码
        std::string segment;  //  号段
        std::string province; // 省区
        std::string city;     // 城市
        std::string carrier;  // 服务商
        std::string area;     // 区号
        std::string postal;   // 邮编
        std::string region;   // 区划代码
    };

    bool Load(const supath& file);

    /**
     * @brief: 查询手机号码信息
     * @param: number 手机号码
     * @return: std::optional<Info> 手机号码信息
     */
    std::optional<Info> Query(const std::string& number) const;

    /**
     * @brief: 批量查询手机号码信息
     * @param: number 手机号码
     * @return: std::vector<Info> 手机号码信息
     */
    std::vector<Info> Query(const std::vector<std::string>& number) const;

    /**
     * 是否是个有效的手机号码
     * @param number
     * @return
     */
    bool IsValid(const std::string& number) const;

protected:
    std::unordered_map<std::string, Info> m_prefix2info;
    std::string m_data_time; // 数据收集时间
};

#endif  // SILLY_MOBILE_H