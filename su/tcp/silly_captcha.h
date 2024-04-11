/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/4/11 14:51
 * @version: 1.0.1
 * @description: 验证码
 */
#ifndef SILLY_UTILS_SILLY_CAPTCHA_H
#define SILLY_UTILS_SILLY_CAPTCHA_H
#include <iostream>

class silly_captcha
{
  public:
    /// <summary>
    /// 获取指定长度的数字+字母字符串的验证码
    /// </summary>
    /// <param name="answer">字符串内容</param>
    /// <param name="image">图像</param>
    /// <param name="letter_len">指定字母长度</param>
    /// <param name="font_path">指定字体文件位置</param>
    /// <returns></returns>
    static bool get_letter_captcha(std::string& answer, std::string& image, const size_t& letter_len = 4, const std::string font_path="./RubikMicrobe.ttf");

    /// <summary>
    /// 获取一个简单10以内加减乘计算的验证码
    /// </summary>
    /// <param name="answer">计算结果</param>
    /// <param name="image">图像</param>
    /// <param name="font_path">指定字体文件位置</param>
    /// <returns></returns>
    static bool get_math_calc_captcha(std::string& answer, std::string& image, const std::string font_path="./RubikMicrobe.ttf");

  private:
    static std::string generate_letter_captcha(const size_t& letter_len = 4);
    static std::string generate_math_calc_captcha(std::string& result);

};

#endif  // SILLY_UTILS_SILLY_CAPTCHA_H
