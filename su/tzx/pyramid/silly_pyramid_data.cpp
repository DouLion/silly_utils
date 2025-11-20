//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_data.h"
TzxPyramidData::TzxPyramidData()
{
    m_head[0] = 'I';
    m_head[1] = 'D';
    m_head[2] = 'A';
    m_head[3] = 'T';
    m_version[0] = 0x01;
    m_version[1] = 0x00;
    m_version[2] = 0x00;
    m_version[3] = 0x00;
}

bool TzxPyramidData::open(const std::filesystem::path& file, const eMMFMode& mode, const bool& usemmap)
{
    return TzxPyramidBase::open(file, mode, usemmap);
}

std::string TzxPyramidData::read(const TzxPyramidBlock& blk)
{
    std::string ret = "";
    TzxPyramidBlock nbkl = blk;
    if (!m_index->read(nbkl))
    {
        return ret;
    }
    // std::cout << nbkl.zoom << " " << nbkl.row << " " << nbkl.col << " "<<  nbkl.pos << " " << nbkl.size << std::endl;
    ret.resize(nbkl.size);
    TzxPyramidBase::read(nbkl.pos, ret.data(), nbkl.size);
    return ret;
}

bool TzxPyramidData::read(TzxPyramidBlock& blk)
{
    if (!m_index->read(blk))
    {
        return false;
    }
    blk.create();
    return TzxPyramidBase::read(blk.pos, blk.data.data(), blk.size);
}

bool TzxPyramidData::write(TzxPyramidBlock& blk)
{
    if (blk.size > 0)
    {
        std::scoped_lock lock(m_mutex);
        blk.pos = append(blk.data.data(), blk.size);
        blk.pos -= blk.size;
        return m_index->write(blk);
    }

    return false;
}

void TzxPyramidData::set(TzxPyramidIndex* idx)
{
    m_index = idx;
}

void TzxPyramidData::close()
{
    if (m_mode == eMMFMode::Write)
    {
        TzxPyramidBase::write_info();
    }
    TzxPyramidBase::close();
}