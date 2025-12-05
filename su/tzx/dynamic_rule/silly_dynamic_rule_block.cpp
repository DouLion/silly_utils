/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-03-13
 * @file: silly_dynamic_rule_block.cpp
 * @description: silly_dynamic_rule_block实现
 * @version: v1.0.1 2025-03-13 dou li yang
 */
#include "silly_dynamic_rule_block.h"
#include <log/silly_log.h>
#include <files/silly_file.h>
#define INDEX_FILENAME ".drule.index"
#define DAT_SUFFIX ".dat"
// 一个站,一年按366天计算, 一天24条记录,占用多少字节
#define CODE_SIZE_PER_YEAR (366 * 24 * sizeof(Cell))

static size_t index_in_year(const silly_posix_time& time)
{
    silly_posix_time pt_0101 = time.time_from_string(time.to_string("%Y-01-01 00:00"));
    return (time.stamp_sec() - pt_0101.stamp_sec()) / SEC_IN_HOUR;
}

suDynamicRule::~suDynamicRule()
{
    close();
}

bool suDynamicRule::init(const suPath& root, const size_t& num, const bool& read_mode)
{
    suPath p(root);
    suPath::mkdir(p);
    std::string index_file = p.append(INDEX_FILENAME).string();
    if (!suPath::exists(index_file))
    {
        SLOG_ERROR("索引文件不存在:{}", index_file)
        return false;
    }
    m_root = root;
    m_read_mode = read_mode;
    std::vector<std::string> lines;
    sufile::readlines(index_file, lines);
    m_num = num;
    for (auto& line : lines)
    {
        std::vector<std::string> items = SPLIT(line, ',');
        if (items.size() != 2)
        {
            continue;
        }
        std::string code = items[0];
        size_t index = static_cast<size_t>(std::stod(items[1]));
        m_max_index = std::max(m_max_index, index);
        m_code_index.insert({code, index});
    }
    m_init = !m_code_index.empty();
    return m_init;
}

bool suDynamicRule::read(const std::string& code, const silly_posix_time& time, Cell& data)
{
    std::string year = time.to_string(DTFMT_Y);
    if (!open_dat(year))
    {
        return false;
    }
    if (m_code_index.find(code) != m_code_index.end())
    {
        size_t index = m_code_index[code];
        size_t offset = index * CODE_SIZE_PER_YEAR;

        size_t pos = offset + index_in_year(time) * sizeof(Cell);
        m_year_mmap[year]->read((suMemMapFile::Ptr)&data, sizeof(Cell), pos);
        return true;
    }
    return false;
}

bool suDynamicRule::read(const silly_posix_time& time, std::map<std::string, Cell>& code2data)
{
    std::string year = time.to_string(DTFMT_Y);
    
    if (!open_dat(year))
    {
        return false;
    }
    for (auto& [code, idx] : m_code_index)
    {
        size_t offset = idx * CODE_SIZE_PER_YEAR;
        Cell tmp;
        size_t pos = offset + index_in_year(time) * sizeof(Cell);
        m_year_mmap[year]->read((suMemMapFile::Ptr)&tmp, sizeof(Cell), pos);

        code2data[code] = tmp;
    }
    return !code2data.empty();
}

bool suDynamicRule::read(const std::string& code, const silly_posix_time& btm, const silly_posix_time& etm, std::map<std::string, Cell>& time2data)
{
    if (m_code_index.find(code) == m_code_index.end())
    {
        return false;
    }
    size_t offset = m_code_index[code];
    if (btm.year() == etm.year())
    {
        std::string year = btm.to_string(DTFMT_Y);
        if (!open_dat(year))
        {
            return false;
        }

        size_t bi = index_in_year(btm);
        size_t ei = index_in_year(etm);
        read(offset, bi, ei, btm, time2data);
        return true;
    }
    else if (etm.year() - btm.year() == 1)  // 结束时间一定比开始时间大
    {
        if (open_dat(btm))
        {
            std::string year = btm.to_string(DTFMT_Y);
            silly_posix_time etm_1231;
            etm_1231.from_string(year + "-12-31 23:00");
            size_t bi = index_in_year(btm);
            size_t ei = index_in_year(etm_1231);
            read(offset, bi, ei, btm, time2data);
        }
        if (open_dat(etm))
        {
            std::string year = etm.to_string(DTFMT_Y);
            silly_posix_time btm_0101;
            btm_0101.from_string(year + "-01-01 00:00");
            size_t bi = index_in_year(btm_0101);
            size_t ei = index_in_year(etm);
            read(offset, bi, ei, btm_0101, time2data);
        }
        return !time2data.empty();
    }
    SLOG_ERROR("时间跨度不能超过一年")
    return false;
}

