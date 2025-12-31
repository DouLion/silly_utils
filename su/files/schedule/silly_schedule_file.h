
/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-18
 * @file: silly_schedule_file.h
 * @description: 定时生成的文件
 * @version: v1.0.1 2024-10-18 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SCHEDULE_FILE_H
#define SILLY_UTILS_SILLY_SCHEDULE_FILE_H
#include <files/silly_memory_map.h>
#include <files/schedule/silly_schedule_index.h>
#include <files/schedule/silly_schedule_data.h>
#include <datetime/silly_posix_time.h>
#include <log/silly_log.h>

class suScheduleFile
{
    static constexpr int CODE_MAX_LEN = 64;   // 编码的最大长度
    static constexpr int RESERVE_LEN = 4096;  // 预留块, 数据始终在此之后开始计算
  public:
    /**
     * 指定一个名称用于构建目录
     * @param desc 名称
     */
    bool SetDesc(const suScheduleData::fileDesc& desc);
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
    bool Read(const std::string& code, const sutime& tm, std::vector<char>& data, const size_t size);

    /**
     * 指定时间段的所有数据
     * @tparam T
     * @param tm
     * @param code2data
     * @return
     */
    template <typename T>
    bool Read(const sutime& tm, std::map<std::string, T>& code2data);
    bool Read(const sutime& tm, std::map<std::string, std::vector<char>>& code2data, const size_t size);

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
    bool Read(const std::string& code, const sutime& btm, const sutime& etm, std::map<sutime, std::vector<char>>& time2data, const size_t size);

    template <typename T>
    void ReadSingleYear(const sutime& btm, const sutime& etm, std::map<sutime, T>& time2data, const size_t rawOff);
    void ReadSingleYear(const sutime& btm, const sutime& etm, std::map<sutime, std::vector<char>>& time2data, const size_t rawOff, const size_t size);

    /**
     *  每个轮次的数据计算完成后, 写入数据
     * @param tm
     * @param code2data
     * @return
     */
    template <typename T>
    bool Write(sutime& tm, const std::map<std::string, T>& code2data);
    bool Write(sutime& tm, const std::map<std::string, std::vector<char>>& code2data, const size_t size);

  private:
    suPath IndexFile() const;
    suPath DataFile(const sutime& tm) const;
    size_t RawOffset(const std::string& code, const size_t& blockSize) const;
    /*相对于RawOffset的偏移位置从code64位开始, 到实际数据的偏移位置, 这个值至少是64*/
    size_t TimeOff(const sutime& tm, const size_t& blockSize);
    size_t BlockOff(const std::string& code, const sutime& tm, const size_t& blockSize) const;
    size_t AssumeFileSize(const size_t& codeNum, const size_t& blockSize) const;
    static bool CheckCode(const char* readCode, const std::string& givenCode);
    template <typename T, typename Func, typename... Args>
    bool ReadUnionFile(Func&& func, Args&&... args);
    template <typename Func, typename... Args>
    bool ReadUnionFile(Func&& func, Args&&... args);
    std::shared_ptr<suMemMapFile> OpenMMap(const int& year, const size_t& size, eMMFMode mode);

