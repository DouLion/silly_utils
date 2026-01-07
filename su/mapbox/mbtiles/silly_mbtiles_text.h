/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: silly_mbtiles_text.h
 * @description: silly_mbtiles_text 类声明
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#ifndef SILLY_MBTILES_TEXT_H
#define SILLY_MBTILES_TEXT_H
#include <su_macro.h>
class suMBTileText
{
  public:
    static std::string CheckUTF8(std::string text);
    static const char *UTF8Next(const char *s, long *c);
    static std::string Truncate16(std::string const &s, size_t runes);
    static int IntegerZoom(std::string where, std::string text);
    static std::string FormatCommandLine(int argc, char **argv);
};

#endif  // SILLY_MBTILES_TEXT_H
