/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-09-25
 * @file: silly_system.cpp
 * @description: silly_system 头文件
 * @version: v1.0.1 2025-09-25 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SYSTEM_H
#define SILLY_UTILS_SILLY_SYSTEM_H
#include <su_macro.h>

/**
 * 1. Debug 模式时,切换到项目目录,能够以./config加载配置信息
 * 2. Release 模式时, 如果 可执行程序与config在同级目录, 不做任何处理
 *      如果 可执行程序在bin目录中, 则切换到config的同级目录
 *      -- config
 *          --
 *      -- bin
 *          -- 模式2.exe
 *          模式1.exe-
 * @param wd  CMAKE中设置的项目目录,仅在debug模式下有效
 * @return
 */
void BIN_CHDIR(const std::filesystem::path& wd);

#if _WIN32
// 这部分在linux下只能使用iconv等第三方库实现
std::wstring S2WS(const std::string& str, UINT codePage = CP_UTF8);

std::string WS2S(const std::wstring& str, UINT codePage = CP_UTF8);

std::wstring UTF8_S2WS(const std::string& str);

std::string WS2UTF8_S(const std::wstring& str);

std::wstring GBK_S2WS(const std::string& str);

std::string WS2GBK_S(const std::wstring& str);
std::string GBKToUTF8(const std::string& gbkStr);

std::string UTF8ToGBK(const std::string& utf8Str);

void WINDOWS_UTF8_PAGE(const unsigned int& codePage = 65001, const std::string& font = "Lucida Console");

#endif

void SET_ENV(const std::string& name, const std::string& value, const bool& overwrite = true);

std::string GET_ENV(const std::string& name);

std::string TO_UPPER(const std::string& str);
std::string TO_LOWER(const std::string& str);

/// 去除所有空白（' '、\t、\n等）
/// @param str
/// @return
std::string TRIM(const std::string& str);
std::string TRIM_LEFT(const std::string& str);
std::string TRIM_RIGHT(const std::string& str);
std::string TRIM_ALL(const std::string& str);

std::string REPLACE(const std::string& str, const std::string& find, const std::string& replace);
std::string REPLACE(const std::string& str, const char& find, const char& replace);
std::string REGEX_REPLACE(const std::string& str, const std::string& pattern, const std::string& replace);

std::vector<std::string> SPLIT(const std::string& str, const char& delimiter);
std::vector<std::string> SPLIT(const std::string& str, const std::string& delimiter);

/**
 * 通配符模式转换为正则表达式
 * @param pattern
 * @return
 */
std::string WILDCARD2REGEX(const std::string& pattern);

bool IS_UTF8(const std::string& str);
bool IS_GBK(const std::string& str);

/**
 * 是否仅包含ASCII编码
 * @param str
 * @return
 */
bool ONLY_ASCII(const std::string& str);

/**
 * 是否包含UTF8编码字符串
 * @param str
 * @return
 */
bool HAS_UTF8(const std::string& str);

/**
 * 是否包含GBK编码字符串
 * @param str
 * @return
 */
bool HAS_GBK(const std::string& str);

double RANDOM(const double& min, const double& max);
std::vector<double> RANDOM(const double& min, const double& max, const size_t& n);

/**
 * 获取内存页大小
 * @return
 */
size_t MEM_PAGE_SIZE();

/**
 * 获取类名
 * @return
 */
template <typename T>
std::string CLASS_SPACE_NAME()
{
    std::string ret = typeid(T).name();
#if _WIN32
    if (ret.find("class ") != std::string::npos) {
        ret = ret.substr(6);
    } else if (ret.find("struct ") != std::string::npos) {
        ret = ret.substr(7);
    } else if (ret.find("union ") != std::string::npos) {
        ret = ret.substr(6);
    } else if (ret.find("enum ") != std::string::npos) {
        ret = ret.substr(5);
    }
#else
    int status = -4;
    std::unique_ptr<char, decltype(&std::free)> demangled(abi::__cxa_demangle(ret.c_str(), nullptr, nullptr, &status), &std::free);

    if (status == 0 && demangled)
    {
        return std::string(demangled.get());
    }
    return ret;
#endif

    return ret;
}
template <typename T>
std::string CLASS_PURE_NAME()
{
    std::string ret;
    ret = CLASS_SPACE_NAME<T>();
    auto pos = ret.find_last_of(':');
    if (pos == std::string::npos)
    {
        return ret;
    }
    return ret.substr(pos+1);
}
template <typename T>
std::string CLASS_SPACE_NAME(const T& )
{
    return CLASS_SPACE_NAME<T>();
}
template <typename T>
std::string CLASS_PURE_NAME(const T& )
{
    return CLASS_PURE_NAME<T>();
}

template <typename CxxMAP, typename K>
bool MAP_HAS(const CxxMAP& map, const K& key)
{
    return map.find(key) != map.end();
}

template <typename CxxSET, typename K>
bool SET_HAS(const CxxSET& set, const K& key)
{
    return set.find(key) != set.end();
}
template <typename CxxVECTOR, typename K>
bool VEC_HAS(const CxxVECTOR& vec, const K& key)
{
    for (const auto& item : vec)
    {
        if (key == item)
        {
            return true;
        }
    }
    return false;
}

#endif
