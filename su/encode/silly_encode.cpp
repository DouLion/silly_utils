//
// Created by dly on 2023/8/3.
//

#include "silly_encode.h"
#include <log/silly_log.h>
#include <iconv.h>

silly_encode::enum_encode silly_encode::system_encode()
{
    silly_encode::enum_encode code = silly_encode::enum_encode::Unknown;
    std::string a = std::cout.getloc().name();
    return code;
}

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
    size_t src_len = src.size();
    for (size_t i = 0; i < src_len; i++)
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

bool silly_encode::iconv_convert(const std::string &from, const std::string &to, const std::string &text, std::string &ret)
{
    bool status = false;
    iconv_t cd;
    cd = iconv_open(to.c_str(), from.c_str());
    if (cd == (iconv_t)-1)
    {
        SLOG_ERROR("iconv_open");
        return status;
    }

    size_t src_len = text.length();
    // 准备输出缓冲区
    std::string str_out;

    size_t out_len = src_len * 4;
    str_out.resize(out_len);  // 假设每个字符最多需要 4 个字节

    // 进行转换
    size_t inBytesLeft = src_len;
    char *in = (char *)&text[0];
    size_t outBytesLeft = out_len;
    char *out = (char *)&str_out[0];


    size_t result = iconv(cd, &in, &inBytesLeft, &out, &outBytesLeft);
   /* if (result == (size_t)-1)
    {
        if (errno == EILSEQ)
        {  // 非法序列
            SLOG_ERROR("非法序列")
        }
        else
        {
            SLOG_ERROR("iconv")
        }
    }
    else*/
    out_len = out_len - outBytesLeft;
    if(out_len > 0)
    {
        // 计算有效输出字节
        ret.resize(out_len);
        status = true;
    }

    memcpy(&ret[0], (char *)&str_out[0], out_len);
    // 关闭 iconv 描述符
    iconv_close(cd);
    return status;
}

silly_encode::enum_encode silly_encode::check_file_encode(const std::string &path)
{
    silly_encode::enum_encode code = silly_encode::enum_encode::Unknown;
    std::ifstream fin(path, std::ios::binary);
    if (fin.is_open())
    {
        return code;
    }
    unsigned char s2;
    fin.read((char *)&s2, sizeof(s2));  // 读取第一个字节，然后左移8位
    int p = s2 << 8;
    fin.read((char *)&s2, sizeof(s2));  // 读取第二个字节
    p += s2;
    fin.close();

    switch (p)  // 判断文本前两个字节
    {
        case 0xfffe:  // 65534
            code = silly_encode::enum_encode::Unicode;
            break;
        case 0xfeff:  // 65279
            code = silly_encode::enum_encode::Unicode_BE;
            break;
        case 0xe6a2:  // 59042
            code = silly_encode::enum_encode::UTF8;
            break;
        default:
            code = silly_encode::enum_encode::ANSI;
    }
    return code;
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
                    return enum_encode::ANSI;
                }
                nBytes--;
            }
        }
        else
        {
            if ((ch & 0xC0) != 0x80)
            {
                return enum_encode::ANSI;
            }
            nBytes--;
        }
    }
    if (nBytes > 0 || bAnsi)
    {
        return enum_encode::ANSI;
    }
    return enum_encode::UTF8;
}

