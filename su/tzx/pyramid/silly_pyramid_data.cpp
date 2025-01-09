//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_data.h"
using namespace silly::pyramid;
data::data()
{
    m_desc[0] = 'I';
    m_desc[1] = 'D';
    m_desc[2] = 'A';
    m_desc[3] = 'T';
}

bool data::open(const char* file, const silly::file::memory_map::access_mode& mode, const bool& usemmap)
{
    return base::open(file, mode, usemmap);
}

std::string data::read(const block& blk)
{
    std::string ret_data = "";
    block nbkl = blk;
    if (error::OK != m_index.read_block(nbkl))
    {
        return ret_data;
    }
    if (nbkl.size == 0 || nbkl.pos == 0)
    {
        return ret_data;
    }
    // std::cout << nbkl.zoom << " " << nbkl.row << " " << nbkl.col << " "<<  nbkl.pos << " " << nbkl.size << std::endl;
    ret_data.resize(nbkl.size);
    base::read(nbkl.pos, &ret_data[0], nbkl.size);
    return ret_data;
}

bool data::read(block& blk)
{
    if (error::OK != m_index.read_block(blk))
    {
        return false;
    }
    if (blk.size == 0 || blk.pos == 0)
    {
        return false;
    }
    if (blk.create())
    {
        base::read(blk.pos, blk.data, blk.size);
        return true;
    }

    return false;
}

bool data::write(const block& blk)
{
    if (blk.size && blk.data)
    {
        return base::write(blk.offset, blk.data, blk.size, 0);
    }

    return true;
}
