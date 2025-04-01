/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/17 20:42
 * @version: 1.0.1
 * @description: color 类声明
 */
#ifndef SILLY_UTILS_SILLY_COLOR_H
#define SILLY_UTILS_SILLY_COLOR_H

#include <su_marco.h>
// TODO: 改为不依赖 非标准库实现
#include <string/silly_format.h>
#define COLOR_MAX_D 255.0
namespace silly
{
class color
{
  public:
    color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : red(r), green(g), blue(b), alpha(a)
    {
    }

    color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b)
    {
    }

    color(uint8_t g, uint8_t a) : alpha(a), gray(g)
    {
    }

    color(uint8_t g) : gray(g)
    {
    }
    color()
    {
    }

  public:
    enum type : uint8_t
    {
        eptRGB = 1,   // PNG_COLOR_TYPE_RGB,
        eptRGBA = 2,  // PNG_COLOR_TYPE_RGB_ALPHA,
        eptGRAY = 3,  // PNG_COLOR_TYPE_GRAY,
        eptGRAYA = 4  // PNG_COLOR_TYPE_GRAY_ALPHA
    };

  public:
    static int channels(const type& t);

    /// <summary>
    /// 从字符串加载  如 ABE0457B
    /// </summary>
    /// <param name="color"></param>
    bool hex2argb(const char* hex);
    bool hex2rgb(const char* hex);
    bool hex2rgba(const char* hex);

    bool hex2argb(const std::string& hex)
    {
        return hex2argb(hex.c_str());
    }

    bool hex2rgb(const std::string& hex)
    {
        return hex2rgb(hex.c_str());
    }

    bool hex2rgba(const std::string& hex)
    {
        return hex2rgba(hex.c_str());
    }

    std::string argb2hex()
    {
        return silly_format::format("{:02X}{:02X}{:02X}{:02X}", alpha, red, green, blue);
    }

    std::string rgb2hex()
    {
        return silly_format::format("{:02X}{:02X}{:02X}", red, green, blue);
    }

    std::string rgba2hex()
    {
        return silly_format::format("{:02X}{:02X}{:02X}{:02X}", red, green, blue, alpha);
    }

    std::string abgr2hex()
    {
        return silly_format::format("{:02X}{:02X}{:02X}{:02X}", alpha, blue, green, red);
    }
    std::string bgra2hex()
    {
        return silly_format::format("{:02X}{:02X}{:02X}{:02X}", blue, green, red, alpha);
    }

    std::string bgr2hex()
    {
        return silly_format::format("{:02X}{:02X}{:02X}", blue, green, red);
    }
        
    /// <summary>
    /// 无损表示-12799.9999 到 12799.9999 之间的值
    /// 这个数值范围在水文,气象领域内应该是足够用的
    /// 使用改实现方式可以在shader方便恢复
    /// </summary>
    /// <param name="v"></param>
    void vencode(const double& v);

    /// <summary>
    /// 恢复为double
    /// </summary>
    /// <returns></returns>
    double vdecode() const;

  public:
    uint8_t gray{0};
    uint8_t red{0};
    uint8_t green{0};
    uint8_t blue{0};
    uint8_t alpha{0};
};
}  // namespace silly
#endif  // SILLY_UTILS_SILLY_COLOR_H

/* shader中的恢复函数

// 能无损表示-12799.9999 到 12799.9999 之间的值
float signed_vec4tofloat(vec4 px){

    float value = px.r *25500.0  + px.g*255.0 +px.b*2.55+ px.a * 0.0255 - 12799.9999;

    return value;
}

// 能无损表示 0 到 25599.9999 之间的值
float unsigned_vec4tofloat(vec4 px){
    float value = px.r *25500.0  + px.g*255.0 +px.b*2.55+ px.a * 0.0255;
    return value;
}

*/