//
// Created by dly on 2023/8/3.
//

#include "silly_encode.h"
#include <iconv.h>

std::string silly_encode::url_encode(const std::string &src)
{
    static char hex[] = "0123456789ABCDEF";
    std::string dst;

    for (size_t i = 0; i < src.size(); i++)
    {
        unsigned char ch = src[i];
        if (isalnum(ch))
        {
            dst += ch;
        }
        else if (src[i] == ' ')
        {
            dst += '+';
        }
        else
        {
            unsigned char c = static_cast<unsigned char>(src[i]);
            dst += '%';
            dst += hex[c / 16];
            dst += hex[c % 16];
        }
    }
    return dst;
}

std::string silly_encode::url_decode(const std::string &src)
{
    std::string dst;
    size_t srclen = src.size();
    for (size_t i = 0; i < srclen; i++)
    {
        if (src[i] == '%')
        {
            if (isxdigit(src[i + 1]) && isxdigit(src[i + 2]))
            {
                char c1 = src[++i];
                char c2 = src[++i];
                c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
                c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
                dst += (unsigned char)(c1 * 16 + c2);
            }
        }
        else
        {
            if (src[i] == '+')
            {
                dst += ' ';
            }
            else
            {
                dst += src[i];
            }
        }
    }

    return dst;
}

std::string silly_encode::encode_convert(const char *from, const char *to, const char *text)
{
    iconv_t cd = iconv_open(to, from);
    if (cd == (iconv_t)(-1))
    {
        SU_ERROR_PRINT("Not support encode convert from %s to %s", from, to);
        return "";
    }
    char *tmp_src = (char *)text;
    size_t src_len = strlen(tmp_src);
    size_t dst_len = src_len * 5;
    char *out = (char *)malloc(dst_len);
    if (!out)
    {
        SU_ERROR_PRINT("malloc failed.")
        return "";
    }
    memset(out, 0, dst_len);
    char *p_out_free = out;

    size_t ret = iconv(cd, &tmp_src, &src_len, &out, &dst_len);
    if (!(ret == (size_t)(-1) && errno == EINVAL))
    {
        SU_MEM_FREE(p_out_free)
        return "";
    }

    std::string retStr(p_out_free);
    SU_MEM_FREE(p_out_free)
    iconv_close(cd);

    return retStr;
}

std::wstring silly_encode::utf8_wchar(const std::string &text)
{
    std::wstring ret;
    iconv_t cd = iconv_open("wchar_t", "char");
    if (cd == (iconv_t)(-1))
    {
        return ret;
    }
    else
    {

        wchar_t *outbuf = new wchar_t[text.size() * 3 + 1];
        if (!outbuf)
        {
            return ret;
        }
        memset(outbuf, 0, sizeof(outbuf));

        char *inptr = (char *)text.c_str();
        size_t inbytesleft = 1;
        char *outptr = (char *)outbuf;
        size_t outbytesleft = sizeof(outbuf);
        size_t r = iconv(cd, (&inptr), &inbytesleft, &outptr, &outbytesleft);

        if (!(r == (size_t)(-1) && errno == EINVAL))
        {
            SUM_MEM_DEL_ARR(outbuf);
            return ret;
        }
        ret = std::wstring(outbuf);
        SUM_MEM_DEL_ARR(outbuf);
        return ret;
    }

    return ret;
}

silly_encode::enum_encode silly_encode::is_utf8(const char *data, size_t size)
{
    bool bAnsi = true;
    unsigned char ch = 0x00;
    int nBytes = 0;
    for (auto i = 0; i < size; i++)
    {
        ch = *(data + i);
        if ((ch & 0x80) != 0x00)
        {
            bAnsi = false;
        }
        if (nBytes == 0)
        {
            if (ch >= 0x80)
            {
                if (ch >= 0xFC && ch <= 0xFD)
                {
                    nBytes = 6;
                }
                else if (ch >= 0xF8)
                {
                    nBytes = 5;
                }
                else if (ch >= 0xF0)
                {
                    nBytes = 4;
                }
                else if (ch >= 0xE0)
                {
                    nBytes = 3;
                }
                else if (ch >= 0xC0)
                {
                    nBytes = 2;
                }
                else
                {
                    return enum_encode::eeANSI;
                }
                nBytes--;
            }
        }
        else
        {
            if ((ch & 0xC0) != 0x80)
            {
                return enum_encode::eeANSI;
            }
            nBytes--;
        }
    }
    if (nBytes > 0 || bAnsi)
    {
        return enum_encode::eeANSI;
    }
    return enum_encode::eeUTF8;
}

