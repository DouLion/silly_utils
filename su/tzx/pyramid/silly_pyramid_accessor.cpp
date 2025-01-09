#include <string/silly_algorithm.h>
#include "silly_pyramid_accessor.h"
#include <files/silly_file.h>
using namespace silly::pyramid;

bool accessor::open(const std::string& root, const silly::file::memory_map::access_mode& mode, bool usemmap)
{
    /*m_root = root;
    return true;*/
    std::filesystem::path pyramid_root(root);
    std::string info_path = std::filesystem::path(root).append(INFO_NAME).string();
    std::string index_path = std::filesystem::path(root).append(INDEX_NAME).string();
    std::string data_path = std::filesystem::path(root).append(DATA_NAME).string();
    if (silly::file::memory_map::access_mode::ReadOnly == mode)  //
    {
        if (!std::filesystem::exists(info_path) || !std::filesystem::exists(index_path) || !std::filesystem::exists(data_path))
        {
            m_err = "路径不存在: " + root;
            return false;
        }

        if (!m_info.open(info_path.c_str(), silly::file::memory_map::access_mode::ReadOnly, false))
        {
            m_info.close();
            return false;
        }
        m_info.read();
        if (!m_data.m_index.open(index_path.c_str(), silly::file::memory_map::access_mode::ReadOnly, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            return false;
        }

        if (!m_data.open(data_path.c_str(), silly::file::memory_map::access_mode::ReadOnly, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            m_data.close();
            return false;
        }
    }
    else
    {
        if (!m_info.open(info_path.c_str(), mode, usemmap))
        {
            m_info.close();
            return false;
        }

        if (!m_data.m_index.open(index_path.c_str(), silly::file::memory_map::access_mode::ReadOnly, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            return false;
        }

        if (!m_data.open(data_path.c_str(), silly::file::memory_map::access_mode::ReadOnly, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            m_data.close();
            return false;
        }
    }
    m_root = root;
    return true;
}

void accessor::close()
{
    m_info.close();
    m_data.m_index.close();
    m_data.close();
}

std::string accessor::read(const block& blk)
{
    return m_data.read(blk);
}

bool accessor::write(const block& blk)
{
    return false;
}

bool accessor::rebuild_to_v2(const std::string& target_root)
{
    writeopt wo;
    accessor accv2;
    accv2.m_root = target_root;
    accv2.m_mode = silly::file::memory_map::access_mode::ReadWrite;
    if (!accv2.open())
    {
        return false;
    }
    auto bdstr = silly::str::algo::split(m_info.bound(), ',');
    silly_rect bound;
    bound.min.x = std::stod(bdstr[0]);
    bound.max.y = std::stod(bdstr[1]);
    bound.max.x = std::stod(bdstr[2]);
    bound.min.y = std::stod(bdstr[3]);
    accv2.m_info.format(m_info.format());
    accv2.m_info.bound(m_info.bound());
    accv2.m_info.source(m_info.source());
    accv2.m_info.project(m_info.project());
    //// index
    accv2.m_data.m_index.m_pack.blayer = m_data.m_index.m_pack.blayer;
    accv2.m_data.m_index.m_pack.elayer = m_data.m_index.m_pack.elayer;
    // accv2.m_data.m_index.m_pack.elayer = 7;
    accv2.m_data.m_index.m_pack.bound = bound;
    if (!accv2.m_data.m_index.init())
    {
        return false;
    }
    accv2.m_data.write();
    size_t pos = len::VER + len::HEAD;
    idx_pack* pack = &accv2.m_data.m_index.m_pack;
    for(uint8_t l = pack->blayer; l <= pack->elayer; ++l)
    {
        SLOG_DEBUG("处理: {} 层", l)
        for(size_t r = pack->layers[l].brow; r <= pack->layers[l].erow; ++r)
        {
            for(size_t c = pack->layers[l].bcol; c <= pack->layers[l].ecol; ++c)
            {
                block blk;
                blk.zoom = l;
                blk.row = r;
                blk.col = c;
                if(m_data.read(blk))
                {
                   /* std::string path = std::filesystem::path(accv2.m_root).append(std::to_string(l)+ "." +std::to_string(r)+ "." +std::to_string(c) +".jpeg").string();
                    silly::file::tools::write(path, blk.data);*/
                   if(pos == blk.pos)
                   {
                       int aa = 0;
                   }
                    blk.pos = pos;
                    accv2.m_data.write(blk);
                    pos+= blk.size;
                    accv2.m_data.m_index.write(blk);
                }

            }
        }
    }
    accv2.m_info.write();
    accv2.close();
    return true;


}

std::string accessor::err()
{
    return m_err;
}

bool accessor::backup()
{
    int i = 0;
    while (++i)
    {
        std::string path = m_root + std::to_string(i);
        try
        {
            if (!std::filesystem::exists(path))
            {
                std::filesystem::rename(m_root, path);
                return true;
            }
        }
        catch (const std::exception& e)
        {
            SLOG_ERROR(e.what())
        }
    }
    return false;
}
bool accessor::begin_read(const readopt& opt)
{
    return false;
}
bool accessor::begin_write(const writeopt& opt)
{
    try
    {
        m_root = opt.root;
        m_mode = silly::file::memory_map::access_mode::ReadWrite;
        if (opt.clean)
        {
            std::filesystem::remove_all(opt.root);
        }
        else
        {
            backup();
        }
        if (!open())
        {
            return false;
        }
        //// information
        m_info.format(opt.format);
        char bound_buff[len::INFO_BOUND] = {0};
        // left top right bottom
        sprintf(bound_buff, "%.10f,%.10f,%.10f,%.10f", opt.bound.min.x, opt.bound.max.y, opt.bound.max.x, opt.bound.min.y);
        m_info.bound(bound_buff);
        m_info.source(opt.src);
        m_info.project(opt.proj);
        //// index
        m_data.m_index.m_pack.blayer = opt.blayer;
        m_data.m_index.m_pack.elayer = opt.elayer;
        m_data.m_index.m_pack.bound = opt.bound;
        if (!m_data.m_index.m_pack.init())
        {
            return false;
        }
    }
    catch (const std::exception& e)
    {
        SLOG_ERROR(e.what())
        return false;
    }

    return true;
}
bool accessor::open()
{
    std::filesystem::path pyramid_root(m_root);
    std::string info_path = std::filesystem::path(m_root).append(INFO_NAME).string();
    std::string index_path = std::filesystem::path(m_root).append(INDEX_NAME).string();
    std::string data_path = std::filesystem::path(m_root).append(DATA_NAME).string();
    if (silly::file::memory_map::access_mode::ReadOnly == m_mode)  //
    {
        if (!std::filesystem::exists(info_path) || !std::filesystem::exists(index_path) || !std::filesystem::exists(data_path))
        {
            m_err = "路径不存在: " + m_root;
            return false;
        }

        if (!m_info.open(info_path.c_str(), m_mode, true))
        {
            m_info.close();
            return false;
        }

        if (!m_data.m_index.open(index_path.c_str(), m_mode, true))
        {
            m_info.close();
            m_data.m_index.close();
            return false;
        }

        if (!m_data.open(data_path.c_str(), m_mode, true))
        {
            m_info.close();
            m_data.m_index.close();
            m_data.close();
            return false;
        }
    }
    else if (silly::file::memory_map::access_mode::ReadWrite == m_mode)
    {
        std::filesystem::create_directories(m_root);
        if (!m_info.open(info_path.c_str(), m_mode, false))
        {
            m_info.close();
            return false;
        }

        if (!m_data.m_index.open(index_path.c_str(), m_mode, false))
        {
            m_info.close();
            m_data.m_index.close();
            return false;
        }

        if (!m_data.open(data_path.c_str(), m_mode, false))
        {
            m_info.close();
            m_data.m_index.close();
            m_data.close();
            return false;
        }
    }
    return true;
}
bool accessor::end_read()
{
    return false;
}
bool accessor::end_write()
{
    return false;
}