silly_encode::enum_encode silly_encode::detect_encode(const char *data, size_t size)
{
    if (size > 2 && data[0] == 0xFF && data[1] == 0xFE)
    {
        return enum_encode::UTF16_LE;
    }
    else if (size > 2 && data[0] == 0xFE && data[1] == 0xFF)
    {
        return enum_encode::UTF16_BE;
    }
    else if (size > 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
    {
        return enum_encode::UTF8_BOM;
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
    int nBytes = 0;  // UFT8可用1-6个字节编码,ASCII用一个字节
    unsigned char chr;
    // bool bAllAscii = true; // 如果全部都是ASCII, 说明不是UTF-8
    for (i = 0; i < length; i++)
    {
        chr = *(str + i);
        // if ((chr & 0x80) != 0 && bAllAscii) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
        //     bAllAscii = false;
        if (nBytes == 0)  // 如果不是ASCII码,应该是多字节符,计算字节数
        {
            if (chr >= 0x80)
            {
                if (chr > 0xFD)
                    return false;
                else if (chr >= 0xFC && chr <= 0xFD)  // 6字节 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                    nBytes = 6;
                else if (chr >= 0xF8)  // 5字节 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                    nBytes = 5;
                else if (chr >= 0xF0)  // 4字节 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    nBytes = 4;
                else if (chr >= 0xE0)  // 3字节 1110xxxx 10xxxxxx 10xxxxxx
                    nBytes = 3;
                else  // if(chr >= 0xC0)  2字节 110xxxxx 10xxxxxx gbk编码
                    return false;

                nBytes--;
            }
        }
        else  // 多字节符的非首字节,应为 10xxxxxx
        {
            if ((chr & 0xC0) != 0x80)
            {
                return false;
            }
            nBytes--;
        }
    }

    if (nBytes > 0 /*|| bAllAscii*/)  // 违返规则 或者 全部都是ASCII, 说明不是UTF-8
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
    catch (std::exception &e)
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
    catch (std::exception &e)
    {
        SU_ERROR_PRINT("%s", e.what());
        s_result.clear();
    }
    return s_result;
}
bool silly_encode::is_utf8(const std::string &str)
{
    int expected_bytes = 0;
    for (unsigned char c : str)
    {
        if (expected_bytes == 0)
        {
            if ((c >> 7) == 0)
            {
                // 1 byte character (0xxxxxxx)
                continue;
            }
            else if ((c >> 5) == 0b110)
            {
                // Start of 2 byte character (110xxxxx)
                expected_bytes = 1;
            }
            else if ((c >> 4) == 0b1110)
            {
                // Start of 3 byte character (1110xxxx)
                expected_bytes = 2;
            }
            else if ((c >> 3) == 0b11110)
            {
                // Start of 4 byte character (11110xxx)
                expected_bytes = 3;
            }
            else
            {
                return false;  // Invalid UTF-8
            }
        }
        else
        {
            if ((c >> 6) != 0b10)
            {
                return false;  // Invalid continuation byte
            }
            expected_bytes--;
        }
    }
    return expected_bytes == 0;  // All expected bytes must be accounted for
}

std::string silly_encode::gbk_utf8(const std::string &text)
{
    std::string ret;
    if (!iconv_convert("GBK", "UTF-8", text, ret))
    {
        ret.clear();
    }
    return ret;
}
std::string silly_encode::utf8_gbk(const std::string &text)
{
    std::string ret;
    if (!iconv_convert("UTF-8", "GBK", text, ret))
    {
        ret.clear();
    }
    return ret;
}
std::string silly_encode::unicode_gbk(const std::string &text)
{
    std::string ret;
    std::string u8str = unicode_utf8(text);
    if (is_utf8(u8str))
    {
        ret = utf8_gbk(u8str);
    }
    return ret;
}
std::string silly_encode::unicode_utf8(const std::string &text)
{
    std::string ret;
    for (size_t i = 0; i < text.size(); ++i)
    {
        if (text[i] == '\\' && i + 5 < text.size())
        {
            // 提取4位十六进制数
            std::string hex = text.substr(i + 2, 4);
            unsigned long code = std::stoul(hex, nullptr, 16);

            // 转换为UTF-8字节序列
            if (code <= 0x7F)
            {
                ret += static_cast<char>(code);
            }
            else if (code <= 0x7FF)
            {
                ret += static_cast<char>((code >> 6) | 0xC0);
                ret += static_cast<char>((code & 0x3F) | 0x80);
            }
            else
            {
                ret += static_cast<char>((code >> 12) | 0xE0);
                ret += static_cast<char>(((code >> 6) & 0x3F) | 0x80);
                ret += static_cast<char>((code & 0x3F) | 0x80);
            }
            i += 5;  // 跳过整个转义序列
        }
        else
        {
            ret.push_back(text[i]);
        }
    }
    return ret;
}
