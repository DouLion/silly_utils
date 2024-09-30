/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_dynamic_rule.h
 * @description: silly_dynamic_rule 类声明
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_DYNAMIC_RULE_H
#define SILLY_UTILS_SILLY_DYNAMIC_RULE_H
#include <su_marco.h>
#include <datastruct/silly_safe_bimap.h>
static uint8_t DRFMT_CODE_INDEX = 0b10001001;   // 前三个个bit表示类型, 后面为占位符
static uint8_t DRFMT_CODE_LENGTH = 0b11100000;  // 前三个bit表示类型, 5位组成字符串长度 字符串长度限制在 0-31 之间

class dynamic_rule_record
{
  public:
    int32_t index;
    std::string code;
    int64_t atm;
    float moisture_percent;
    std::map<uint8_t, std::map<uint8_t, float>> intv_grade_threshold;

  public:
    bool deserialize(const std::string& data);

    std::string serialize() const;

    std::string serialize(const int& code_index) const;
};

/// 这个索引文件对外应该只支持读, 不支持写, 是一个相对固定的文件
class dynamic_rule_code_index : public silly_safe_bimap<std::string, size_t>
{
  public:
    bool load(const std::string& path);
    bool save(const std::string& path);
    bool add(const std::string& code);
    bool remove(const std::string& code);
    size_t index(const std::string& code) const;
    std::string code(const size_t index) const;

  private:
    bool add(const std::string& code, const size_t index);

  private:
    std::mutex m_mutex;
    size_t max_index = 0;
};

class silly_dynamic_rule
{
  public:
    silly_dynamic_rule();
    ~silly_dynamic_rule();

    bool load_code_index(const std::string& path);
    bool save_code_index(const std::string& path);

    bool read_with_code_index(const std::string& path, std::map<std::string, dynamic_rule_record>& records) const;

    bool write_with_code_index(const std::string& path, const std::map<std::string, dynamic_rule_record>& records) const;
    void add_code_index(const std::string& code,  size_t& index);

  private:
    dynamic_rule_code_index m_index{};
};

#endif  // SILLY_UTILS_SILLY_DYNAMIC_RULE_H
