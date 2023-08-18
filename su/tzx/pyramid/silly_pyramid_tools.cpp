#include "silly_pyramid_tools.h"
#include <filesystem>

bool silly_pyramid::open(const std::string& root, const open_mode& mode)
{
    std::filesystem::path pyrmid_root(root);
    auto info_path = pyrmid_root; info_path.append(TZX_IMAGE_DATA_INFO_NAME);
    auto index_path = pyrmid_root; index_path.append(TZX_IMAGE_DATA_INDEX_NAME);
    auto data_path = pyrmid_root; data_path.append(TZX_IMAGE_DATA_DATA_NAME);
    if (open_mode::READ == mode) // 
    {
        if (!std::filesystem::exists(info_path) || !std::filesystem::exists(index_path) || !std::filesystem::exists(data_path))
        {
            printf("%s_%d: Cannot read %s with read mode.", __FILE__, __LINE__, std::filesystem::absolute(pyrmid_root).string().c_str());
            return false;
        }
       
        if (!m_info.open(info_path.string().c_str(), open_mode::READ))
        {
            m_info.close();
            return false;
        }

        if (!m_index.open(index_path.string().c_str(), open_mode::READ))
        {
            m_info.close();
            m_index.close();
            return false;
        }

        if (!m_data.open(data_path.string().c_str(), open_mode::READ))
        {
            m_info.close();
            m_index.close();
            m_data.close();
            return false;
        }
    }
    else
    {

        if (!m_info.open(info_path.string().c_str(), mode))
        {
            m_info.close();
            return false;
        }

        if (!m_index.open(index_path.string().c_str(), open_mode::READ))
        {
            m_info.close();
            m_index.close();
            return false;
        }

        if (!m_data.open(data_path.string().c_str(), open_mode::READ))
        {
            m_info.close();
            m_index.close();
            m_data.close();
            return false;
        }
    }
    return false;
}

void silly_pyramid::close()
{
    m_info.close();
    m_index.close();
    m_data.close();
}

char* silly_pyramid::read_data(const uint32_t& layer, const uint64_t& row, const uint64_t& col, size_t& size)
{
    return nullptr;
}

bool silly_pyramid::write(const uint32_t& layer, const uint64_t& row, const uint64_t& col, const size_t& size, const char* data)
{
    

    return false;
}
