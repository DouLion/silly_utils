/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/24 15:43
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_TZX_GRID_H
#define SILLY_UTILS_SILLY_TZX_GRID_H

#include <su_marco.h>
#include <math/silly_matrix.h>

#define SILLY_TZX_GRID_FILE_SUFFIX ".rgrid"

/// 此函数中使用的float是从存储空间大小考虑
/// 大多数情况下, float的数据范围和精度都已经足够,
/// 所以, 这里使用float, 减少内存和序列化之后的空间占用
class silly_tzx_grid
{
  public:
    silly_tzx_grid();

    // 从文件读写
    bool read(const std::filesystem::path& file);
    bool save(const std::filesystem::path& file);

    /// <summary>
    /// 将多个网格数据拼接为一个,重叠部分采用最大值
    /// TODO: 可以使用多线程优化,并且 以每个grids为主, 减少循环次数
    /// 这里限制delta的原因是,希望使用同样的一套数据,所有的数据使用相同的尺度
    /// </summary>
    /// <param name="grids">多个网格点</param>
    /// <param name="boundary">目标的范围</param>
    /// <param name="delta">每个格点的大小</param>
    void puzzle(const std::vector<silly_tzx_grid>& grids, const silly_rect& boundary, const float& d = 0.0025);

    silly_tzx_grid& operator=(const silly_tzx_grid& rh);

    /// 网格范围和宽高是否一致
    bool same(const silly_tzx_grid& rh) const;

    bool valid() const;

    /// 拷贝整个对象
    silly_tzx_grid copy() const;

    /// 拷贝第i个网格数据
    silly_tzx_grid copy(const size_t& i) const;

    void copy_info(const silly_tzx_grid& rh);

    su::FMatrix& frame(const size_t& i);

    /// 总帧数
    size_t frame_num() const;

    bool set(const size_t& i, const silly_tzx_grid& rh);
    /// 添加一个网格数据
    bool add(const silly_tzx_grid& rh);
    bool add(const su::FMatrix grid);
    bool set(const std::vector<su::FMatrix> grids);

    size_t row() const;
    void row(const size_t& r);

    size_t col() const;
    void col(const size_t& c);

    float xdelta() const;
    float ydelta() const;
    // void delta(const float& d);

    silly_rect rect() const;
    void rect(const silly_rect& boundary);

    /// <summary>
    /// 释放内存
    /// </summary>
    void release();

  private:
    /// <summary>
    ///
    /// </summary>
    /// <param name="buff">输出: 数据区域</param>
    /// <returns></returns>
    bool serialize(std::string& buff);
    bool serialize_v1(std::string& buff);

    bool serialize_v2(std::string& buff);

    /// <summary>
    ///
    /// </summary>
    /// <param name="buff">输入: 数据区域</param>
    /// <returns></returns>
    bool unserialize(const std::string& buff);
    bool unserialize_v1(const std::string& buff);

    bool unserialize_v2(const std::string& buff);

    /// <summary>
    /// 用lz4压缩数据
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <returns></returns>
    bool lz4_cps_data(const std::string& src, std::string& dst) const;

    /// <summary>
    /// 用lz4解压数据块
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <returns></returns>
    bool lz4_dcps_data(const std::string& src, std::string& dst) const;

  protected:
    size_t m_total{0};
    float m_left{0.};
    float m_right{0.};
    float m_top{0.};
    float m_bottom{0.};

    float m_xdelta{0.};
    float m_ydelta{0.};

    char m_name[32]{0};
    char m_units[32]{0};

    size_t m_row = 0;
    size_t m_col = 0;

    std::vector<su::FMatrix> m_frames;
    std::vector<std::string> m_buff;

  private:
    char m_prefix[4]{};
};

#endif  // SILLY_UTILS_SILLY_TZX_GRID_H
