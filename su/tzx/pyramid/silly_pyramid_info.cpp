//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_info.h"

silly::pyramid::info::info()
{
    m_desc[0] = 'I';
    m_desc[1] = 'I';
    m_desc[2] = 'N';
    m_desc[3] = 'F';

    // m_fileVersion = BITMAP_PYRAMID_FILE_VERSION_CODE;

    memset(m_data_src, 0, sizeof(m_data_src));
    memset(m_prjection, 0, sizeof(m_prjection));
    memset(m_bound, 0, sizeof(m_bound));
    memset(m_tiling_format, 0, sizeof(m_tiling_format));
}

bool silly::pyramid::info::read()
{
    /*if (m_mmap)
    {
            memcpy(m_desc, m_mmap + PYRAMID_DESC_OFFSET, PYRAMID_DESC_LENGTH);
            memcpy((void*)m_major_ver, m_mmap + PYRAMID_MVER_OFFSET, PYRAMID_MVER_LENGTH);
            memcpy((void*)m_primary_ver, m_mmap + PYRAMID_PVER_OFFSET, PYRAMID_PVER_LENGTH);

            return true;
    }
    return false;*/
    return false;
}
