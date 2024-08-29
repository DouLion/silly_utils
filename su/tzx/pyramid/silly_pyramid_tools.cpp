#include "silly_pyramid_tools.h"
#include <filesystem>

bool silly_pyramid::open(const std::string& root, const silly_mmap::enum_mmap_open_mode& mode, bool usemmap)
{
    std::filesystem::path pyrmid_root(root);
    auto info_path = pyrmid_root;
    info_path.append(TZX_IMAGE_DATA_INFO_NAME);
    auto index_path = pyrmid_root;
    index_path.append(TZX_IMAGE_DATA_INDEX_NAME);
    auto data_path = pyrmid_root;
    data_path.append(TZX_IMAGE_DATA_DATA_NAME);
    if (silly_mmap::enum_mmap_open_mode::emomRead == mode)  //
    {
        if (!std::filesystem::exists(info_path) || !std::filesystem::exists(index_path) || !std::filesystem::exists(data_path))
        {
            printf("%s_%d: Cannot read %s with read mode.", __FILE__, __LINE__, std::filesystem::absolute(pyrmid_root).string().c_str());
            return false;
        }

        if (!m_info.open(info_path.string().c_str(), silly_mmap::enum_mmap_open_mode::emomRead, usemmap))
        {
            m_info.close();
            return false;
        }

        if (!m_data.m_index.open(index_path.string().c_str(), silly_mmap::enum_mmap_open_mode::emomRead, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            return false;
        }

        if (!m_data.open(data_path.string().c_str(), silly_mmap::enum_mmap_open_mode::emomRead, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            m_data.close();
            return false;
        }
    }
    else
    {
        if (!m_info.open(info_path.string().c_str(), mode, usemmap))
        {
            m_info.close();
            return false;
        }

        if (!m_data.m_index.open(index_path.string().c_str(), silly_mmap::enum_mmap_open_mode::emomRead, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            return false;
        }

        if (!m_data.open(data_path.string().c_str(), silly_mmap::enum_mmap_open_mode::emomRead, usemmap))
        {
            m_info.close();
            m_data.m_index.close();
            m_data.close();
            return false;
        }
    }
    return false;
}

void silly_pyramid::close()
{
    m_info.close();
    m_data.m_index.close();
    m_data.close();
}

char* silly_pyramid::read_data(const uint32_t& layer, const uint64_t& row, const uint64_t& col, size_t& size)
{
    // char* ret = nullptr;
    uint32_t ss = 0;
    char* ret = m_data.read_block(layer, row, col, ss);
    size = ss;
    return ret;
}

bool silly_pyramid::write(const uint32_t& layer, const uint64_t& row, const uint64_t& col, const size_t& size, const char* data)
{
    return false;
}

bool silly_pyramid::rebuild_to_v2(const std::string& target_root)
{
    return false;
}
