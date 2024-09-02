/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: grid_to_rect.c
 * @description: grid_to_rect实现
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#include <geo/silly_geo_utils.h>

void help()
{
    SLOG_INFO("GridToRect <shp_file> <cell_size>\n 输入的shp文件,或者其他支持的格式  网格点间距 x和y格点间距一致")
}
int main(int argc, char** argv)
{
    std::string file;
    double cell_size;
    if (argc != 3)
    {
        help();
        return -1;
    }
    double left = 74.0, top = 54.0, right =135.0, bottom = 3.0;
    file = argv[1];
    cell_size = std::stod(argv[2]);
    silly_geo_utils::init_gdal_env();
    std::vector<silly_geo_coll> collections;
    silly_geo_utils::read_geo_coll(file, collections);
    std::vector<silly_geo_coll> new_colls;
    for (auto& coll : collections)
    {
        if (enum_geometry_type::egtPoint != coll.m_type)
        {
            continue;
        }

        silly_poly tmp;
        tmp.outer_ring.points.clear();
        tmp.outer_ring.points.push_back({coll.m_point.lgtd - cell_size/2, coll.m_point.lttd + cell_size/2});
        tmp.outer_ring.points.push_back({coll.m_point.lgtd + cell_size/2, coll.m_point.lttd + cell_size/2});
        tmp.outer_ring.points.push_back({coll.m_point.lgtd + cell_size/2, coll.m_point.lttd - cell_size/2});
        tmp.outer_ring.points.push_back({coll.m_point.lgtd - cell_size/2, coll.m_point.lttd - cell_size/2});
        tmp.outer_ring.points.push_back({coll.m_point.lgtd - cell_size/2, coll.m_point.lttd + cell_size/2});

        silly_geo_coll tmp_gc(tmp);
        tmp_gc.m_props["id"] = (int)(std::floor((coll.m_point.lttd - bottom)/cell_size) + std::floor((coll.m_point.lgtd - left)/cell_size));

        new_colls.push_back(tmp_gc);
    }

    silly_geo_utils::write_geo_coll("grid_poly.shp", new_colls);

    silly_geo_utils::destroy_gdal_env();
    return 0;
}