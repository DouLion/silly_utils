/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-18
 * @file: silly_ultimate_warn.h
 * @description: 告警内容的终极信息, 尽量包含目前已知的信息
 * @version: v1.0.1 2024-10-18 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_ULTIMATE_WARN_H
#define SILLY_UTILS_SILLY_ULTIMATE_WARN_H
#include <tzx/warn/silly_simple_warn.h>

class silly_ultimate_warn : public silly_simple_warn
{
  public:
    /// 构造函数
    silly_ultimate_warn() = default;
    /// 析构函数
    ~silly_ultimate_warn() = default;

    /// getter and setter
    std::string name() const;
    void name(const std::string& name);
    std::string desc() const;
    void desc(const std::string& desc);
    std::string adnm() const;
    void adnm(const std::string& adnm);
    float static_rule() const;
    void static_rule(const float& static_rule);
    float dynamic_rule() const;
    void dynamic_rule(const float& dynamic_rule);
    float moisture_p() const;
    void moisture_p(const float& moisture_p);



  protected:
    std::string m_name; // 告警名称
    std::string m_desc; // 告警描述
    std::string m_adnm; // 政区信息
    float m_static_rule{0}; // 静态指标
    float m_dynamic_rule{0}; // 动态指标
    float m_moisture_p{0}; // 含水率百分比 单位%
};

#endif  // SILLY_UTILS_SILLY_ULTIMATE_WARN_H
