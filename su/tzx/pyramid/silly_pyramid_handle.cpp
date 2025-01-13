/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-10
 * @file: silly_pyramid_handle.cpp
 * @description: silly_pyramid_handle实现
 * @version: v1.0.1 2025-01-10 dou li yang
 */
#include "silly_pyramid_handle.h"
using namespace silly::pyramid;
handle::handle()
{
    m_info = new info();
    m_index = new index();
    m_data = new data();
}
handle::~handle()
{
    SU_MEM_DEL(m_info)
    SU_MEM_DEL(m_info)
    SU_MEM_DEL(m_info)
}
bool handle::begin_read(const char* root)
{
    return begin_read(std::filesystem::path(root));
}
bool handle::begin_write(const char* root)
{
    return begin_write(std::filesystem::path(root));
}

bool handle::begin_read(const std::string& root)
{
    return begin_read(std::filesystem::path(root));
}
bool handle::begin_write(const std::string& root)
{
    return begin_write(std::filesystem::path(root));
}

bool handle::begin_read(const std::filesystem::path& root)
{
    if (!std::filesystem::exists(root))
    {
        return false;
    }
    m_root = root.string();
    m_use_mmap = true;
    m_mode = silly::file::memory_map::access_mode::Read;
    return open();
}
bool handle::begin_write(const std::filesystem::path& root)
{
    if (!std::filesystem::exists(root))
    {
        return false;
    }
    m_root = root.string();
    m_use_mmap = false;
    m_mode = silly::file::memory_map::access_mode::Write;
    return open();
}

bool handle::read(block& blk)
{
    if (m_opened)
    {
        return m_data->read(blk);
    }

    return false;
}
std::string handle::read(const block& blk)
{
    if (m_opened)
    {
        return m_data->read(blk);
    }

    return "";
}

bool handle::index_layer()
{
    return m_index->build();
}
bool handle::write(block& blk)
{
    if (m_opened)
    {
        return m_data->write(blk);
    }

    return false;
}

bool handle::open()
{
    std::scoped_lock lock(m_open_mutex);
    if (!m_opened)
    {
        std::string pinfo = std::filesystem::path(m_root).append(INFO_NAME).string();
        std::string pindex = std::filesystem::path(m_root).append(INDEX_NAME).string();
        std::string pdata = std::filesystem::path(m_root).append(DATA_NAME).string();
        if (!m_info->open(pinfo, m_mode, m_use_mmap))
        {
            m_info->close();
            return m_opened;
        }

        if (!m_index->open(pindex, m_mode, m_use_mmap))
        {
            m_index->close();
            m_info->close();
            return m_opened;
        }

        if (!m_data->open(pdata, m_mode, m_use_mmap))
        {
            m_info->close();
            m_index->close();
            m_data->close();
            return m_opened;
        }
        m_data->set(m_index);
        m_opened = true;
    }

    return m_opened;
}

uint8_t handle::beg_layer() const
{
    if (m_index)
    {
        return m_index->beg_layer();
    }
    return 0;
}
uint8_t handle::end_layer() const
{
    if (m_index)
    {
        return m_index->end_layer();
    }
    return 0;
}
silly_rect handle::bound() const
{
    if (m_info)
    {
        return m_info->bound();
    }
    return silly_rect();
}
void handle::beg_layer(const uint8_t& beg) noexcept
{
    if (!m_index)
        return;
    if (beg > len::MAX_ZOOM)
    {
        SLOG_WARN("超过最大支持层级:{}, 自动改为支持的最大层级")
        m_index->beg_layer(len::MAX_ZOOM);
    }
    m_index->beg_layer(beg);
}
void handle::end_layer(const uint8_t& end) noexcept
{
    if (!m_index)
        return;
    if (end > len::MAX_ZOOM)
    {
        SLOG_WARN("超过最大支持层级:{}, 自动改为支持的最大层级")
        m_index->end_layer(len::MAX_ZOOM);
    }
    m_index->end_layer(end);
}

void handle::version(const char ver[4])
{
    if (PYRAMID_MATCH_VERSION(PYRAMID_VERSION_2, ver))
    {
    }
    else if (PYRAMID_MATCH_VERSION(PYRAMID_VERSION_1, ver))
    {
    }
    else
    {
        throw std::invalid_argument("无效的版本号");
    }
}

void handle::bound(const silly_rect& rect)
{
    if (!m_index || !m_info)
        return;
    bool is_lonlat = rect.max.x > -180 && rect.max.x < 180 && rect.max.y > -90 && rect.max.y < 90 && rect.min.x > -180 && rect.min.x < 180 && rect.min.y > -90 && rect.min.y < 90;
    if (!is_lonlat)
        throw std::invalid_argument("无效的经纬度范围 ");
    m_index->bound(rect);
    m_info->bound(rect);
}

void handle::close()
{
    m_index->close();
    m_data->close();
    m_info->close();
}

size_t handle::brow(const uint8_t& layer) const
{
    if (m_index)
    {
        return m_index->brow(layer);
    }
    return 0;
}
size_t handle::bcol(const uint8_t& layer) const
{
    if (m_index)
    {
        return m_index->bcol(layer);
    }
    return 0;
}
size_t handle::erow(const uint8_t& layer) const
{
    if (m_index)
    {
        return m_index->erow(layer);
    }
    return 0;
}
size_t handle::ecol(const uint8_t& layer) const
{
    if (m_index)
    {
        return m_index->ecol(layer);
    }
    return 0;
}
size_t handle::rows(const uint8_t& layer) const
{
    if (m_index)
    {
        return m_index->rows(layer);
    }
    return 0;
}
size_t handle::cols(const uint8_t& layer) const
{
    if (m_index)
    {
        return m_index->cols(layer);
    }
    return 0;
}

std::string handle::format() const
{
    if (m_info)
    {
        return m_info->format();
    }
    return "";
}

std::string handle::project() const
{
    if (m_info)
    {
        return m_info->project();
    }
    return "";
}

void handle::format(const std::string& fmt)
{
    if (m_info && !fmt.empty())
    {
        m_info->format(fmt);
    }
}
void handle::project(const std::string& proj)
{
    if (m_info)
    {
        m_info->project(proj);
    }
}