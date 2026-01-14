/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-09-25
 * @file: silly_system.c
 * @description: silly_system实现
 * @version: v1.0.1 2025-09-25 dou li yang
 */
#include "silly_system.h"
extern void BIN_CHDIR(const std::filesystem::path& wd)
{
#ifndef NDEBUG
    auto cfgPath = wd;
    cfgPath.append("config");
    if (std::filesystem::exists(cfgPath))
    {
        std::filesystem::current_path(wd);
    }
#else
    auto curr = std::filesystem::current_path();
    if (curr.filename().string() == "bin")
    {
        std::filesystem::current_path(curr.parent_path());
    }
#endif
}

#if _WIN32
extern std::wstring S2WS(const std::string& str, UINT codePage)

{
    std::wstring ret;
    if (str.empty())
    {
        return ret;
    }

    int size = ::MultiByteToWideChar(codePage,                      // 源编码
                                     0,                             // 标志（一般填0）
                                     str.c_str(),                   // 输入窄字符串
                                     static_cast<int>(str.size()),  // 输入字符串长度
                                     nullptr,                       // 输出缓冲区（NULL 表示计算所需大小）
                                     0                              // 输出缓冲区大小
    );

    if (size <= 0)
    {
        return ret;
    }

    ret.resize(size, 0);
    MultiByteToWideChar(codePage, 0, str.c_str(), static_cast<int>(str.size()), ret.data(), size);

    return ret;
}
extern std::string WS2S(const std::wstring& str, UINT codePage)

{
    std::string ret;
    if (str.empty())
    {
        return ret;
    }

    int size = WideCharToMultiByte(codePage,                      // 目标编码
                                   0,                             // 标志（一般填0）
                                   str.c_str(),                   // 输入宽字符串
                                   static_cast<int>(str.size()),  // 输入字符串长度（-1 表示自动计算长度）
                                   nullptr,                       // 输出缓冲区（NULL 表示计算所需大小）
                                   0,                             // 输出缓冲区大小
                                   nullptr,
                                   nullptr  // 默认字符和是否使用默认字符
    );

    if (size <= 0)
    {
        return ret;
    }
    ret.resize(size, 0);
    WideCharToMultiByte(codePage, 0, str.c_str(), static_cast<int>(str.size()), ret.data(), size, nullptr, nullptr);

    return ret;
}
extern std::wstring UTF8_S2WS(const std::string& str)

{
    return S2WS(str, CP_UTF8);
}
extern std::string WS2UTF8_S(const std::wstring& str)

{
    return WS2S(str, CP_UTF8);
}
extern std::wstring GBK_S2WS(const std::string& str)
{
    return S2WS(str, CP_ACP);
}
extern std::string WS2GBK_S(const std::wstring& str)
{
    return WS2S(str, CP_ACP);
}
extern std::string GBKToUTF8(const std::string& gbkStr)
{
    return WS2S(S2WS(gbkStr, CP_ACP), CP_UTF8);
}
extern std::string UTF8ToGBK(const std::string& utf8Str)
{
    return WS2S(S2WS(utf8Str, CP_UTF8), CP_ACP);
}
extern void WINDOWS_UTF8_PAGE(const unsigned int& codePage, const std::string& font)
{
    SetConsoleOutputCP(codePage);
    CONSOLE_FONT_INFOEX info = {0};
    info.cbSize = sizeof(info);
    info.dwFontSize.Y = 18;
    info.FontWeight = FW_NORMAL;
    wcscpy_s(info.FaceName, S2WS(font).c_str() /*需要是系统支持的字体*/);
    GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
}
#endif

extern bool START_WITH(const std::string& s, const std::string& prefix)
{
    if (prefix.size() > s.size())
        return false;
    return s.compare(0, prefix.size(), prefix) == 0;
}

