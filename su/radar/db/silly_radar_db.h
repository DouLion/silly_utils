/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-03
 * @file: silly_radar_db.cpp
 * @description: 固定时段的数据存储, 主要用于雷达数据
 *              1. 4字节固定头  4字节版本号
 *              2. 数据描述信息
 *              3. 64位 code1 用于验证 从当年 1月1日开始 逐5分钟的数据块存储
 *              4. 64位 code2, 从当年 1月1日开始 逐5分钟的数据块存储
 *              ...
 * @version: v1.0.1 2025-12-03 dou li yang
 */
#ifndef SILLY_RADAR_DB_H
#define SILLY_RADAR_DB_H

#include <log/silly_log.h>
#include <files/silly_memory_map.h>
#include <datetime/silly_posix_time.h>
#include <radar/db/silly_radar_db_index.h>
#include <radar/db/silly_radar_db_data.h>

class suRadarDB
{
public:
    /**
     * 指定一个名称用于构建目录
     * @param desc 名称
     */
    void Desc(const suRadarDBData::Desc& desc);

    bool Open();

    /**
     * 指定编码,指定时间的数据
     * @tparam T
     * @param code
     * @param tm
     * @param data
     * @return
     */
    template <typename T>
    bool Read(const std::string& code, const sutime& tm, T& data);

    /**
     * 指定时间段的所有数据
     * @tparam T
     * @param tm
     * @param code2data
     * @return
     */
    template <typename T>
    bool Read(const sutime& tm, std::map<std::string, T>& code2data);

    /**
     * 指定编码 某一个时间段的数据
     * @tparam T
     * @param code
     * @param btm
     * @param etm
     * @param time2data
     * @return
     */
    template <typename T>
    bool Read(const std::string& code, const sutime& btm, const sutime& etm, std::map<sutime, T>& time2data);

    /**
     *  每个轮次的数据计算完成后, 写入数据
     * @param tm
     * @param code2data
     * @return
     */
    template <typename T>
    bool Write(sutime& tm, const std::map<std::string, T>& code2data);
private:
    suPath IndexFile() const;
    suPath DataFile(const sutime& tm) const;
    size_t RawOffset(const std::string& code, const size_t& blockSize) const;
    /*从code64位开始, 到实际数据的偏移位置, 这个值至少是64*/
    size_t TimeOff(const sutime& tm, const size_t& blockSize);
    size_t BlockOff(const std::string& code, const sutime& tm, const size_t& blockSize) const;
    size_t AssumeFileSize(const size_t& codeNum, const size_t& blockSize) const;

    static bool CheckCode(const char* readCode, const std::string& givenCode);
protected:
    std::map<int,std::shared_ptr<suMemMapFile>> m_year2mmap;
    suRadarDBIndex m_index;
    char m_header[4] = {'T','Z','X','R'}; // 固定头
    char m_version[4] = {0x00, 0x01,0x00, 0x01}; // 版本号
    suRadarDBData::Desc m_desc;
    bool m_isDescSet = false;
    size_t m_rawNum = 0;
    std::mutex m_WriteMutex;
};

template <typename T>
bool suRadarDB::Read(const std::string& code, const sutime& tm, T& data)
{
    if (m_desc.name != CLASS_PURE_NAME<T>())
    {
        SLOG_WARN("描述名称[{}]应该与类名[]一致", m_desc.name, CLASS_PURE_NAME<T>())
        return false;
    }
    while (!m_WriteMutex.try_lock())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

}
template <typename T>
bool suRadarDB::Read(const sutime& tm, std::map<std::string, T>& code2data)
{
    if (m_desc.name != CLASS_PURE_NAME<T>())
    {
        SLOG_WARN("描述名称[{}]应该与类名[]一致", m_desc.name, CLASS_PURE_NAME<T>())
        return false;
    }
}
template <typename T>
bool suRadarDB::Read(const std::string& code, const sutime& btm, const sutime& etm, std::map<sutime, T>& time2data)
{
    if (m_desc.name != CLASS_PURE_NAME<T>())
    {
        SLOG_WARN("描述名称[{}]应该与类名[]一致", m_desc.name, CLASS_PURE_NAME<T>())
        return false;
    }
}
template <typename T>
bool suRadarDB::Write(sutime& tm, const std::map<std::string, T>& code2data)
{
    if (m_desc.name != CLASS_PURE_NAME<T>())
    {
        SLOG_WARN("描述名称[{}]应该与类名[]一致", m_desc.name, CLASS_PURE_NAME<T>())
        return false;
    }
    std::set<std::string> newCodes;
    for (const auto& [code,_]: code2data)
    {
        if (code.size() > RD_DB_CODE_LEN)
        {
            continue;
        }
        if (!m_index.contains(code))
        {
            m_index.add(code);
        }
    }
    if (m_index.write(IndexFile()))
    {
        SLOG_ERROR("写入索引文件失败")
        return false;
    }
    int year = tm.year();
    std::scoped_lock lock(m_WriteMutex);

    auto tmp = m_year2mmap[year];
    if (!tmp)
    {
        return false;
    }
    size_t tmOff = TimeOff(tm, sizeof(T));
    for (const auto& [code,data]: code2data)
    {
        size_t rawOff = RawOffset(code, sizeof(T));
        char name[RD_DB_CODE_LEN] = {};
        tmp->read((unsigned char*)name, RD_DB_CODE_LEN, rawOff);
        if (!CheckCode(name, code))
        {
            continue;
        }
        tmp->write(&data, sizeof(T), rawOff + tmOff);
    }
    return true;
}
#endif  // SILLY_RADAR_DB_H
