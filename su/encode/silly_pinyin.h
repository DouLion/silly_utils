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
#include <su_marco.h>

class silly_pinyin
{
public:
    /// <summary>
    /// 将输入的gbk编码的中文字符转为对应的拼音, 如输入"中AA,BBCC，文" 输出为 ZhongAA,BBCCWen
    /// </summary>
    /// <param name="gbk_chinese"></param>
    /// <returns></returns>
    static std::string chinese_to_pinyin(const std::string &gbk_chinese);
};

#endif // SILLY_UTILS_SILLY_PINYIN_H
