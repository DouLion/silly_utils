/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/17 20:42
 * @version: 1.0.1
 * @description: silly_color 类声明
 */
#ifndef SILLY_UTILS_SILLY_COLOR_H
#define SILLY_UTILS_SILLY_COLOR_H

#include <su_marco.h>

class silly_color
{
  public:
    silly_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : red(r), green(g), blue(b), alpha(a), gray(0)
    {
    }

    silly_color(unsigned char r, unsigned char g, unsigned char b) : red(r), green(g), blue(b), alpha(0), gray(0)
    {
    }

    silly_color(unsigned char g, unsigned char a) : red(0), green(0), blue(0), alpha(a), gray(g)
    {
    }

    silly_color(unsigned char g) : red(0), green(0), blue(0), alpha(0), gray(g)
    {
    }
    silly_color() : red(0), green(0), blue(0), alpha(0), gray(0)
    {
    }

    /// <summary>
    /// 从字符串加载  如 ABE0457B
    /// </summary>
    /// <param name="color"></param>
    bool from_hex_argb(const char *color)
    {
        unsigned int v = 0;
        if (1 != sscanf(color, "%x", &v))
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

    bool from_hex_rgb(const char *color)
    {
        unsigned int v = 0;
        if (1 != sscanf(color, "%x", &v))
        {
            return false;
        }
        blue = (v << 16) >> 16;
        green = (v << 8) >> 16;
        red = v >> 16;
        return true;
    }

    unsigned char gray{0};
    unsigned char red{0};
    unsigned char green{0};
    unsigned char blue{0};
    unsigned char alpha{0};
};

#endif  // SILLY_UTILS_SILLY_COLOR_H
