//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_index.h"
using namespace silly::pyramid;
index::index()
{
    m_desc[0] = 'I';
    m_desc[1] = 'H';
    m_desc[2] = 'D';
    m_desc[3] = 'R';
    for (int i = 0; i <= TZX_IMAGE_MAX_LEVEL; ++i)
    {
        m_layer_infos[i] = layer_info();
    }
}

bool index::open(const char* file, const silly::file::memory_map::param::eAccess& mode, const bool& usemmap)
{
    if (!base::open(file, mode, usemmap))
    {
        return false;
    }
    if (mode == silly::file::memory_map::param::eAccess::ReadOnly)
    {
        return init_layer_info();
    }
    return true;
}

error index::read_block(block& blk)
{
    auto iter = m_layer_infos.find(blk.zoom);
    if (iter == std::end(m_layer_infos))
    {
        return error::OK;
    }
    if (iter->second.out(blk))
    {
        return error::OK;
    }

    uint64_t pos = m_layer_bpos.find(blk.zoom)->second;
    if (0 == pos)
    {
        return error::OK;
    }
    auto ii = iter->second.index(blk);
    // std::cout << "Index: " << ii << std::endl;
    pos += ii * (TZX_IMAGE_DATA_POS_SIZE + TZX_IMAGE_DATA_SIZE_SIZE);
    // std::cout << "Pos: " << pos << std::endl;
    char buff[TZX_IMAGE_DATA_POS_SIZE + TZX_IMAGE_DATA_SIZE_SIZE] = {0};
    base::read(pos, buff, TZX_IMAGE_DATA_POS_SIZE + TZX_IMAGE_DATA_SIZE_SIZE);
    // read(pos + TZX_IMAGE_DATA_POS_SIZE, (char*)(&datasize), TZX_IMAGE_DATA_SIZE_SIZE);
    blk.pos = ((uint64_t*)buff)[0];
    blk.size = ((uint32_t*)buff)[2];
    // std::cout << blk.zoom << " " << blk.row << " " << blk.col << " " << blk.pos << " " << blk.size << std::endl;
    return error::OK;
}

bool index::init_layer_info()
{
    if (version::Version_2 == m_major_ver)
    {
        uint64_t pos = TZX_IMAGE_INDEX_INFO_SIZE;
        for (uint8_t l = 0; l < TZX_IMAGE_MAX_LEVEL; ++l)
        {
            char buff[40];
            read(pos, buff, 40);
            uint64_t begrow = ((uint64_t*)(buff))[0];
            uint64_t begcol = ((uint64_t*)(buff))[1];
            uint64_t endrow = ((uint64_t*)(buff))[2];
            uint64_t endcol = ((uint64_t*)(buff))[3];
            uint64_t begpos = ((uint64_t*)(buff))[4];
            pos += 5 * sizeof(uint64_t);
            m_layer_infos[l].rbeg = begrow;
            m_layer_infos[l].cbeg = begcol;
            m_layer_infos[l].rend = endrow;
            m_layer_infos[l].cend = endcol;
            m_layer_infos[l].fill();
            m_layer_bpos[l] = begpos;
        }
    }
    else if (version::Version_1 == m_major_ver)
    {
        uint8_t beglyr, endlyr = 0;

        uint64_t pos = TZX_IMAGE_INDEX_INFO_SIZE;
        read(pos, (char*)(&beglyr), TZX_IMAGE_LAYER_SIZE);
        pos += TZX_IMAGE_LAYER_SIZE;
        read(pos, (char*)(&endlyr), TZX_IMAGE_LAYER_SIZE);
        pos += TZX_IMAGE_LAYER_SIZE;

        int sss = sizeof(uint32_t);
        for (uint8_t l = beglyr; l <= endlyr; ++l)
        {
            char buff[16];
            read(pos, buff, 16);
            m_layer_infos[l].rbeg = ((uint32_t*)(buff))[0];
            m_layer_infos[l].cbeg = ((uint32_t*)(buff))[1];
            m_layer_infos[l].rend = ((uint32_t*)(buff))[2];
            m_layer_infos[l].cend = ((uint32_t*)(buff))[3];
            m_layer_infos[l].fill();
            pos += TZX_IMAGE_COLROW_SIZE * 4;
        }

        for (uint8_t l = beglyr; l <= endlyr; ++l)
        {
            m_layer_bpos[l] = pos;
            pos += m_layer_infos[l].rows * m_layer_infos[l].cols * (TZX_IMAGE_DATA_SIZE_SIZE + TZX_IMAGE_DATA_POS_SIZE);
        }
    }
    else
    {
        return false;
    }

    return true;
}

uint64_t index::get_layer_start_pos(const uint32_t& layer)
{
    return 0;
}
void index::set_layer_info(const uint32_t& layer, const layer_info& linfo)
{
    m_layer_infos[layer] = linfo;
    uint64_t pos = TZX_IMAGE_INDEX_DATA_BEGIN_POS;
    // 重新构建索引
    for (auto [l, info] : m_layer_infos)
    {
        m_layer_bpos[l] = pos;
        uint64_t rows = info.rend - info.rbeg + 1;
        uint64_t cols = info.cend - info.cbeg + 1;
        pos += (rows * cols * (TZX_IMAGE_DATA_POS_SIZE + TZX_IMAGE_DATA_SIZE_SIZE));
    }
}
bool index::write_block(const block& blk)
{
    base::write(blk.offset, (char*)(&blk.pos), sizeof(blk.pos), 0);
    base::write(sizeof(blk.pos), (char*)(&blk.size), sizeof(blk.size), 0);
    return true;
}

void index::write_layer_info()
{
    if (m_stream)
    {
        size_t pos = PYRAMID_DESC_LENGTH + PYRAMID_MVER_LENGTH + PYRAMID_PVER_LENGTH;
        for (auto [l, info] : m_layer_infos)
        {
            base::write(pos, (char*)(&info.rbeg), sizeof(info.rbeg), 0);
            pos += sizeof(info.rbeg);
            base::write(pos, (char*)(&info.cbeg), sizeof(info.cbeg), 0);
            pos += sizeof(info.cbeg);
            base::write(pos, (char*)(&info.rend), sizeof(info.rend), 0);
            pos += sizeof(info.rend);
            base::write(pos, (char*)(&info.cend), sizeof(info.cend), 0);
            pos += sizeof(info.cend);
            base::write(pos, (char*)(&m_layer_bpos[l]), sizeof(m_layer_bpos[l]), 0);
            pos += sizeof(m_layer_bpos[l]);
        }
    }
}

bool index::close()
{
    if (m_mode != silly::file::memory_map::param::eAccess::ReadOnly)
    {
        write_layer_info();
    }

    return base::close();
}
