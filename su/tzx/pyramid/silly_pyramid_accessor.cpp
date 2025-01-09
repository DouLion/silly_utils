#include "silly_pyramid_accessor.h"
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
    return false;
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
        }catch (const std::exception& e)
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
    try{
        m_root = opt.root;
        m_mode = silly::file::memory_map::access_mode::ReadWrite;
        if(opt.clean)
        {
            std::filesystem::remove_all(opt.root);
        }
        else
        {
            backup();
        }
        if(!open())
        {
            return false;
        }
        //// information
        m_info.format(opt.format);
        char bound_buff[len::INFO_BOUND] = {0};
        // left top right bottom
        sprintf(bound_buff, "left:%.10f,top:%.10f,right:%.10f,bottom:%.10f", opt.bound.min.x, opt.bound.max.y, opt.bound.max.x, opt.bound.min.y);
        m_info.bound(bound_buff);
        m_info.source(opt.src);
        m_info.project(opt.proj);
        //// index

        m_data.m_index.m_pack.bound = opt.bound;
        if(!m_data.m_index.m_pack.init())
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
