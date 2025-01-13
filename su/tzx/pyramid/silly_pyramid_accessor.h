
/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 9:49
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_PYRAMID_ACCESSOR_H
#define SILLY_UTILS_SILLY_PYRAMID_ACCESSOR_H

#include <tzx/pyramid/silly_pyramid_info.h>
#include <tzx/pyramid/silly_pyramid_index.h>
#include <tzx/pyramid/silly_pyramid_data.h>
namespace silly
{
namespace pyramid
{

struct readopt
{
    std::string root;
    bool mmap = true;
};

struct writeopt
{
    std::string root;
    bool mmap = false;
    uint8_t blayer = 0;
    uint8_t elayer = 0;
    char vdata[len::VER] = {0x01, 0x00, 0x00, 0x00};
    char vinfo[len::VER] = {0x01, 0x00, 0x00, 0x00};
    char vindex[len::VER] = {0x02, 0x00, 0x00, 0x00};
    std::string src = "TianZhiXiang";
    std::string proj = "Default Projection";
    silly_rect bound;
    info::tile_format format;
    bool clean = true;  // 清除原文件 或者将原文件备份
};
class accessor
{
  public:
    accessor(void) = default;
    bool begin_read(const readopt& opt);
    bool begin_write(const writeopt& opt);
    bool end_read();
    bool end_write();
    bool open(const std::string& root, const silly::file::memory_map::access_mode& mode = silly::file::memory_map::access_mode::Read, bool usemmap = false);
    void close();

    /// <summary>
    /// 读取指定块的数据内容
    /// </summary>
    /// <param name="layer">块所在层号 0 - 24</param>
    /// <param name="row">块所在行号 0 - 2^layer-1</param>
    /// <param name="col">块所在列号 0 - 2^layer-1</param>
    /// <param name="size">块数据大小</param>
    /// <returns>数据内容</returns>
    std::string read(const block& blk);

    /// <summary>
    /// 向指定块写入数据
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <param name="size"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    bool write(const block& blk);

    /// <summary>
    /// 将版本1的金字塔转为版本2的金字塔
    /// </summary>
    /// <param name="dst">目标目录</param>
    /// <returns></returns>
    bool rebuild_to_v2(const std::string& dst);

    bool backup();

    std::string err();

  private:
    bool open();

  private:
    silly::file::memory_map::access_mode m_mode = silly::file::memory_map::access_mode::Read;
    std::string m_root;
    info m_info;
    data m_data;
    std::string m_err;
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PYRAMID_ACCESSOR_H