silly_encode::enum_encode silly_encode::detect_encode(const char *data, size_t size)
{
    if (size > 2 && data[0] == 0xFF && data[1] == 0xFE)
    {
        return enum_encode::eeUTF16_LE;
    }
    else if (size > 2 && data[0] == 0xFE && data[1] == 0xFF)
    {
        return enum_encode::eeUTF16_BE;
    }
    else if (size > 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
    {
        return enum_encode::eeUTF8_BOM;
    }
    else
    {
        return is_utf8(data, size);
    }
}

bool silly_encode::check_text_utf8(const char *str, int length)
{
    bool is_utf8;
    int i;
    int nBytes = 0; // UFT8可用1-6个字节编码,ASCII用一个字节
    unsigned char chr;
    // bool bAllAscii = true; // 如果全部都是ASCII, 说明不是UTF-8
    for (i = 0; i < length; i++)
    {
        chr = *(str + i);
        // if ((chr & 0x80) != 0 && bAllAscii) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
        //     bAllAscii = false;
        if (nBytes == 0) // 如果不是ASCII码,应该是多字节符,计算字节数
        {
            if (chr >= 0x80)
            {
                if (chr > 0xFD)
                    return false;
                else if (chr >= 0xFC && chr <= 0xFD) // 6字节 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                    nBytes = 6;
                else if (chr >= 0xF8) // 5字节 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                    nBytes = 5;
                else if (chr >= 0xF0) // 4字节 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    nBytes = 4;
                else if (chr >= 0xE0) // 3字节 1110xxxx 10xxxxxx 10xxxxxx
                    nBytes = 3;
                else // if(chr >= 0xC0)  2字节 110xxxxx 10xxxxxx gbk编码
                    return false;

                nBytes--;
            }
        }
        else // 多字节符的非首字节,应为 10xxxxxx
        {
            if ((chr & 0xC0) != 0x80)
            {
                return false;
            }
            nBytes--;
        }
    }

    if (nBytes > 0 /*|| bAllAscii*/) // 违返规则 或者 全部都是ASCII, 说明不是UTF-8
    {
        return false;
    }

    return true;
}

std::string silly_encode::to_hex(const unsigned char *s_byte, const size_t &len)
{
    std::string ret;
    ret.resize(2 * len);
    for (int i = 0; i < len; i++)
    {
        sprintf(&ret[2 * i], "%02X", s_byte[i]);
    }
    ret[len * 2] = 0;
    return ret;
}

void silly_encode::from_hex(const char *str, unsigned char *Hstr)
{
    int Len = (int)(strlen(str) / 2);
    for (int i = 0; i < Len; ++i)
    {
        if (isdigit(str[2 * i]))
        {
            Hstr[i] = (str[2 * i] - '0') * 16;
        }
        else
        {
            Hstr[i] = (toupper(str[2 * i]) - 'A' + 10) * 16;
        }

        if (isdigit(str[2 * i + 1]))
        {
            Hstr[i] += str[2 * i + 1] - '0';
        }
        else
        {
            Hstr[i] += toupper(str[2 * i + 1]) - 'A' + 10;
        }
    }

    Hstr[Len] = 0;
}

#include <codecvt>
#include <string>
#include <cstring>
#include <locale>
// c++17 已经将wstring_convert废弃了,但是目前还能用
std::wstring silly_encode::cxx11_string_wstring(const std::string &str)
{
    std::wstring ws_result;
    try
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;

        ws_result = converterX.from_bytes(str);
    }
    catch (std::exception& e)
    {
        SU_ERROR_PRINT("%s", e.what());
        ws_result.clear();
    }
    return ws_result;
   }

std::string silly_encode::cxx11_wstring_string(const std::wstring &wstr)
{
    std::string s_result;
    try
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;

        s_result = converterX.to_bytes(wstr);
    }
    catch (std::exception& e)
    {
        SU_ERROR_PRINT("%s", e.what());
        s_result.clear();
    }
    return s_result;
}
