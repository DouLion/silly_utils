//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_data.h"
using namespace silly::pyramid;
data::data()
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

bool data::open(const char* file, const silly::file::memory_map::access_mode& mode, const bool& usemmap)
{
    // std::filesystem:::
    return base::open(file, mode, usemmap);
}

bool data::open(const std::string& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap)
{
    return open(file.c_str(), mode, usemmap);
}
bool data::open(const std::filesystem::path& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap)
{
    return open(file.string().c_str(), mode, usemmap);
}

std::string data::read(const block& blk)
{
    std::string ret = "";
    block nbkl = blk;
    if (!m_index->read(nbkl))
    {
        return ret;
    }
    // std::cout << nbkl.zoom << " " << nbkl.row << " " << nbkl.col << " "<<  nbkl.pos << " " << nbkl.size << std::endl;
    ret.resize(nbkl.size);
    base::read(nbkl.pos, ret.data(), nbkl.size);
    return ret;
}

bool data::read(block& blk)
{
    if (!m_index->read(blk))
    {
        return false;
    }
    blk.create();
    return base::read(blk.pos, blk.data.data(), blk.size);
}

bool data::write(block& blk)
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


void data::set(index* idx)
{
    m_index = idx;
}

void data::close()
{
    if (m_mode == silly::file::memory_map::access_mode::Write)
    {
        base::write_info();
    }
    base::close();
}