/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: suMBTileDB.h
 * @description: suMBTileDB 类声明
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#ifndef SILLY_MBTILES_SQLITE3_H
#define SILLY_MBTILES_SQLITE3_H
#include <su_marco.h>
#include <sqlite3.h>
#include <mapbox/mbtiles/silly_mbtiles_metadata.h>
#include <mapbox/mbtiles/silly_mvt.h>

class suMBTileDB
{
  public:
    struct Option
    {
        bool create_tables = false;
        int forcetable = 0;
    };
    std::map<std::string, LayerMapEntry> MergeLayerMaps(std::vector<std::map<std::string, LayerMapEntry>> const &maps);
    std::map<std::string, LayerMapEntry> MergeLayerMaps(std::vector<std::map<std::string, LayerMapEntry>> const &maps, bool trunc);

  public:
    suMBTileDB() = default;
    ~suMBTileDB() = default;
    bool open(char *dbname, const Option &opt);

    bool write_tile(int z, int tx, int ty, const char *data, int size);

    bool write_metadata(const char *outdir, const char *fname, const suMBTileMetadata &data);

    bool close();

    bool read_tile(int z, int x, int y, char **data, int &size);

    bool read_tile(int z, int x, int y, MVTTile &layer);

    bool read_tile(int z, std::map<int, std::map<int, MVTTile>> &x_y_layer);

    bool read_tile(std::map<int, std::map<int, std::map<int, MVTTile>>> &z_x_y_layer);

    void add_to_file_keys(std::map<std::string, TypeAndStringStats> &file_keys, std::string const &layername, TypeAndString const &val);

    std::string err();

  private:
    bool create_tables(int forcetable);

  private:
    sqlite3 *m_db = nullptr;
    std::string m_err;
};

#endif  // SILLY_MBTILES_SQLITE3_H
