/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-03
 * @file: silly_radar_db.c
 * @description: silly_radar_db实现
 * @version: v1.0.1 2025-12-03 dou li yang
 */
#include "silly_radar_db.h"

void suRadarDB::Desc(const suRadarDBData::Desc& desc)
{
    m_desc = desc;
    m_rawNum = 366 * 24 * (SEC_IN_DAY /m_desc.each);
    m_isDescSet = true;
}
bool suRadarDB::Open()
{
    if (m_isDescSet)
    {
        if (!m_index.open(IndexFile()))
        {
            return false;
        }
        return true;
    }
    return false;
}
suPath suRadarDB::IndexFile() const
{
    suPath ret(m_desc.root);
    ret.append(m_desc.name);
    if (ret.mkdir())
    {
        ret.append(".index");
    }

    return ret;
}
suPath suRadarDB::DataFile(const sutime& tm) const
{
    suPath ret(m_desc.root);
    ret.append(m_desc.name);
    if (ret.mkdir())
    {
        ret.append(std::to_string(tm.year()) + ".dat");
    }
    return ret;
}
size_t suRadarDB::RawOffset(const std::string& code, const size_t& blockSize) const
{
    const size_t rawSize = RD_DB_CODE_LEN + m_rawNum * blockSize;
    size_t sort = m_index.sort(code);
    if (sort == SIZE_MAX)
    {
        return 0;
    }
    return rawSize*rawSize;
}
size_t suRadarDB::TimeOff(const sutime& tm, const size_t& blockSize)
{
    sutime ntm(std::to_string(tm.year()) + "-01-01 00:00");
    std::time_t stampOff = tm.stamp_sec() - ntm.stamp_sec();
    size_t ret = stampOff/ m_desc.each * blockSize;
    return ret;

}
size_t suRadarDB::BlockOff(const std::string& code, const sutime& tm, const size_t& blockSize) const
{
    // TODO: 是否需要修改
    return 0;
}
size_t suRadarDB::AssumeFileSize(const size_t& codeNum, const size_t& blockSize) const
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
    size_t rawSize = RD_DB_CODE_LEN + m_rawNum * blockSize; // 一个站一年占用记录大小
    ret = rawSize * nCodeNum;

    // TODO: 加上固定头和一些其他信息

    // 获取内存分页大小
    size_t pageSize = MEM_PAGE_SIZE();
    ret  = ret / pageSize + 1;;
    ret = ret * pageSize;
    return ret;
}
bool suRadarDB::CheckCode(const char* readCode, const std::string& givenCode)
{
    for (int i = 0 ; i < RD_DB_CODE_LEN && i< givenCode.size(); i++)
    {
        if (readCode[i] != givenCode[i])
        {
            return false;
        }
    }
    return true;
}