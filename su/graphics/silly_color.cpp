/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/17 20:42
 * @version: 1.0.1
 * @description: silly_color 类实现
 */
#include "silly_color.h"

#define MAX_ENCODE_V 12799.9999
#define MIN_ENCODE_V (-12799.9999)

int suColor::channels(const eColorType& t)
{
    switch (t)
    {
        case RGB:
            return 3;
        case RGBA:
            return 4;
        case GRAY:
            return 1;
        case GRAYA:
            return 2;
        default:
            return 0;
    }
}

bool suColor::hex2argb(const char* color)
{
    unsigned int v = 0;
    if (1 != sscanf(color, "%x", &v) || strlen(color) != 8)
    {
        return false;
    }
    // 或者 左移然后 和 0xFF 做与
    blue = (v << 24) >> 24;
    green = (v << 16) >> 24;
    red = (v << 8) >> 24;
    alpha = v >> 24;

    return true;
}

bool suColor::hex2rgb(const char* color)
{
    unsigned int v = 0;
    if (1 != sscanf(color, "%x", &v) || strlen(color) != 6)
    {
        return false;
    }
    blue = (v << 16) >> 16;
    green = (v << 8) >> 16;
    red = v >> 16;
    return true;
}

bool suColor::hex2rgba(const char* color)
{
    unsigned int v = 0;
    if (1 != sscanf(color, "%x", &v) || strlen(color) != 8)
    {
        return false;
    }
    alpha = (v << 24) >> 24;
    blue = (v << 16) >> 24;
    green = (v << 8) >> 24;
    red = v >> 24;
    return true;
}

double suColor::vdecode() const
{
    double ret = 0;
    ret += red * 1.e2;
    ret += green;
    ret += blue / 100.0;
    ret += alpha / 10000.0;
    return ret + MIN_ENCODE_V;
}

void suColor::vencode(const double& vv)
{
    double v = vv - MIN_ENCODE_V;
    red = static_cast<unsigned char>(static_cast<int>(v / 1e2));
    green = static_cast<unsigned char>(static_cast<int>(v) % 100);
    blue = static_cast<unsigned char>(static_cast<int>(v * 100) % 100);
    alpha = static_cast<unsigned char>(static_cast<int>(v * 10000) % 100);
}

std::string suColor::argb2hex() const
{
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0');
    oss << std::setw(2) << alpha;
    oss << std::setw(2) << red;
    oss << std::setw(2) << green;
    oss << std::setw(2) << blue;
    return oss.str();
}

std::string suColor::rgb2hex() const
{
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0');
    oss << std::setw(2) << red;
    oss << std::setw(2) << green;
    oss << std::setw(2) << blue;
    return oss.str();
}

std::string suColor::rgba2hex() const
{
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0');
    oss << std::setw(2) << red;
    oss << std::setw(2) << green;
    oss << std::setw(2) << blue;
    oss << std::setw(2) << alpha;
    return oss.str();
}

std::string suColor::abgr2hex() const
{
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0');
    oss << std::setw(2) << alpha;
    oss << std::setw(2) << blue;
    oss << std::setw(2) << green;
    oss << std::setw(2) << red;
    return oss.str();
}
std::string suColor::bgra2hex() const
{
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0');
    oss << std::setw(2) << blue;
    oss << std::setw(2) << green;
    oss << std::setw(2) << red;
    oss << std::setw(2) << alpha;
    return oss.str();
}

std::string suColor::bgr2hex() const
{
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0');
    oss << std::setw(2) << blue;
    oss << std::setw(2) << green;
    oss << std::setw(2) << red;
    return oss.str();
}