extern bool END_WITH(const std::string& s, const std::string& suffix)
{
    if (suffix.size() > s.size())
        return false;
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

extern std::string TO_UPPER(const std::string& str)
{
    std::string ret = str;
    std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
    return ret;
}
extern std::string TO_LOWER(const std::string& str)
{
    std::string ret = str;
    std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
    return ret;
}
extern void SET_ENV(const std::string& name, const std::string& value, const bool& overwrite)
{
#if defined(_WIN32)
    SetEnvironmentVariable(UTF8_S2WS(name).c_str(), UTF8_S2WS(value).c_str());
#else
    // Linux/macOS 使用 setenv
    setenv(name.c_str(), value.c_str(), overwrite ? 1 : 0);
#endif
}
extern std::string GET_ENV(const std::string& name)
{
#if defined(_WIN32)
    // Windows：使用宽字符 API 读取环境变量
    std::wstring wName = UTF8_S2WS(name);

    DWORD size = GetEnvironmentVariable(wName.c_str(), nullptr, 0);
    if (size == 0)
    {
        return "";
    }

    std::wstring wValue(size, 0);
    GetEnvironmentVariable(wName.c_str(), wValue.data(), size);
    wValue.resize(size - 1);  // 去除终止null
    return WS2GBK_S(wValue);
#else
    // Linux/macOS：直接读取 UTF-8 环境变量
    const char* value = getenv(name.c_str());
    if (value == nullptr)
    {
        throw std::runtime_error("Environment variable not found: " + name);
    }
    return std::string(value);
#endif
}
extern std::string TRIM(const std::string& str)
{
    return TRIM_LEFT(TRIM_RIGHT(str));
}
extern std::string TRIM_LEFT(const std::string& str)
{
    auto it = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
    return std::string(it, str.end());
}
extern std::string TRIM_RIGHT(const std::string& str)
{
    auto it = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); }).base();
    return std::string(str.begin(), it);
}
extern std::string TRIM_ALL(const std::string& str)
{
    std::string ret = str;
    ret.erase(std::remove_if(ret.begin(), ret.end(), [](char c) { return std::isspace(static_cast<unsigned char>(c)); }), ret.end());
    return ret;
}
extern std::string REPLACE(const std::string& str, const std::string& find, const std::string& replace)
{
    std::string ret = str;
    size_t pos = 0;
    while ((pos = ret.find(find, pos)) != std::string::npos)
    {
        ret.replace(pos, find.length(), replace);
        pos += replace.length();  // 避免重复替换
    }
    return ret;
}
extern std::string REPLACE(const std::string& str, const char& find, const char& replace)
{
    std::string result = str;  // 创建副本以避免修改原字符串
    for (char& c : result)
    {  // 遍历每个字符
        if (c == find)
        {
            c = replace;  // 替换匹配的字符
        }
    }
    return result;
}
extern std::string REGEX_REPLACE(const std::string& str, const std::string& pattern, const std::string& replace)
{
    return std::regex_replace(str, std::regex(pattern), replace);
}
extern std::vector<std::string> SPLIT(const std::string& str, const char& delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}
extern std::vector<std::string> SPLIT(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos)
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));  // 添加最后一部分

    return tokens;
}
extern std::string WILDCARD2REGEX(const std::string& pattern)
{
    std::string regexPattern;
    for (char c : pattern)
    {
        switch (c)
        {
            case '*':
                regexPattern += ".*";
                break;  // * 匹配任意数量的字符
            case '?':
                regexPattern += ".";
                break;  // ? 匹配单个字符
            case '.':
                regexPattern += "\\.";
                break;  // . 需要转义
            case '\\':
                regexPattern += "\\\\";
                break;  // \ 需要转义
            default:
                regexPattern += c;
                break;
        }
    }
    return "^" + regexPattern + "$";  // 确保整个字符串匹配
}

extern bool ONLY_ASCII(const std::string& str)
{
    return !std::any_of(str.begin(), str.end(), [](char c) { return static_cast<unsigned char>(c) > 0x7F; });
}

