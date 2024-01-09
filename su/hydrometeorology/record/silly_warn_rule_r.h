/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/15 11:35
 * @version: 1.0.1
 * @description: 动态指标记录
 */
#ifndef SILLY_UTILS_SILLY_WARN_RULE_R_H
#define SILLY_UTILS_SILLY_WARN_RULE_R_H
#include <iostream>

#define SILLY_WARN_RULE_CODE_LENGTH 32
#define SILLY_WARN_RULE_R_LENGTH sizeof(char) + SILLY_WARN_RULE_CODE_LENGTH + sizeof(unsigned int) * 2 + sizeof(char) * 2 + sizeof(float) * 3
#define SILLY_WARN_RULE_R_BEG_CHAR '^'

class sliiy_warn_rule_record
{
  public:
    // 网格点ID
    char code[SILLY_WARN_RULE_CODE_LENGTH]{0};
    // 分析时间
    std::string atm;
    // intv 时间间隔 以小时计算
    float intv{0};
    // 等级
    int grade{0};
    // 土壤含水率
    float swc{0};
    // 雨量阈值
    float drrp{0};
    // 对应intv时间内的降雨量
    float drp{0};
    // 备注和时间戳不记录

  public:
    /// <summary>
    /// 将对象序列化位二进制数据
    /// </summary>
    /// <param name="c_in">序列化后的二进制数据</param>
    /// <param name="len">序列化后的二进制数据长度</param>
    /// <returns></returns>
    bool serialize(unsigned char** c_in, size_t& len);

    /// <summary>
    /// 将二进制数据反序列化位该对象
    /// </summary>
    /// <param name="c_in"></param>
    /// <param name="len"></param>
    /// <returns></returns>
    bool unserialize(const unsigned char* c_in, const size_t& len);

    bool operator==(const sliiy_warn_rule_record& other) const;

    sliiy_warn_rule_record() = default;
    sliiy_warn_rule_record(const std::string& atm, float intv, int grade, float swc, float drrp, float drp);
};

#endif  // SILLY_UTILS_SILLY_WARN_RULE_R_H
