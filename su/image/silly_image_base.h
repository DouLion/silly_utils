/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-02
 * @file: silly_image_base.h
 * @description: silly_image_base 类声明
 * @version: v1.0.1 2025-01-02 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_IMAGE_BASE_H
#define SILLY_UTILS_SILLY_IMAGE_BASE_H
#include <image/silly_color.h>
namespace silly
{
namespace image
{
class base
{
  public:
    /// <summary>
    /// 创建指定宽高和颜色类型的空图像,颜色为全为0
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    virtual bool create(const size_t& width, const size_t& height, const silly::color::type& type, const uint8_t& depth = 8) = 0;

    /// <summary>
    /// 从文件读取图像
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    virtual bool read(const std::string& file) = 0;

    /// <summary>
    /// 将图像写入文件
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    virtual bool write(const std::string& file) const = 0;

    /// <summary>
    /// 设置颜色
    /// </summary>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <param name="pixel"></param>
    virtual void pixel(const size_t& row, const size_t& col, const silly::color& pixel) = 0;

    /// <summary>
    /// 获取指定位置的颜色
    /// </summary>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <returns></returns>
    virtual silly::color pixel(const size_t& row, const size_t& col) const = 0;

    /// <summary>
    /// 从二进制数据创建图像
    /// </summary>
    /// <param name="bin"></param>
    /// <returns></returns>
    virtual bool decode(const std::string& bin) = 0;

    /// <summary>
    /// 将图像转换为二进制数据
    /// </summary>
    /// <returns></returns>
    virtual std::string encode() const = 0;

    /// <summary>
    /// 判断是否为有效的图像
    /// </summary>
    /// <param name="bin"></param>
    /// <returns></returns>
    bool valid(const std::string& bin);
    bool valid(const char* data, const size_t len);

    /// <summary>
    /// 判断是否为空
    /// </summary>
    /// <returns></returns>
    bool empty();

    /// <summary>
    /// 主动释放内存
    /// </summary>
    /// <returns></returns>
    void release();

    size_t width() const
    {
        return m_width;
    }

    size_t height() const
    {
        return m_height;
    }
    uint8_t pixel_size() const
    {
        return m_pixel_size;
    }
    uint8_t depth() const
    {
        return m_depth;
    }
    silly::color::type type() const
    {
        return m_type;
    }
    uint8_t* bytes() const
    {
        return m_bytes;
    }

    std::string err() const
    {
        return m_err;
    }

  protected:
    uint8_t* m_bytes{nullptr};  // 二进制数据
    size_t m_width{0};          // 宽度
    size_t m_height{0};         // 高度
    uint8_t m_channels{0};      // 通道数
    uint8_t m_depth{0};         // 位深度
    uint8_t m_pixel_size{0};
    silly::color::type m_type{2};  // 颜色类型
    std::vector<uint8_t> HEADER;   // 固定头部,由于判断类型
    std::string m_err;
    std::vector<unsigned char*> m_nbytes;
};
}  // namespace image

}  // namespace silly

#endif  // SILLY_UTILS_SILLY_IMAGE_BASE_H
