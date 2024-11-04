/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: silly_mbtiles_metadata.h
 * @description: silly_mbtiles_metadata 类声明
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_MBTILES_METADATA_H
#define SILLY_UTILS_SILLY_MBTILES_METADATA_H
#include <su_marco.h>
#define max_tilestats_attributes 1000
#define max_tilestats_sample_values 1000
#define max_tilestats_values 100
struct type_and_string
{
    int type = 0;
    std::string string = "";

    bool operator<(const type_and_string &o) const;
    bool operator!=(const type_and_string &o) const;
};

struct type_and_string_stats
{
    std::vector<type_and_string> sample_values = std::vector<type_and_string>();  // sorted
    double min = INFINITY;
    double max = -INFINITY;
    int type = 0;
};

struct layermap_entry
{
    size_t id = 0;
    std::map<std::string, type_and_string_stats> file_keys{};
    int minzoom = 0;
    int maxzoom = 0;
    std::string description = "";

    size_t points = 0;
    size_t lines = 0;
    size_t polygons = 0;
    size_t retain = 0;  // keep for tilestats, even if no features directly here

    layermap_entry(size_t _id)
    {
        id = _id;
    }
};

class silly_mbtiles_metadata
{
  public:
    int minzoom;
    int maxzoom;
    double minlat;
    double minlon;
    double maxlat;
    double maxlon;
    double midlat;
    double midlon;
    int forcetable;
    const char *attribution;
    std::map<std::string, layermap_entry> layermap;
    bool vector;
    const char *description;
    bool do_tilestats;
    std::map<std::string, std::string> attribute_descriptions;
    std::string program;
    std::string commandline;

    static std::map<std::string, layermap_entry> merge_layermaps(std::vector<std::map<std::string, layermap_entry> > const &maps);
    static std::map<std::string, layermap_entry> merge_layermaps(std::vector<std::map<std::string, layermap_entry> > const &maps, bool trunc);
};

#endif  // SILLY_UTILS_SILLY_MBTILES_METADATA_H
