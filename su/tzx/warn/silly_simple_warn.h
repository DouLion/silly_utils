/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-18
 * @file: silly_simple_warn.h
 * @description: 一个告警必要的属性内容
 * @version: v1.0.1 2024-10-18 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SIMPLE_WARN_H
#define SILLY_UTILS_SILLY_SIMPLE_WARN_H
#include <datetime/silly_posix_time.h>
class silly_simple_warn
{
  public:
    /// 构造函数 析构函数
    silly_simple_warn() = default;
    ~silly_simple_warn() = default;
    /// getter
    std::string code() const;
    silly_posix_time btm() const;
    int grade() const;
    int type() const;
    int status() const;
    float intv() const;
    float rule() const;
    float value() const;
    /// setter
    void code(const std::string& code);
    void btm(const silly_posix_time& btm);
    void grade(const int& grade);
    void type(const int& type);
    void status(const int& status);
    void intv(const float& intv);
    void rule(const float& rule);
    void value(const float& value);

  protected:
    std::string m_code;
    silly_posix_time m_btm;
    int m_grade{0};    //  告警等级
    int m_type{0};     // 告警类型 实测降雨 实测水位等
    int m_status{0};   // 告警状态
    float m_intv{0};   // 仅对累计值有用 如降雨
    float m_rule{0};   // 指标值
    float m_value{0};  // 告警值
};

#endif  // SILLY_UTILS_SILLY_SIMPLE_WARN_H
