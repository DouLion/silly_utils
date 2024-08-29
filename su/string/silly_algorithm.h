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
    static std::vector<std::string> split(const std::string& str, const char& c);
    static std::vector<std::string> split(const std::string& str, const std::string& s);
    static std::string replace(const std::string& src, const std::string& find, const std::string rep);

    static size_t count_with_chinese_character(const std::string& u8str);
};

#endif  // SILLY_UTILS_SILLY_ALGORITHM_H
