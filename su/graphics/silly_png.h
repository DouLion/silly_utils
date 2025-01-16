/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-02
 * @file: silly_png.h
 * @description: silly_png 类声明
 * @version: v1.0.1 2025-01-02 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_PNG_H
#define SILLY_UTILS_SILLY_PNG_H
#include <graphics/silly_color.h>
#include <graphics/silly_image_base.h>

namespace silly
{
namespace png
{
/// 1x1的空白png图片
const static std::vector<uint8_t> EMPTY_1X1_RGBA = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0x15, 0xC4, 0x89, 0x00,
                                                    0x00, 0x00, 0x0B, 0x49, 0x44, 0x41, 0x54, 0x08, 0x99, 0x63, 0x60, 0x00, 0x02, 0x00, 0x00, 0x05, 0x00, 0x01, 0x62, 0x55, 0x32, 0x88, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};

const static std::string EMPTY_1X1_RGBA_STR(EMPTY_1X1_RGBA.begin(), EMPTY_1X1_RGBA.end());

class data : public image::base
{
  public:
    data()
    {
        HEADER = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    }
    /// <summary>
    /// 创建指定宽高和颜色类型的空图像,颜色为全为0
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    bool create(const size_t &width, const size_t &height, const silly::color::type &type, const uint8_t &depth = 8) override;

    /// <summary>
    /// 从文件读取图像
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    bool read(const std::string &file) override;

    /// <summary>
    /// 将图像写入文件
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    bool write(const std::string &file) const override;

    /// <summary>
    /// 设置颜色
    /// </summary>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <param name="pixel"></param>
    void pixel(const size_t &row, const size_t &col, const silly::color &pixel) override;

    /// <summary>
    /// 获取指定位置的颜色
    /// </summary>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <returns></returns>
    silly::color pixel(const size_t &row, const size_t &col) const override;

    /// <summary>
    /// 从二进制数据创建图像
    /// </summary>
    /// <param name="bin"></param>
    /// <returns></returns>
    bool decode(const std::string &bin) override;

    /// <summary>
    /// 将图像转换为二进制数据
    /// </summary>
    /// <returns></returns>
    std::string encode() const override;

    uint8_t **nbytes() const
    {
        return (uint8_t **)&m_nbytes;
    }

    uint8_t **nbytes()
    {
        return (uint8_t **)&m_nbytes;
    }

    silly::png::data &silly::png::data::operator=(const silly::png::data &rh);

  protected:
    uint32_t m_pixel_size{0};
};

class tools
{
  public:
    /// <summary>
    /// RGB图像转RGBA
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <returns></returns>
    static bool rgb2rgba(const data &src, data &dst, const uint8_t &alpha = 255);

    /// <summary>
    /// RGBA图像转RGB
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <returns></returns>
    static bool rgba2rgb(const data &src, data &dst);
};
}  // namespace png
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PNG_H
