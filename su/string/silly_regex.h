/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/2 14:04
 * @version: 1.0.1
 * @description: c++ 正则表达式工具
 */
#ifndef SILLY_UTILS_SILLY_REGEX_H
#define SILLY_UTILS_SILLY_REGEX_H

#include <regex>
#include <encode/silly_encode.h>

// #define IS_##varname( str) std::regex_match( str, std::regex(varname))

namespace silly
{
namespace str
{
class valid
{
  public:
    /// <summary>
    /// 是否是QQ格式
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static bool QQ(const std::string& str);

    /// <summary>
    /// 是否是邮箱格式
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static bool email(const std::string& str);

    /// <summary>
    /// 是否是大陆身份证格式
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static bool mainland_id(const std::string& str);

    /// <summary>
    /// 是否是大陆手机号格式
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static bool mainland_mobile(const std::string& str);

    /// <summary>
    /// 是否是大陆邮编格式
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static bool mainland_post(const std::string& str);

    /// <summary>
    /// 是否是大陆车牌号格式
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static bool mainland_vehicle(const std::string& str);
};
}  // namespace str
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_REGEX_H
