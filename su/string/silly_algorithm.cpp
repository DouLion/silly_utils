//
// Created by dell on 2024/2/2.
//

#include "silly_algorithm.h"

std::vector<std::string> silly::str::algo::split(const std::string &str, const char &c)
{
    std::vector<std::string> results;
    size_t pos1 = 0;
    size_t pos0 = 0;
    size_t total = str.size();
    std::string n_str = str;

    while (pos1 < total)
    {
        if (n_str[pos1] == c)
        {
            std::string tmp = n_str.substr(pos0, pos1 - pos0);
            results.push_back(tmp);
            pos0 = pos1 + 1;
        }
        pos1++;
    }
    if (pos0 != total)
    {
        results.push_back(str.substr(pos0, total - pos0));
    }
    return results;
}

std::vector<std::string> silly::str::algo::split(const std::string &str, const std::string &s)
{
    std::vector<std::string> results;
    size_t pos_s = s.size();
    size_t pos_s0 = 0;
    size_t post_se = 0;
    size_t pos1 = 0;
    size_t pos0 = 0;
    size_t total = str.size() - pos_s;

    while (pos1 < total)
    {
        pos_s0 = 0;
        while (pos_s0 < pos_s && str[pos1 + pos_s0] == s[pos_s0])
        {
            pos_s0++;
        }
        if (pos_s0 == pos_s)  // 完全匹配上
        {
            std::string tmp = str.substr(pos0, pos1 - pos0);
            results.push_back(tmp);
            pos1 = pos1 + pos_s;
            pos0 = pos1;
        }
        pos1++;
    }
    pos_s0 = 0;
    while (pos_s0 < pos_s && str[pos1 + pos_s0] == s[pos_s0])
    {
        pos_s0++;
    }
    if (pos_s0 == pos_s)  // 完全匹配上
    {
        std::string tmp = str.substr(pos0, pos1 - pos0);
        results.push_back(tmp);
        pos1 = pos1 + pos_s;
        pos0 = pos1;
    }
    else
    {
        if (pos0 != pos1)
        {
            results.push_back(str.substr(pos0, str.size() - pos0));
        }
    }

    return results;
}

std::string silly::str::algo::replace(const std::string &src, const std::string &find, const std::string rep)
{
    std::string result = src;
    size_t pos = 0;
    while ((pos = result.find(find, pos)) != std::string::npos)
    {
        result.replace(pos, find.length(), rep);
        pos += rep.length();  // 移动到替换后的位置，避免无限循环
    }
    return result;
}

size_t silly::str::algo::count_with_chinese_character(const std::string &u8str)
{
    size_t count = 0;
    for (size_t i = 0; i < u8str.size(); ++i)
    {
        // 对于UTF-8编码，中文字符占3或4个字节，其首字节均大于0x80
        if ((unsigned char)u8str[i] >= 0x80)
        {
            // 跳过整个中文字符的字节
            if ((u8str[i] & 0xF0) == 0xF0)
            {
                i += 3;  // 对于4字节的UTF-8字符
            }
            else
            {
                i += 2;  // 对于3字节的UTF-8字符
            }
            ++count;
        }
    }
    return count;
}

std::string silly::str::algo::ltrim(std::string str)
{
    auto it = std::find_if(str.begin(), str.end(), [](char ch) {
        return !std::isspace<char>(ch, std::locale::classic());
    });
    return std::string(it, str.end());
}

std::string silly::str::algo::rtrim(std::string str)
{
    auto it = std::find_if(str.rbegin(), str.rend(), [](char ch) {
                  return !std::isspace<char>(ch, std::locale::classic());
              }).base();
    return std::string(str.begin(), it);
}

std::string silly::str::algo::trim(std::string str)
{
    return ltrim(rtrim(str));
}

std::string silly::str::algo::trim_all(std::string str)
{
    std::string result;
    std::remove_copy_if(str.begin(), str.end(), std::back_inserter(result), ::isspace);
    return result;
}
