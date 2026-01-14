/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-03-13
 * @file: silly_dynamic_rule_block.h
 * @description: 动态指标年度分块文件读写, 考虑使用mmap
 *  动态指标每个小时记录一次,每个站按照366天*24小时分配空间
 * @version: v1.0.1 2025-03-13 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_DYNAMIC_RULE_BLOCK_H
#define SILLY_UTILS_SILLY_DYNAMIC_RULE_BLOCK_H
#include <datetime/silly_posix_time.h>
#include <files/silly_memory_map.h>
class suDynamicRule
{
  public:
#pragma pack(1)
    struct Cell
    {
        float h1P = 0.0;  // 1 小时准备转移
        float h1I = 0.0;  // 1 小时立即转移
        float h3P = 0.0;
        float h3I = 0.0;
        float h6P = 0.0;
        float h6I = 0.0;
        float soil = 0.0;  // 土壤含水量 0.0 -100.0
    };
#pragma pack()
  public:
    suDynamicRule() = default;
    ~suDynamicRule();
    /// <summary>
    ///
    /// </summary>
    /// <param name="root">文件根目录</param>
    /// <param name="num">最大危险区数量,</param>
    /// <returns></returns>
    bool init(const suPath& root, const size_t& num = 30000, const bool& read_mode = true);

    /// <summary>
    /// 读取指定时间指定危险区的数据
    /// </summary>
    /// <param name="code"></param>
    /// <param name="time"></param>
    ///
    /// <param name="data"></param>
    /// <returns></returns>
    bool read(const std::string& code, const sutime& time, Cell& data);

    /// <summary>
    /// 读取指定时间一批危险区的数据
    /// </summary>
    /// <param name="time"></param>
    /// <param name="code_data"></param>
    /// <returns></returns>
    bool read(const sutime& time, std::map<std::string, Cell>& code_data);

    /// <summary>
    /// 读取指定危险区在一段时间内的数据, [btm, etm], 最多能跨一年
    /// </summary>
    /// <param name="code"></param>
    /// <param name="btm"></param>
    /// <param name="etm"></param>
    /// <param name="time2data"></param>
    /// <returns></returns>
    bool read(const std::string& code, const sutime& btm, const sutime& etm, std::map<std::time_t, Cell>& time2data);

    /// <summary>
    /// 写入指定时间指定危险区的数据
    /// </summary>
    /// <param name="code"></param>
    /// <param name="time"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    bool write(const std::string& code, const sutime& time, const Cell& data);

    /// <summary>
    /// 写入指定时间点一批危险区的数据
    /// 一般都是这个操作
    /// </summary>
    /// <param name="time"></param>
    /// <param name="code2data"></param>
    /// <returns></returns>
    bool write(const sutime& time, const std::map<std::string, Cell>& code2data);

    /// <summary>
    /// 写入指定时间一批危险区的数据
    /// </summary>
    /// <param name="code"></param>
    /// <param name="time2data"></param>
    /// <returns></returns>
    bool write(const std::string& code, const std::map<std::time_t, Cell>& time2data);

    void close();

  private:
    bool open_dat(const sutime& time);
    bool open_dat(const std::string& year_str);

    void read(const size_t& offset, const size_t& bi, const size_t& ei, const sutime& time, std::map<std::time_t, Cell>& time2data);

  private:
    bool m_read_mode = true;
    bool m_init = false;
    suPath m_root;
    size_t m_num = 0;
    size_t m_max_index = 0;  // 当前最大危险区索引, 不能超过m_num
    std::map<std::string, size_t> m_code_index;
    std::map<std::string, std::shared_ptr<suMemMapFile>> m_year_mmap;
};


#endif  // SILLY_UTILS_SILLY_DYNAMIC_RULE_BLOCK_H