extern bool IS_UTF8(const std::string& str)
{
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(str.data());
    const uint8_t* end = ptr + str.size();
    while (ptr < end)
    {
        uint8_t first_byte = *ptr;
        int n;

        if ((first_byte & 0x80) == 0x00)
        {  // 1-byte (ASCII)
            n = 1;
        }
        else if ((first_byte & 0xE0) == 0xC0)
        {  // 2-byte
            n = 2;
        }
        else if ((first_byte & 0xF0) == 0xE0)
        {  // 3-byte
            n = 3;
        }
        else if ((first_byte & 0xF8) == 0xF0)
        {  // 4-byte
            n = 4;
        }
        else
        {
            return false;
        }

        if (ptr + n > end)
        {  // 检查剩余字节是否足够
            return false;
        }

        for (int j = 1; j < n; ++j)
        {
            if ((ptr[j] & 0xC0) != 0x80)
            {
                return false;
            }
        }
        ptr += n;
    }
    return true;
}
extern bool IS_GBK(const std::string& str)
{
    for (size_t i = 0; i < str.size();)
    {
        uint8_t ch = static_cast<uint8_t>(str[i]);
        if (ch <= 0x7F)
        {
            // ASCII 字符（单字节）
            i++;
        }
        else if (ch >= 0x81 && ch <= 0xFE)
        {
            // 可能是 GBK 双字节字符
            if (i + 1 >= str.size())
            {
                return false;  // 没有足够的字节
            }
            uint8_t next = static_cast<uint8_t>(str[i + 1]);
            if ((next >= 0x40 && next <= 0x7E) || (next >= 0x80 && next <= 0xFE))
            {
                i += 2;  // 有效的双字节字符
            }
            else
            {
                return false;  // 无效的次字节
            }
        }
        else
        {
            return false;  // 无效的首字节
        }
    }
    return true;
}

extern bool HAS_UTF8(const std::string& str)
{
    if (str.empty())
        return false;

    for (size_t i = 0; i < str.length();)
    {
        unsigned char byte = static_cast<unsigned char>(str[i]);

        // 单字节字符 (ASCII)
        if ((byte & 0x80) == 0)
        {
            i++;
        }
        // 多字节UTF-8字符
        else if ((byte & 0xE0) == 0xC0)
        {  // 2字节UTF-8
            if (i + 1 < str.length() && (static_cast<unsigned char>(str[i + 1]) & 0xC0) == 0x80)
            {
                return true;  // 发现UTF-8编码序列
            }
            i++;
        }
        else if ((byte & 0xF0) == 0xE0)
        {  // 3字节UTF-8
            if (i + 2 < str.length() && (static_cast<unsigned char>(str[i + 1]) & 0xC0) == 0x80 && (static_cast<unsigned char>(str[i + 2]) & 0xC0) == 0x80)
            {
                return true;  // 发现UTF-8编码序列
            }
            i++;
        }
        else if ((byte & 0xF8) == 0xF0)
        {  // 4字节UTF-8
            if (i + 3 < str.length() && (static_cast<unsigned char>(str[i + 1]) & 0xC0) == 0x80 && (static_cast<unsigned char>(str[i + 2]) & 0xC0) == 0x80 && (static_cast<unsigned char>(str[i + 3]) & 0xC0) == 0x80)
            {
                return true;  // 发现UTF-8编码序列
            }
            i++;
        }
        else
        {
            i++;  // 非UTF-8字节，继续检查下一个
        }
    }
    return false;
}

extern bool HAS_GBK(const std::string& str)
{
    if (str.empty())
        return false;

    for (size_t i = 0; i < str.length();)
    {
        unsigned char byte = static_cast<unsigned char>(str[i]);

        // 单字节ASCII字符
        if (byte < 0x80)
        {
            i++;
        }
        // 双字节GBK字符
        else if (byte >= 0x81 && byte <= 0xFE)
        {
            if (i + 1 < str.length())
            {
                unsigned char secondByte = static_cast<unsigned char>(str[i + 1]);
                if ((secondByte >= 0x40 && secondByte <= 0x7E) || (secondByte >= 0x80 && secondByte <= 0xFE))
                {
                    return true;  // 发现GBK编码序列
                }
            }
            i++;
        }
        else
        {
            i++;  // 非GBK字节，继续检查下一个
        }
    }
    return false;
}
extern double RANDOM(const double& min, const double& max)
{
    // 1. 创建随机设备（用于种子）
    std::random_device rd;

    // 2. 创建生成器（常用 mt19937）
    std::mt19937 gen(rd());

    // 3. 创建分布（例如 1~100 的整数）
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}
extern std::vector<double> RANDOM(const double& min, const double& max, const size_t& n)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);
    std::vector<double> ret;
    ret.resize(n);
    for (size_t i = 0; i < n; ++i)
    {
        ret[i] = dis(gen);
    }
    return ret;
}
extern size_t MEM_PAGE_SIZE()
{
#if _WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    DWORD page_size = si.dwPageSize;
    return page_size;
#else
    long page_size = sysconf(_SC_PAGESIZE);
    return page_size;
#endif
}