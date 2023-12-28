//
// Created by dell on 2023/12/28.
// dem数据读写实现
//

#include "silly_dem.h"
#include "files/silly_file.h"
#include "files/silly_mmap.h"
#include <string>

#define SILLY_DEM_DEF_NCOLS     "ncols"
#define SILLY_DEM_DEF_NROWS     "nrows"
#define SILLY_DEM_DEF_XLLCORNER     "xllcorner"
#define SILLY_DEM_DEF_YLLCORNER     "yllcorner"
#define SILLY_DEM_DEF_CELLSIZE     "cellsize"
#define SILLY_DEM_DEF_NO_DATA     "NODATA_value"

#define SILLY_DEM_SKIP_BLANK(s, off, idx)                     \
    off = idx;                                                   \
    while (s[off] == ' ' || s[off] == '\r' || s[off] == '\n')    \
    {                                                            \
        off++;                                                   \
    }                                                            \
    idx = off;                                                   \
    while (!(s[idx] == ' ' || s[idx] == '\r' || s[idx] == '\n')) \
    {                                                            \
        i++;                                                     \
    }

bool silly_dem_utils::read(const std::string& path, silly_dem_frame& dem_frm) {
	return false;
}

bool silly_dem_utils::mmap_read(const std::string& path, silly_dem_frame& dem_frm) {

	silly_mmap dem_mmap;
	if (!dem_mmap.mopen(path))
	{
		return false;
	}
	mmap_cur* cur = dem_mmap.at();
	// 跳过头部
	size_t offset = 0;
	size_t total_size = dem_mmap.size();
	while (offset < total_size)
	{
		if (cur[offset] == 'N' && cur[offset + 1] == 'O')
		{
			while (cur[offset] != '\n')
			{
				offset++;
			}
			break;
		}
		offset++;
	}
	char _dot = '.' - 1;


	while (offset < total_size)
	{
		if (cur[offset] != ' ')
		{
			size_t i = 0;
			char buff[32] = { 0 };
			while (cur[offset] > _dot)
			{
				buff[i] = cur[offset];
				i++;
				offset++;
			}
			if (i) { dem_frm.data.push_back(std::stof(buff)); }

		}
		offset++;
	}
	return false;
}
