/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/2/2 10:09
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_ALGORITHM_H
#define SILLY_UTILS_SILLY_ALGORITHM_H
#include <su_marco.h>
#include <vector>

class silly_string_algo
{
  public:
    /// <summary>
    ///
    /// </summary>
    /// <param name="str"></param>
    /// <param name="c"></param>
    /// <returns></returns>
    static std::vector<std::string> split(const std::string& str, const char& c);

    /// <summary>
    ///
    /// </summary>
    /// <param name="str"></param>
    /// <param name="str"></param>
    /// <returns></returns>
    static std::vector<std::string> split(const std::string& str, const std::string& s);

    /// <summary>
    /// 在src中,用 rep 替换find
    /// </summary>
    /// <param name="str"></param>
    /// <param name="find"></param>
    /// <param name="rep"></param>
    /// <returns></returns>
    static std::string replace(const std::string& src, const std::string& find, const std::string rep);

    /// <summary>
    /// 统计utf8字符串中的中文字符个数
    /// </summary>
    /// <param name="u8str"></param>
    /// <returns></returns>
    static size_t count_with_chinese_character(const std::string& u8str);

    /// <summary>
    /// 去除左边空格
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static std::string ltrim(std::string str);

    /// <summary>
    /// 去除右边空格
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static std::string rtrim(std::string str);

    /// <summary>
    /// 去除左右空格
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static std::string trim(std::string str);

    /// <summary>
    /// 去除全部空格
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static std::string trim_all(std::string str);
};

#endif  // SILLY_UTILS_SILLY_ALGORITHM_H
