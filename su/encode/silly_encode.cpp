//
// Created by dly on 2023/8/3.
//

#include "silly_encode.h"
#include <log/silly_log.h>
#include <iconv.h>
#include <files/silly_file.h>

eCharset silly_encode::system_encode()
{
    eCharset code = eCharset::Unknown;
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
    if (out_len > 0)
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

eCharset silly_encode::check_file_encode(const suPath &file)
{
    eCharset code = eCharset::Unknown;
    std::ifstream fin(file.string(), std::ios::binary);
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
            code = eCharset::Unicode;
            break;
        case 0xfeff:  // 65279
            code = eCharset::Unicode_BE;
            break;
        case 0xe6a2:  // 59042
            code = eCharset::UTF8;
            break;
        default:
            code = eCharset::ANSI;
    }
    return code;
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
    if (IS_UTF8(u8str))
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
