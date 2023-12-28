/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/28 10:16
 * @version: 1.0.1
 * @description: dem数据读写
 */
#ifndef SILLY_UTILS_SILLY_DEM_H
#define SILLY_UTILS_SILLY_DEM_H
#include <su_marco.h>
#include <vector>
/*ncols         9478
nrows         7183
xllcorner     218990.98442769
yllcorner     3278474.0715599
cellsize      9.8080330000000
NODATA_value  -9999
99.19468 99.05762 98.98864*/

class silly_dem_frame
{
public:
    size_t ncols;
    size_t nrows;
    float xllcorner;
    float yllcorner;
    float cellsize;
    float nodata;
    std::vector<float> data;
};


class silly_dem_utils
{
public:
    static bool read(const std::string& path, silly_dem_frame& dem_frm);
    static bool mmap_read(const std::string& path, silly_dem_frame& dem_frm);
};

#endif //SILLY_UTILS_SILLY_DEM_H