  protected:
    std::map<int, std::shared_ptr<suMemMapFile>> m_year2mmap;
    suScheduleIndex m_index;
    char m_header[4] = {'T', 'Z', 'X', 'S'};       // 固定头
    char m_version[4] = {0x00, 0x01, 0x00, 0x01};  // 版本号
    suScheduleData::fileDesc m_desc;
    bool m_isDescSet = false;
    size_t m_rawNum = 0;
    std::mutex m_WriteMutex;
};
template <typename T, typename Func, typename... Args>
bool suScheduleFile::ReadUnionFile(Func&& func, Args&&... args)
{
    if (m_desc.name != CLASS_PURE_NAME<T>())
    {
        SLOG_WARN("描述名称[{}]应该与类名[{}]一致", m_desc.name, CLASS_PURE_NAME<T>())
        return false;
    }
    std::scoped_lock lock(m_WriteMutex);
    return func(std::forward<Args>(args)...);
}
template <typename Func, typename... Args>
bool suScheduleFile::ReadUnionFile(Func&& func, Args&&... args)
{
    // if (m_desc.name != CLASS_PURE_NAME<T>())
    //{
    // SLOG_WARN("描述名称[{}]应该与类名[{}]一致", m_desc.name, CLASS_PURE_NAME<T>())
    //  return false;
    //}
    std::scoped_lock lock(m_WriteMutex);
    return func(std::forward<Args>(args)...);
}
template <typename T>
bool suScheduleFile::Read(const std::string& code, const sutime& tm, T& data)
{
    return ReadUnionFile<T>([this, code, tm, &data]() -> bool {
        const size_t rawOff = RawOffset(code, sizeof(T));
        if (0 == rawOff)
        {
            return false;
        }
        const int year = tm.year();
        const size_t timeOff = TimeOff(tm, sizeof(T));
        // SLOG_DEBUG("读 rawOff: {}, timeOff: {}", rawOff, timeOff);
        auto tmp = OpenMMap(year, AssumeFileSize(m_index.size(), sizeof(T)), eMMFMode::Read);
        if (!tmp)
        {
            return false;
        }
        return tmp->read((unsigned char*)(&data), sizeof(T), rawOff + timeOff);
    });
}
template <typename T>
bool suScheduleFile::Read(const sutime& tm, std::map<std::string, T>& code2data)
{
    return ReadUnionFile<T>([this, tm, &code2data]() -> bool {
        const int year = tm.year();
        auto tmp = OpenMMap(year, AssumeFileSize(m_index.size(), sizeof(T)), eMMFMode::Read);
        if (!tmp)
        {
            return false;
        }
        const size_t timeOff = TimeOff(tm, sizeof(T));
        for (const auto& [code, sort] : m_index.m_code2sort)
        {
            const size_t rawOff = RawOffset(code, sizeof(T));
            if (0 == rawOff)
            {
                continue;
            }
            T data;
            if (tmp->read((unsigned char*)(&data), sizeof(T), rawOff + timeOff))
            {
                code2data[code] = data;
            }
        }

        return !code2data.empty();
    });
}
template <typename T>
bool suScheduleFile::Read(const std::string& code, const sutime& btm, const sutime& etm, std::map<sutime, T>& time2data)
{
    return ReadUnionFile<T>([this, code, btm, etm, &time2data]() -> bool {
        if (btm > etm)
        {
            return false;
        }
        const size_t rawOff = RawOffset(code, sizeof(T));
        if (0 == rawOff)
        {
            return false;
        }
        if (btm.year() == etm.year())
        {
            ReadSingleYear(btm, etm, time2data, rawOff);
        }
        else if (etm.year() - btm.year() == 1)
        {
            sutime btm_0101, etm_1231;
            etm_1231.from_string(btm.to_string(DTFMT_Y) + "-12-31 23:55");
            btm_0101.from_string(etm.to_string(DTFMT_Y) + "-01-01 00:00");

            ReadSingleYear(btm, etm_1231, time2data, rawOff);
            ReadSingleYear(btm_0101, etm, time2data, rawOff);
        }
        else
        {
            SLOG_ERROR("时间跨度不能超过一年")
            return false;
        }
        return !time2data.empty();
    });
}
template <typename T>
void suScheduleFile::ReadSingleYear(const sutime& btm, const sutime& etm, std::map<sutime, T>& time2data, const size_t rawOff)
{
    auto tmp = OpenMMap(btm.year(), AssumeFileSize(m_index.size(), sizeof(T)), eMMFMode::Read);
    int num = (etm.stamp_sec() - btm.stamp_sec()) / m_desc.each + 1;

    const size_t timeOff = TimeOff(btm, sizeof(T));
    std::vector<T> records(num);
    if (tmp->read((unsigned char*)records.data(), sizeof(T) * num, rawOff + timeOff))
    {
        sutime tm = btm;
        for (int i = 0; i < num; ++i)
        {
            time2data[tm] = records[i];
            tm += m_desc.each;
        }
    }
}

template <typename T>
bool suScheduleFile::Write(sutime& tm, const std::map<std::string, T>& code2data)
{
    if (m_desc.name != CLASS_PURE_NAME<T>())
    {
        SLOG_WARN("描述名称[{}]应该与类名[]一致", m_desc.name, CLASS_PURE_NAME<T>())
        return false;
    }
    std::scoped_lock lock(m_WriteMutex);
    std::set<std::string> newCodes;
    for (const auto& [code, _] : code2data)
    {
        if (code.size() > CODE_MAX_LEN)
        {
            continue;
        }
        if (!m_index.contains(code))
        {
            m_index.add(code);
        }
    }
    if (!m_index.write(IndexFile()))
    {
        SLOG_ERROR("写入索引文件失败")
        return false;
    }
    const int year = tm.year();

    auto mmap = OpenMMap(year, AssumeFileSize(m_index.size(), sizeof(T)), eMMFMode::Write);
    if (!mmap)
    {
        return false;
    }
    size_t timeOff = TimeOff(tm, sizeof(T));
    for (const auto& [code, data] : code2data)
    {
        size_t rawOff = RawOffset(code, sizeof(T));
        SLOG_DEBUG("写 rawOff: {}, timeOff: {}", rawOff, timeOff);
        char name[CODE_MAX_LEN] = {};
        /*mmap->read((unsigned char*)name, CODE_MAX_LEN, rawOff);
        if (!CheckCode(name, code))
        {
            continue;
        }*/
        mmap->write((unsigned char*)(code.data()), code.size(), rawOff);
        mmap->write((unsigned char*)(&data), sizeof(T), rawOff + timeOff);
    }
    return true;
}

#endif  // SILLY_UTILS_SILLY_SCHEDULE_FILE_H
