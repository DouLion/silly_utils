/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/3 11:36
 * @version: 1.0.1
 * @software: silly_utils
 * @description: 主要依靠libiconv实现字符串编码转换功能
 */
#pragma once

/* 支持的编码 https://www.gnu.org/software/libiconv/#TOCdownloading

    It provides support for the encodings:

    European languages
        ASCII, ISO-8859-{1,2,3,4,5,7,9,10,13,14,15,16}, KOI8-R, KOI8-U, KOI8-RU, CP{1250,1251,1252,1253,1254,1257}, CP{850,866,1131}, Mac{Roman,CentralEurope,Iceland,Croatian,Romania}, Mac{Cyrillic,Ukraine,Greek,Turkish}, Macintosh
    Semitic languages
        ISO-8859-{6,8}, CP{1255,1256}, CP862, Mac{Hebrew,Arabic}
    Japanese
        EUC-JP, SHIFT_JIS, CP932, ISO-2022-JP, ISO-2022-JP-2, ISO-2022-JP-1, ISO-2022-JP-MS
    Chinese
        EUC-CN, HZ, GBK, CP936, GB18030, EUC-TW, BIG5, CP950, BIG5-HKSCS, BIG5-HKSCS:2004, BIG5-HKSCS:2001, BIG5-HKSCS:1999, ISO-2022-CN, ISO-2022-CN-EXT
    Korean
        EUC-KR, CP949, ISO-2022-KR, JOHAB
    Armenian
        ARMSCII-8
    Georgian
        Georgian-Academy, Georgian-PS
    Tajik
        KOI8-T
    Kazakh
        PT154, RK1048
    Thai
        ISO-8859-11, TIS-620, CP874, MacThai
    Laotian
        MuleLao-1, CP1133
    Vietnamese
        VISCII, TCVN, CP1258
    Platform specifics
        HP-ROMAN8, NEXTSTEP
    Full Unicode
        UTF-8
        UCS-2, UCS-2BE, UCS-2LE
        UCS-4, UCS-4BE, UCS-4LE
        UTF-16, UTF-16BE, UTF-16LE
        UTF-32, UTF-32BE, UTF-32LE
        UTF-7
        C99, JAVA
    Full Unicode, in terms of uint16_t or uint32_t (with machine dependent endianness and alignment)
        UCS-2-INTERNAL, UCS-4-INTERNAL
    Locale dependent, in terms of `char' or `wchar_t' (with machine dependent endianness and alignment, and with OS and locale dependent semantics)
        char, wchar_t
        The empty encoding name "" is equivalent to "char": it denotes the locale dependent character encoding.

    !!!!当配置了编译选项--enable-extra-encodings后,才有以下的编码可以使用!!!!
    When configured with the option --enable-extra-encodings, it also provides support for a few extra encodings:
    European languages
        CP{437,737,775,852,853,855,857,858,860,861,863,865,869,1125}
    Semitic languages
        CP864
    Japanese
        EUC-JISX0213, Shift_JISX0213, ISO-2022-JP-3
    Chinese
        BIG5-2003 (experimental)
    Turkmen
        TDS565
    Platform specifics
        ATARIST, RISCOS-LATIN1
    It can convert from any of these encodings to any other, through Unicode conversion.
*/

#ifndef SILLY_UTILS_SILLY_ENCODE_H
#define SILLY_UTILS_SILLY_ENCODE_H
#include <su_marco.h>

class silly_encode
{
  public:
    enum class enum_encode
    {
        Unknown = 0,
        ANSI = 1,    //
        UTF16_LE = 2,
        UTF16_BE = 3,
        UTF8_BOM = 4,
        UTF8 = 5,
        Unicode = 6,
        Unicode_BE = 7,
        GBK = 8
    };

    static bool is_utf8(const std::string& str);

    static enum_encode system_encode();

    /// <summary>
    /// 检查文件编码
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static enum_encode check_file_encode(const std::string &path);


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
    /// <param name="from">源字符串编码</param>
    /// <param name="to">目标字符串编码</param>
    /// <param name="text">源字符串</param>
    /// <returns></returns>
    static bool iconv_convert(const std::string &from, const std::string &to, const std::string &text, std::string &ret);


    /// <summary>
    /// 一常用的编码转化
    /// </summary>
    /// <param name="text"></param>
    /// <returns></returns>
    static std::string gbk_utf8(const std::string &text);
    static std::string utf8_gbk(const std::string &text);
    static std::string unicode_gbk(const std::string &text);
    static std::string unicode_utf8(const std::string &text);

    /// <summary>
    /// utf8转宽字符, 这个函数没有经过验证,且于区别平台的大端序和小端序
    /// </summary>
    /// <param name="text"></param>
    /// <returns></returns>
    static std::wstring utf8_wchar(const std::string &text);

    static enum_encode is_utf8(const char *data, size_t size);

    static enum_encode detect_encode(const char *data, size_t size);

    static bool check_text_utf8(const char *str, int length);

    static std::string to_hex(const unsigned char *s_byte, const size_t &len);
    static void from_hex(const char *str, unsigned char *Hstr);

    static std::wstring cxx11_string_wstring(const std::string &str);

    static std::string cxx11_wstring_string(const std::wstring &wstr);
};

#endif  // SILLY_UTILS_SILLY_ENCODE_H
