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
    SLOG_INFO("GridToRect <shp_file> <cell_size> <output>\n 输入的shp文件,或者其他支持的格式  网格点间距 x和y格点间距一致 输出文件")
}
int main(int argc, char** argv)
{
    std::string in_file, out_file;
    double cell_size;
    if (argc != 4)
    {
        help();
        return -1; 
    }
    double left = 74.0, top = 54.0, right = 135.0, bottom = 3.0;
    in_file = __argv[1];
    cell_size = std::stod(__argv[2]);
    out_file = __argv[3];
    silly_geo_utils::init_gdal_env();
    std::vector<silly_geo_coll> collections;
    silly_geo_utils::read_geo_coll(in_file, collections);
    std::vector<silly_geo_coll> new_colls;
    int width = (right - left) / cell_size;
    int height = (top - bottom) / cell_size;

    for (auto& coll : collections)
    {
        if (enum_geometry_type::egtPoint != coll.m_type)
        {
            continue;
        }

        silly_poly tmp;
        tmp.outer_ring.points.clear();
        tmp.outer_ring.points.push_back({coll.m_point.x - cell_size / 2, coll.m_point.y + cell_size / 2});
        tmp.outer_ring.points.push_back({coll.m_point.x + cell_size / 2, coll.m_point.y + cell_size / 2});
        tmp.outer_ring.points.push_back({coll.m_point.x + cell_size / 2, coll.m_point.y - cell_size / 2});
        tmp.outer_ring.points.push_back({coll.m_point.x - cell_size / 2, coll.m_point.y - cell_size / 2});
        tmp.outer_ring.points.push_back({coll.m_point.x - cell_size / 2, coll.m_point.y + cell_size / 2});

        silly_geo_coll tmp_gc(tmp);
        int r = (coll.m_point.y - bottom) / cell_size;
        int c = (coll.m_point.x - left) / cell_size;

        tmp_gc.m_props["id"] = r * width + c;

        new_colls.push_back(tmp_gc);
    }

    silly_geo_utils::write_geo_coll(out_file.c_str(), new_colls);

    silly_geo_utils::destroy_gdal_env();
    return 0;
}