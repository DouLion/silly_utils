/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/11/17 14:03
 * @version: 1.0.1
 * @description: grib2格式的读取, 参照 https://github.com/weathersource/grib_api/tree/master
 */
#ifndef SILLY_GRIB2_H
#define SILLY_GRIB2_H
#include <files/silly_file.h>
#if SU_THIRD_SUPPORT_ECCODES
#include <eccodes.h>
#endif

class suGrib2
{
  public:
    /// <summary>
    /// grib2的帧数据
    /// </summary>
    class Frame
    {
      public:
        bool is_valid{false};

        std::vector<double> data;
    };
    /// <summary>
    /// 从GRIB2文件中读取某一帧数据
    /// </summary>
    /// <param name="file">GRIB2文件路径</param>
    /// <param name="grb">用于存储读取到的帧数据</param>
    /// <param name="fidx">要读取的帧索引，默认为0</param>
    /// <returns>读取成功返回true，否则返回false</returns>
    static bool read(const suPath& file, suGrib2::Frame& grb, const size_t& fidx = 0);

    /// <summary>
    /// 从GRIB2文件中读取所有帧数据
    /// </summary>
    /// <param name="file">GRIB2文件路径</param>
    /// <param name="msgf_grb">用于存储所有帧数据的映射，键为帧索引</param>
    /// <returns>读取成功返回true，否则返回false</returns>
    static bool read(const suPath& file, std::map<size_t, suGrib2::Frame>& msgf_grb);

  private:
    /// <summary>
    /// 打开GRIB2文件并获取句柄
    /// </summary>
    /// <param name="file">GRIB2文件路径</param>
    /// <param name="fileHdl">文件句柄指针</param>
    /// <param name="grbCtx">ECCODES上下文句柄指针</param>
    /// <param name="grbHdl">GRIB2消息句柄指针</param>
    /// <returns>打开成功返回true，否则返回false</returns>
    static bool open_grib2_handle(const suPath& file, void** fileHdl, void** grbCtx, void** grbHdl);

    /// <summary>
    /// 加载GRIB2帧数据
    /// </summary>
    /// <param name="grbHdl">GRIB2消息句柄</param>
    /// <param name="grb">用于存储读取到的帧数据</param>
    /// <param name="skip">是否跳过当前帧，默认为true</param>
    /// <returns>加载成功返回true，否则返回false</returns>
#if SU_THIRD_SUPPORT_ECCODES
    // 只有在启用ECCODES时才定义此函数，并使用codes_handle*类型
    static bool load_grib2_frame(codes_handle* grbHdl, suGrib2::Frame& grb, const bool& skip = true);
#else
    // 如果不启用ECCODES，提供一个空实现，避免编译错误
    static bool load_grib2_frame(void* grbHdl, suGrib2::Frame& grb, const bool& skip = true)
    {
        return false;
    }
#endif

    /// <summary>
    /// 关闭GRIB2文件句柄
    /// </summary>
    /// <param name="fileHdl">文件句柄指针</param>
    /// <param name="grbCtx">ECCODES上下文句柄指针</param>
    /// <param name="grbHdl">GRIB2消息句柄指针</param>
    /// <returns>关闭成功返回true，否则返回false</returns>
    static bool close_grib2_handle(void* fileHdl, void* grbCtx, void* grbHdl);
};

#endif  // SILLY_GRIB2_H
