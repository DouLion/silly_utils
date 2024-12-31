//
// Created by dell on 2024/1/2.
//

#include "silly_regex.h"
using namespace silly::str;
/// 一些常用正则表达式

// QQ号
#define REG_QQ "^[1-9]{1}[0-9]{4,14}$"

// 邮箱
#define REG_EMAIL "^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$"

// 以下标记为内地的仅适用于中国内地地区, 港澳台地区不清楚
// 内地身份证
#define REG_MAINLAND_ID_CARD_NUM "^[1-9]\\d{5}(18|19|20)\\d{2}((0[1-9])|(1[0-2]))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$"

// 内地手机号
#define REG_MAINLAND_MOBILE "^[1]{1}[0-9]{10}$"

// 内地邮编
#define REG_MAINLAND_POST_CODE "^[0-9]{6}$"

// 内地车牌号, 输入的中文必须是UTF8编码,省份市编码最多有一个特殊字符 (' ', '+', '-','.', 不包含'_')隔开 例如 "京A 12345"
#define REG_MAINLAND_LICENSE_PLATE_NUMBER "^[\\u4e00-\\u9fa5][A-Z]([\\W]{0,1})[A-Z0-9]{5}$"

bool valid::QQ(const std::string&  str)
{
    return std::regex_match( str, std::regex(REG_QQ));
}
bool valid::email(const std::string&  str)
{
    return std::regex_match( str, std::regex(REG_EMAIL));
}
bool valid::mainland_id(const std::string&  str)
{
    return std::regex_match( str, std::regex(REG_MAINLAND_ID_CARD_NUM));
}
bool valid::mainland_mobile(const std::string&  str)
{
    return std::regex_match( str, std::regex(REG_MAINLAND_MOBILE));
}
bool valid::mainland_post(const std::string&  str)
{
    return std::regex_match( str, std::regex(REG_MAINLAND_POST_CODE));
}
bool valid::mainland_vehicle(const std::string&  str)
{
    std::string us =  str;
    if (!silly_encode::check_text_utf8( str.c_str(),  str.size()))
    {
        us = silly_encode::gbk_utf8( str);
    }

    return std::regex_match(us, std::regex(REG_MAINLAND_LICENSE_PLATE_NUMBER));
}
