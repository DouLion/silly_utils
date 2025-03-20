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


int silly::color::channels(const silly::color::type& t)
{
    switch (t)
    {
        case eptRGB:
            return 3;
        case eptRGBA:
            return 4;
        case eptGRAY:
            return 1;
        case eptGRAYA:
            return 2;
        default:
            return 0;
    }
}

bool silly::color::hex2argb(const char* color)
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

bool silly::color::hex2rgb(const char* color)
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

 bool silly::color::hex2rgba(const char* color)
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