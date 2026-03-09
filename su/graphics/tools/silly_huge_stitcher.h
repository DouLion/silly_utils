/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-11-05
 * @file: silly_huge_stitcher.h
 * @description:  超大图像拼接
 * @version: v1.0.1 2024-11-05 dou li yang
 */
#ifndef SILLY_HUGE_STITCHER_H
#define SILLY_HUGE_STITCHER_H
#include <su_macro.h>
#include <graphics/silly_png.h>
#include <geo/silly_geo_utils.h>
class suHugeStitcher
{
  public:
    struct Pos
    {
        // 图像上的偏移位置
        int64_t xOffset = 0;
        int64_t yOffset = 0;
    };
    struct Image
    {
        std::string data;
        int64_t width = 0;
        int64_t height = 0;
    };
    struct Block
    {
        Pos src;  // 从data图像上截取的位置
        Pos dst;  // 写入目标的位置
        // 裁剪区域的宽高
        int64_t xSize = 0;
        int64_t ySize = 0;
        Image dIMG;
    };

  public:
    /// <summary>
    ///
    /// </summary>
    /// <param name="file">输出文件路径</param>
    /// <param name="nXSize">宽</param>
    /// <param name="nYSize">高</param>
    /// <param name="nBands">PNG通道数</param>
    /// <returns></returns>
    bool create(const suPath& file, const int& nXSize, const int& nYSize, const int& nBands);

    /// <summary>
    /// 检查图像的宽高波段数
    /// </summary>
    /// <param name="data"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="bands"></param>
    /// <returns></returns>
    bool check(const std::string& data, int& width, int& height, int& bands);

    /// <summary>
    /// 矩形裁剪源图像,贴到目标图像上,不会做缩放
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    bool attach(const Block& blk);

    /// <summary>
    /// 释放占用资源,需要手动调用
    /// </summary>
    void release();

#ifndef NDEBUG
    // 用法示例1: 根据经纬范围,切割tile,拼接完整图像
    void test_case1();
    // 用法示例2,根据经纬范围,保留完整tile,拼接为大图像
    void test_case2();
#endif

  private:
#if SU_THIRD_SUPPORT_GDAL
    GDALDataset* m_dateset = nullptr;
#endif
    bool m_init = false;
    int m_bands = 0;
    int m_width = 0;
    int m_height = 0;
    std::mutex m_mutex;
};

#endif  // SILLY_HUGE_STITCHER_H