/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-18
 * @file: silly_schedule_file.cpp
 * @description: silly_schedule_file实现
 * @version: v1.0.1 2024-10-18 dou li yang
 */
#include "silly_schedule_file.h"

bool suScheduleFile::SetDesc(const suScheduleData::Desc& desc)
{
    m_desc = desc;
    m_rawNum = 366 * 24 * (SEC_IN_HOUR / m_desc.each);

    if (!m_index.open(IndexFile()))
    {
        SLOG_ERROR("打开索引文件失败")
        return false;
    }
    m_isDescSet = true;
    if (m_index.size() == 0)
    {
        SLOG_WARN("索引为空")
    }
    return true;
}

suPath suScheduleFile::IndexFile() const
{
    suPath ret(m_desc.root);
    ret.append(m_desc.name);
    if (ret.mkdir())
    {
        ret.append(".index");
    }

    return ret;
}
suPath suScheduleFile::DataFile(const sutime& tm) const
{
    suPath ret(m_desc.root);
    ret.append(m_desc.name);
    if (ret.mkdir())
    {
        ret.append(std::to_string(tm.year()) + ".dat");
    }
    return ret;
}
size_t suScheduleFile::RawOffset(const std::string& code, const size_t& blockSize) const
{
    const size_t rawSize = CODE_MAX_LEN + m_rawNum * blockSize;
    size_t sort = m_index.sort(code);
    if (sort == SIZE_MAX)
    {
        return 0;
    }
    return sort * rawSize + RESERVE_LEN;
}
size_t suScheduleFile::TimeOff(const sutime& tm, const size_t& blockSize)
{
    sutime ntm(std::to_string(tm.year()) + "-01-01 00:00");
    std::time_t stampOff = tm.stamp_sec() - ntm.stamp_sec();
    size_t ret = stampOff / m_desc.each * blockSize;
    return ret + CODE_MAX_LEN;
}
size_t suScheduleFile::BlockOff(const std::string& code, const sutime& tm, const size_t& blockSize) const
{
    // TODO: 是否需要修改
    return 0;
}
size_t suScheduleFile::AssumeFileSize(const size_t& codeNum, const size_t& blockSize) const
{
    size_t ret = 0;
    if (codeNum > 0xFFFFFF)
    {
        SLOG_ERROR("记录数量{}, 超过一般经验值({}), 另行实现", codeNum, 0xFFFFFF)
        return 0;
    }

    size_t nCodeNum = codeNum;
    if (nCodeNum < 1000)
    {
        nCodeNum += 100;
    }
    else
    {
        nCodeNum = static_cast<size_t>(static_cast<double>(nCodeNum) * 1.1);
    }
    size_t rawSize = CODE_MAX_LEN + m_rawNum * blockSize;  // 一个站一年占用记录大小
    ret = rawSize * nCodeNum;

    // TODO: 加上固定头和一些其他信息

    // 获取内存分页大小
    size_t pageSize = MEM_PAGE_SIZE();
    ret = ret / pageSize + 1;
    ;
    ret = ret * pageSize;
    return ret;
}
bool suScheduleFile::CheckCode(const char* readCode, const std::string& givenCode)
{
    for (int i = 0; i < CODE_MAX_LEN && i < givenCode.size(); i++)
    {
        if (readCode[i] != givenCode[i])
        {
            return false;
        }
    }
    return true;
}
std::shared_ptr<suMemMapFile> suScheduleFile::OpenMMap(const int& year, const size_t& size, eMMFMode mode)
{
    if (MAP_HAS(m_year2mmap, year))
    {
        // SLOG_DEBUG("已存在[{}.dat]文件映射", year)
        return m_year2mmap.at(year);
    }
    suPath datafile(m_desc.root);
    datafile.append(m_desc.name);
    if (datafile.mkdir())
    {
        datafile.append(std::to_string(year) + ".dat");
    }

    if (!datafile.exists())
    {
        if (mode == eMMFMode::Write)
        {
            std::string buff(1024, '\0');
            char* p = buff.data();
            memcpy(p, m_header, sizeof(m_header));
            p += sizeof(m_header);
            memcpy(p, m_version, sizeof(m_version));
            p += sizeof(m_version);

            std::string llsize = std::to_string(sizeof(long long) * 8);
            memcpy(p, llsize.data(), llsize.size());
            p += llsize.size();
            if (SU_LITTLE_ENDIAN)
            {
                memcpy(p, "LEND", 4);
            }
            else
            {
                memcpy(p, "BEND", 4);
            }
            sufile::write(datafile, buff);
        }
        else
        {
            SLOG_ERROR("文件不存在:{}", datafile.u8string())
            return nullptr;
        }
    }
    if (!datafile.resize_file(size))
    {
        SLOG_ERROR("预分配文件失败:{}", datafile.u8string())
        return nullptr;
    }
    std::shared_ptr<suMemMapFile> ret = std::make_shared<suMemMapFile>();
    suMemMapFile::Param p;
    p.file_size = size;
    p.map_size = size;
    p.mode = mode;
    p.path = datafile;
    if (ret->open(p))
    {
        m_year2mmap[year] = ret;
        return ret;
    }
    SLOG_ERROR("文件映射失败:{}", datafile.u8string())
    return nullptr;
}

bool suScheduleFile::Read(const std::string& code, const sutime& tm, std::vector<unsigned char>& data, const size_t size)
{
    return ReadUnionFile([this, code, tm, &data, size]() -> bool {
        const size_t rawOff = RawOffset(code, size);
        if (0 == rawOff)
        {
            return false;
        }
        const int year = tm.year();
        const size_t timeOff = TimeOff(tm, size);
        SLOG_DEBUG("读 rawOff: {}, timeOff: {}", rawOff, timeOff);
        auto tmp = OpenMMap(year, AssumeFileSize(m_index.size(), size), eMMFMode::Read);
        if (!tmp)
        {
            return false;
        }
        return tmp->read(data.data(), size, rawOff + timeOff);
    });
}

bool suScheduleFile::Write(sutime& tm, const std::map<std::string, std::vector<unsigned char>>& code2data, const size_t size)
{
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

    auto mmap = OpenMMap(year, AssumeFileSize(m_index.size(), size), eMMFMode::Write);
    if (!mmap)
    {
        return false;
    }
    size_t timeOff = TimeOff(tm, size);
    for (const auto& [code, data] : code2data)
    {
        size_t rawOff = RawOffset(code, size);
        SLOG_DEBUG("写 rawOff: {}, timeOff: {}", rawOff, timeOff);
        char name[CODE_MAX_LEN] = {};
        /*mmap->read((unsigned char*)name, CODE_MAX_LEN, rawOff);
        if (!CheckCode(name, code))
        {
            continue;
        }*/
        mmap->write((unsigned char*)(code.data()), code.size(), rawOff);
        mmap->write((unsigned char*)(&data), size, rawOff + timeOff);
    }
    return true;
}
