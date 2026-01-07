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
#ifndef SILLY_MBTILES_METADATA_H
#define SILLY_MBTILES_METADATA_H
#include <su_macro.h>
#define SUMB_MAX_TILE_STATS_ATTRIBUTES 1000
#define SUMB_MAX_TILE_STATS_SAMPLE_VALUES 1000
#define SUMB_MAX_TILE_STATS_VALUES 100
struct TypeAndString
{
    int type = 0;
    std::string string = "";

    bool operator<(const TypeAndString &o) const;
    bool operator!=(const TypeAndString &o) const;
};

struct TypeAndStringStats
{
    std::vector<TypeAndString> sample_values = std::vector<TypeAndString>();  // sorted
    double min = INFINITY;
    double max = -INFINITY;
    int type = 0;
};

struct LayerMapEntry
{
    size_t id = 0;
    std::map<std::string, TypeAndStringStats> file_keys{};
    int minzoom = 0;
    int maxzoom = 0;
    std::string description = "";

    size_t points = 0;
    size_t lines = 0;
    size_t polygons = 0;
    size_t retain = 0;  // keep for tilestats, even if no features directly here

    LayerMapEntry(size_t _id)
    {
        id = _id;
    }
};

class suMBTileMetadata
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
    std::map<std::string, LayerMapEntry> layermap;
    bool vector;
    const char *description;
    bool do_tilestats;
    std::map<std::string, std::string> attribute_descriptions;
    std::string program;
    std::string commandline;

    static std::map<std::string, LayerMapEntry> MergeLayerMaps(std::vector<std::map<std::string, LayerMapEntry> > const &maps);
    static std::map<std::string, LayerMapEntry> MergeLayerMaps(std::vector<std::map<std::string, LayerMapEntry> > const &maps, bool trunc);
};

#endif  // SILLY_MBTILES_METADATA_H
