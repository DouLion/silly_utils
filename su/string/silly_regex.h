/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/2 14:04
 * @version: 1.0.1
 * @description: c++ 正则表达式工具
 */
#ifndef SILLY_UTILS_SILLY_REGEX_H
#define SILLY_UTILS_SILLY_REGEX_H

#include <regex>
#include <encode/silly_encode.h>

// #define IS_##varname(utf8s) std::regex_match(utf8s, std::regex(varname))

/// 一些常用正则表达式

// 空模板
#ifndef SILLY_REGEX_EMPTY
#define SILLY_REGEX_EMPTY ""
#define IS_SILLY_REGEX_EMPTY(utf8s) std::regex_match(utf8s, std::regex(SILLY_REGEX_EMPTY))
#endif

// QQ号
#ifndef SILLY_REGEX_QQ
#define SILLY_REGEX_QQ "^[1-9]{1}[0-9]{4,14}$"
#define IS_SILLY_REGEX_QQ(utf8s) std::regex_match(utf8s, std::regex(SILLY_REGEX_QQ))
#endif

// 邮箱
#ifndef SILLY_REGEX_EMAIL
#define SILLY_REGEX_EMAIL "^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$"
#define IS_SILLY_REGEX_EMAIL(utf8s) std::regex_match(utf8s, std::regex(SILLY_REGEX_EMAIL))
#endif

// 以下标记为内地的仅适用于中国内地地区, 港澳台地区不清楚
// 内地身份证
#ifndef SILLY_REGEX_CN_INLAND_ID_CARD_NUM
#define SILLY_REGEX_CN_INLAND_ID_CARD_NUM "^[1-9]\\d{5}(18|19|20)\\d{2}((0[1-9])|(1[0-2]))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$"
#define IS_SILLY_REGEX_CN_INLAND_ID_CARD_NUM(utf8s) std::regex_match(utf8s, std::regex(SILLY_REGEX_CN_INLAND_ID_CARD_NUM))
#endif

// 内地手机号
#ifndef SILLY_REGEX_CN_INLAND_MOBILE
#define SILLY_REGEX_CN_INLAND_MOBILE "^[1]{1}[0-9]{10}$"
#define IS_SILLY_REGEX_CN_INLAND_MOBILE(utf8s) std::regex_match(utf8s, std::regex(SILLY_REGEX_CN_INLAND_MOBILE))
#endif

// 内地邮编
#ifndef SILLY_REGEX_CN_INLAND_POST_CODE
#define SILLY_REGEX_CN_INLAND_POST_CODE "^[0-9]{6}$"
#define IS_SILLY_REGEX_CN_INLAND_POST_CODE(utf8s) std::regex_match(utf8s, std::regex(SILLY_REGEX_CN_INLAND_POST_CODE))
#endif

// 内地车牌号, 输入的中文必须是UTF8编码,省份市编码最多有一个特殊字符 (' ', '+', '-','.', 不包含'_')隔开 例如 "京A 12345"
#ifndef SILLY_REGEX_CN_INLAND_LICENSE_PLATE_NUMBER
#define SILLY_REGEX_CN_INLAND_LICENSE_PLATE_NUMBER L"^[\\u4e00-\\u9fa5][A-Z]([\\W]{0,1})[A-Z0-9]{5}$"
#define IS_SILLY_REGEX_CN_INLAND_LICENSE_PLATE_NUMBER(utf8s) silly_encode::check_text_utf8(utf8s.c_str(), utf8s.size()) ? std::regex_match(silly_encode::cxx11_string_wstring(utf8s), std::wregex(SILLY_REGEX_CN_INLAND_LICENSE_PLATE_NUMBER)) : false
#endif

#endif  // SILLY_UTILS_SILLY_REGEX_H