bool suDynamicRule::write(const std::string& code, const silly_posix_time& time, const Cell& data)
{
    std::string year = time.to_string(DTFMT_Y);
    if (!open_dat(year))
    {
        return false;
    }
    auto iter = m_code_index.find(code);
    if (iter != m_code_index.end())
    {
        size_t offset = iter->second * CODE_SIZE_PER_YEAR;

        size_t pos = offset + index_in_year(time) * sizeof(Cell);
        m_year_mmap[year]->write((suMemMapFile::Ptr)&data, sizeof(Cell), pos);
        return true;
    }
    return false;
}

bool suDynamicRule::write(const silly_posix_time& time, const std::map<std::string, Cell>& code2data)
{
    std::string year = time.to_string(DTFMT_Y);
    if (!open_dat(year))
    {
        return false;
    }
    for (const auto& [code, data] : code2data)
    {
        auto iter = m_code_index.find(code);
        if (iter != m_code_index.end())
        {
            size_t offset = iter->second * CODE_SIZE_PER_YEAR;

            size_t pos = offset + index_in_year(time) * sizeof(Cell);
            m_year_mmap[year]->write((suMemMapFile::Ptr)&data, sizeof(Cell), pos);
        }
    }

    return true;
}

bool suDynamicRule::open_dat(const silly_posix_time& time)
{
    std::string year = time.to_string(DTFMT_Y);
    return open_dat(year);
}

bool suDynamicRule::open_dat(const std::string& year_str)
{
    if (!m_init)
    {
        return false;
    }
    if (m_year_mmap.find(year_str) == m_year_mmap.end())
    {
        std::string file = suPath(m_root).append(year_str + DAT_SUFFIX).string();
        bool reset_file = false;
        size_t total_size = m_num * CODE_SIZE_PER_YEAR;
        if (!suPath::exists(file))
        {
            reset_file = true;
        }
        else
        {
            if (total_size > std::filesystem::file_size(file))
            {
                reset_file = true;
            }
        }
        if (reset_file)
        {
            std::ofstream ofs(file, std::ios::binary | std::ios::out);
            ofs.seekp(total_size, std::ios::beg);
            ofs.write("EOF", 3);
            ofs.close();
        }
        std::shared_ptr<suMemMapFile> tmp = std::make_shared<suMemMapFile>();
        if (m_read_mode)
        {
            if (!tmp->open(file, eMMFMode::Read))
            {
                tmp->close();
                return false;
            }
        }
        else
        {
            suMemMapFile::Param p;
            p.mode = eMMFMode::Write;
            p.path = file;
            p.disposition = OpenExisting;
            p.map_size = total_size;
            if (!tmp->open(p))
            {
                tmp->close();
                return false;
            }
        }       
        std::cout << "打开: " << file << std::endl;
        m_year_mmap[year_str] = tmp;
    }
    return true;
}

void suDynamicRule::read(const size_t& offset, const size_t& bi, const size_t& ei, const silly_posix_time& time, std::map<std::string, Cell>& time2data)
{
    std::string year = time.to_string(DTFMT_Y);
    for (size_t i = bi; i <= ei; i++)
    {
        Cell tmp;
        size_t pos = offset * CODE_SIZE_PER_YEAR + i * sizeof(Cell);
        m_year_mmap[year]->read((suMemMapFile::Ptr)&tmp, sizeof(Cell), pos);
        std::string tmstr = (time + silly_time_duration(i - bi, 0, 0)).to_string(DTFMT_YMDHM);
        time2data[tmstr] = tmp;
    }
}

bool suDynamicRule::write(const std::string& code, const std::map<std::string, Cell>& time2data)
{
    return false;
}

void suDynamicRule::close()
{
    for (auto& [year, mmap] : m_year_mmap)
    {
        mmap->close();
    }
    m_year_mmap.clear();
    m_init = false;
}