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
#include <log/silly_log.h>
#include <singleton/silly_singleton.h>

class silly_captcha : public silly_singleton<silly_captcha>
{
    friend class silly_singleton<silly_captcha>;

  public:
    /// <summary>
    /// 添加字体
    /// </summary>
    /// <param name="name">字体名称</param>
    /// <param name="path">字体文件路径</param>
    /// <returns></returns>
    bool add_font(const std::string& name, const std::string& path);

    /// <summary>
    /// 获取指定长度的数字+字母字符串的验证码
    /// </summary>
    /// <param name="answer">字符串内容</param>
    /// <param name="image">图像</param>
    /// <param name="letter_len">指定字母长度</param>
    /// <param name="font_path">指定字体文件位置</param>
    /// <returns></returns>
    bool get_letter_captcha(std::string& answer, std::string& image, const size_t& letter_len = 4, const std::string font_name = "");

    /// <summary>
    /// 获取一个简单10以内加减乘计算的验证码
    /// </summary>
    /// <param name="answer">计算结果</param>
    /// <param name="image">图像</param>
    /// <param name="font_path">指定字体文件位置</param>
    /// <returns></returns>
    bool get_math_calc_captcha(std::string& answer, std::string& image, const std::string font_name = "");

    /// <summary>
    /// 比较是否相等,不区分大小写
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <returns></returns>
    bool euqal(const std::string& src, const std::string& dst);

  private:
    silly_captcha();
    ~silly_captcha();

  private:
    std::string generate_letter_captcha(const size_t& letter_len = 4);
    std::string generate_math_calc_captcha(std::string& result);
};

#endif  // SILLY_UTILS_SILLY_CAPTCHA_H
