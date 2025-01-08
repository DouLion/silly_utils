#include "silly_pyramid_tools.h"
using namespace silly::pyramid;

bool tools::open(const std::string& root, const silly::file::memory_map::param::eAccess& mode, bool usemmap)
{
    /*m_root = root;
    return true;*/
    std::filesystem::path pyrmid_root(root);
    std::string info_path = std::filesystem::path(root).append(TZX_IMAGE_DATA_INFO_NAME).string();
    std::string index_path = std::filesystem::path(root).append(TZX_IMAGE_DATA_INDEX_NAME).string();
    std::string data_path = std::filesystem::path(root).append(TZX_IMAGE_DATA_DATA_NAME).string();
    if (silly::file::memory_map::param::eAccess::ReadOnly == mode)  //
    {
        if (!std::filesystem::exists(info_path) || !std::filesystem::exists(index_path) || !std::filesystem::exists(data_path))
        {
            m_err = "路径不存在: " + root;
            return false;
        }

        if (!m_info.open(info_path.c_str(), silly::file::memory_map::param::eAccess::ReadOnly, false))
        {
            m_info.close();
            return false;
        }

        if (!m_data.m_index.open(index_path.c_str(), silly::file::memory_map::param::eAccess::ReadOnly, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            return false;
        }

        if (!m_data.open(data_path.c_str(), silly::file::memory_map::param::eAccess::ReadOnly, usemmap))
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

        if (!m_data.m_index.open(index_path.c_str(), silly::file::memory_map::param::eAccess::ReadOnly, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            return false;
        }

        if (!m_data.open(data_path.c_str(), silly::file::memory_map::param::eAccess::ReadOnly, usemmap))
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

void tools::close()
{
    m_info.close();
    m_data.m_index.close();
    m_data.close();
}

std::string tools::read(const block& blk)
{
    return m_data.read(blk);
}

bool tools::write(const block& blk)
{
    return false;
}

bool tools::rebuild_to_v2(const std::string& target_root)
{
    return false;
}

std::string tools::err()
{
    return m_err;
}

bool tools::backup()
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
