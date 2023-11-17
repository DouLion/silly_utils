/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/11/17 15:07
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_PINYIN_H
#define SILLY_UTILS_SILLY_PINYIN_H
#include <iostream>

class silly_pinyin
{
public:
    std::string chinese_to_pinyin(const std::string& dest_chinese);

};

#endif //SILLY_UTILS_SILLY_PINYIN_H
