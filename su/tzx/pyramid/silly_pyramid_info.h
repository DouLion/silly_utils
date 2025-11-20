/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 9:48
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#ifndef SILLY_PYRAMID_INFO_H
#define SILLY_PYRAMID_INFO_H

#include <tzx/pyramid/silly_pyramid_base.h>
#include <geo/silly_geo.h>
enum ePyramidTileFormat
{
    jpeg = 1,
    png,
    terrain,
    pbf
};

class TzxPyramidInfo : public TzxPyramidBase
{
  public:
    TzxPyramidInfo();

    /// <summary>
    /// 打开文件, 如果是读模式,读完会自动关闭
    /// </summary>
    /// <param name="file"></param>
    /// <param name="mode"></param>
    /// <param name="usemmap"></param>
    /// <returns></returns>
    bool open(const std::filesystem::path& file, const eMMFMode& mode, const bool& usemmap) override;

    bool read();

    void close();

    void source(const std::string& src);

    void project(const std::string& proj);

    void bound(const std::string& bd);
    void bound(const suRect& bd);

    void format(const ePyramidTileFormat& fmt);

    void format(const std::string fmt);

    std::string source();
    std::string project();
    suRect bound();

    std::string format();

  private:
    /// <summary>
    /// 写入信息
    /// </summary>
    void write_info();

  protected:
    char m_source[SU_PYRAMID_INF_SRC_LEN] = {0};
    /// Projection TzxPyramidInfo.
    char m_projection[SU_PYRAMID_INF_PROJ_LEN] = {0};
    /// Bound TzxPyramidInfo.
    char m_bound[SU_PYRAMID_INF_BOUND_LEN] = {0};
    /// version 1
    int64_t m_bt = 0;
    int64_t m_et = 0;
    int32_t m_interval = 0;
    /// version 2
    /// Tiling format TzxPyramidInfo.
    char m_format[SU_PYRAMID_INF_FMT_LEN] = {0};
};

#endif  // SILLY_PYRAMID_INFO_H
