/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-02
 * @file: silly_jpeg.h
 * @description: silly_jpeg 类声明
 * @version: v1.0.1 2025-01-02 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_JPEG_H
#define SILLY_UTILS_SILLY_JPEG_H
#include "jpeglib.h"
#include "jerror.h"
#include <setjmp.h>
#include <image/silly_image_base.h>
namespace silly
{
namespace jpeg
{
class data : public image::base
{
  public:
    data()
    {
        HEADER = {0xFF, 0xD8};
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

    silly::jpeg::data &silly::jpeg::data::operator=(const silly::jpeg::data &rh);

  protected:
    int m_quality{75};
};

}  // namespace jpeg
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_JPEG_H
