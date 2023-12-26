/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/3 11:36
 * @version: 1.0.1
 * @software: silly_utils
 * @description: 字符串编码转换功能
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_ENCODE_H
#define SILLY_UTILS_SILLY_ENCODE_H
#include <su_marco.h>

class silly_encode
{
public:
    enum enum_encode
    {
        eeANSI = 1,
        eeUTF16_LE,
        eeUTF16_BE,
        eeUTF8_BOM,
        eeUTF8
    };

    /// <summary>
    /// URL编码
    /// </summary>
    /// <param name="src"></param>
    /// <returns></returns>
    static std::string url_encode(const std::string &src);

    /// <summary>
    /// URL解码
    /// </summary>
    /// <param name="src"></param>
    /// <returns></returns>
    static std::string url_decode(const std::string &src);

    /// <summary>
    /// 使用 iconv做的编码转换
    /// </summary>
    /// <param name="from"></param>
    /// <param name="to"></param>
    /// <param name="text"></param>
    /// <returns></returns>
    static std::string encode_convert(const char *from, const char *to, const char *text);

    static std::string gbk_utf8(const char *text)
    {
        return encode_convert("GBK", "UTF-8", text);
    }

    static std::string utf8_gbk(const char *text)
    {
        return encode_convert("UTF-8", "GBK", text);
    }

    static std::string unicode_gbk(const char *text)
    {
        return encode_convert("EUC-CN", "GBK", text);
    }

    static std::string unicode_utf8(const char *text)
    {
        return encode_convert("EUC-CN", "UTF-8", text);
    }

    static std::string gbk_utf8(const std::string &text)
    {
        return encode_convert("GBK", "UTF-8", text.c_str());
    }

    static std::string utf8_gbk(const std::string &text)
    {
        return encode_convert("UTF-8", "GBK", text.c_str());
    }

    static std::string unicode_gbk(const std::string &text)
    {
        return encode_convert("EUC-CN", "GBK", text.c_str());
    }

    static std::string unicode_utf8(const std::string &text)
    {
        return encode_convert("EUC-CN", "UTF-8", text.c_str());
    }

    static enum_encode is_utf8(const char *data, size_t size);

    static enum_encode detect_encode(const char *data, size_t size);

    static bool check_text_utf8(const char *str, int length);

    static std::string to_hex(const unsigned char *s_byte, const size_t &len);
    static void from_hex(const char *str, unsigned char *Hstr);
};

#endif // SILLY_UTILS_SILLY_ENCODE_H
