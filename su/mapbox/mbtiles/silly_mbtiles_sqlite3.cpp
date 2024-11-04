/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: silly_mbtiles_sqlite3.cpp
 * @description: silly_mbtiles_sqlite3实现
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#include "silly_mbtiles_sqlite3.h"
#include "silly_mvt.h"
#include "json/silly_jsonpp.h"
#include "silly_mbtiles_text.h"

bool silly_mbtiles_sqlite3::open(char *dbname, const option &opt)
{
    bool status = false;
    if (m_db)
    {
        m_err = "Opened m_db";
        return status;
    }
    if (sqlite3_open(dbname, &m_db) != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        return status;
    }

    if (opt.create_tables)
    {
        status = create_tables(opt.forcetable);
    }
    else
    {
        status = (m_db != nullptr);
    }

    return status;
}

bool silly_mbtiles_sqlite3::write_tile(int z, int tx, int ty, const char *data, int size)
{
    sqlite3_stmt *stmt;
    const char *query = "insert into tiles (zoom_level, tile_column, tile_row, tile_data) values (?, ?, ?, ?)";
    if (sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        fprintf(stderr, "sqlite3 insert prep failed\n");
        return false;
    }

    sqlite3_bind_int(stmt, 1, z);
    sqlite3_bind_int(stmt, 2, tx);
    sqlite3_bind_int(stmt, 3, (1 << z) - 1 - ty);
    sqlite3_bind_blob(stmt, 4, data, size, nullptr);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        fprintf(stderr, "sqlite3 insert failed: %s\n", sqlite3_errmsg(m_db));
    }
    if (sqlite3_finalize(stmt) != SQLITE_OK)
    {
        fprintf(stderr, "sqlite3 finalize failed: %s\n", sqlite3_errmsg(m_db));
    }
    return true;
}

void tilestats(std::map<std::string, layermap_entry> const &layermap1, size_t elements, Json::Value &state)
{
    // Consolidate layers/attributes whose names are truncated
    std::vector<std::map<std::string, layermap_entry>> lmv;
    lmv.push_back(layermap1);
    std::map<std::string, layermap_entry> layermap = silly_mbtiles_metadata::merge_layermaps(lmv, true);

    /*  state.json_write_hash();

      state.nospace = true;
      state.json_write_string("layerCount");
      state.json_write_unsigned(layermap.size());*/
    state["layerCount"] = layermap.size();

    /* state.nospace = true;
     state.json_write_string("layers");
     state.json_write_array();*/

    Json::Value jv_layers = Json::arrayValue;

    for (auto layer : layermap)
    {
        Json::Value jv_layer = Json::objectValue;
        /*state.nospace = true;
        state.json_write_hash();

        state.nospace = true;
        state.json_write_string("layer");
        state.json_write_string(layer.first);*/
        jv_layer["layer"] = layer.first;

        /*  state.nospace = true;
          state.json_write_string("count");
          state.json_write_unsigned(layer.second.points + layer.second.lines + layer.second.polygons);*/

        jv_layer["count"] = layer.second.points + layer.second.lines + layer.second.polygons;

        std::string geomtype = "Polygon";
        if (layer.second.points >= layer.second.lines && layer.second.points >= layer.second.polygons)
        {
            geomtype = "Point";
        }
        else if (layer.second.lines >= layer.second.polygons && layer.second.lines >= layer.second.points)
        {
            geomtype = "LineString";
        }

        /*state.nospace = true;
        state.json_write_string("geometry");
        state.json_write_string(geomtype);*/
        jv_layer["geometry"] = geomtype;

        size_t attrib_count = layer.second.file_keys.size();
        if (attrib_count > max_tilestats_attributes)
        {
            attrib_count = max_tilestats_attributes;
        }

        /*state.nospace = true;
        state.json_write_string("attributeCount");
        state.json_write_unsigned(attrib_count);*/
        jv_layer["attributeCount"] = attrib_count;
        /* state.nospace = true;
         state.json_write_string("attributes");
         state.nospace = true;
         state.json_write_array();*/

        size_t attrs = 0;
        Json::Value jv_attrs = Json::arrayValue;
        for (auto attribute : layer.second.file_keys)
        {
            if (attrs == elements)
            {
                break;
            }
            attrs++;
            Json::Value jv_attr = Json::objectValue;
            /*    state.nospace = true;
                state.json_write_hash();

                state.nospace = true;
                state.json_write_string("attribute");
                state.json_write_string(attribute.first);*/
            jv_attr["attribute"] = attribute.first;

            size_t val_count = attribute.second.sample_values.size();
            if (val_count > max_tilestats_sample_values)
            {
                val_count = max_tilestats_sample_values;
            }

            /*state.nospace = true;
            state.json_write_string("count");
            state.json_write_unsigned(val_count);*/
            jv_attr["count"] = val_count;

            int type = 0;
            for (auto s : attribute.second.sample_values)
            {
                type |= (1 << s.type);
            }

            std::string type_str;
            // No "null" because null attributes are dropped
            if (type == (1 << mvt_double))
            {
                type_str = "number";
            }
            else if (type == (1 << mvt_bool))
            {
                type_str = "boolean";
            }
            else if (type == (1 << mvt_string))
            {
                type_str = "string";
            }
            else
            {
                type_str = "mixed";
            }

            /*  state.nospace = true;
              state.json_write_string("type");
              state.json_write_string(type_str);
  */
            jv_attr["type"] = type_str;

            /*state.nospace = true;
            state.json_write_string("values");
            state.json_write_array();*/

            size_t vals = 0;
            Json::Value jv_vals = Json::arrayValue;
            for (auto value : attribute.second.sample_values)
            {
                if (vals == elements)
                {
                    break;
                }

                if (value.type == mvt_double || value.type == mvt_bool)
                {
                    vals++;

                    // state.json_write_stringified(value.string);
                    jv_vals.append(value.string);
                }
                else
                {
                    std::string trunc = silly_mbtiles_text::truncate16(value.string, 256);

                    if (trunc.size() == value.string.size())
                    {
                        vals++;

                        // state.json_write_string(value.string);
                        jv_vals.append(value.string);
                    }
                }
            }
            jv_attr["values"] = jv_vals;

            if ((type & (1 << mvt_double)) != 0)
            {
                /*state.nospace = true;
                state.json_write_string("min");
                state.json_write_number(attribute.second.min);*/
                jv_attr["min"] = attribute.second.min;
                /*   state.nospace = true;
                   state.json_write_string("max");
                   state.json_write_number(attribute.second.max);*/
                jv_attr["max"] = attribute.second.max;
            }
            jv_attrs.append(jv_attr);
        }
        jv_layer["attributes"] = jv_attrs;
    }

    state["layers"] = jv_layers;
}

bool silly_mbtiles_sqlite3::write_metadata(const char *outdir, const char *fname, const silly_mbtiles_metadata &data)
{
    char *sql, *err;

    if (m_db == nullptr)
    {
        if (sqlite3_open("", &m_db) != SQLITE_OK)
        {
            fprintf(stderr, "Temporary m_db: %s\n", sqlite3_errmsg(m_db));
            return false;
        }
        if (sqlite3_exec(m_db, "CREATE TABLE metadata (name text, value text);", nullptr, nullptr, &err) != SQLITE_OK)
        {
            fprintf(stderr, "Create metadata table: %s\n", err);
            return false;
        }
    }

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('name', %Q);", fname);
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set name in metadata: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('description', %Q);", data.description != nullptr ? data.description : fname);
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set description in metadata: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('version', %d);", 2);
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set version : %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('minzoom', %d);", data.minzoom);
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set minzoom: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('maxzoom', %d);", data.maxzoom);
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set maxzoom: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('center', '%f,%f,%d');", data.midlon, data.midlat, data.maxzoom);
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set center: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('bounds', '%f,%f,%f,%f');", data.minlon, data.minlat, data.maxlon, data.maxlat);
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set bounds: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('type', %Q);", "overlay");
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set type: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    if (data.attribution != nullptr)
    {
        sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('attribution', %Q);", data.attribution);
        if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
        {
            fprintf(stderr, "set type: %s\n", err);
            if (!data.forcetable)
            {
                return false;
            }
        }
        sqlite3_free(sql);
    }

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('format', %Q);", data.vector ? "pbf" : "png");
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set format: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    std::string version = data.program + " TZX1.0";
    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('generator', %Q);", version.c_str());
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set version: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('generator_options', %Q);", data.commandline.c_str());
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "set commandline: %s\n", err);
        if (!data.forcetable)
        {
            return false;
        }
    }
    sqlite3_free(sql);

    if (data.vector)
    {
        size_t elements = max_tilestats_values;
        std::string buf;

        {
            Json::Value root = Json::objectValue;
            Json::Value jv_vector_layers = Json::arrayValue;
            std::vector<std::string> lnames;
            for (auto ai = data.layermap.begin(); ai != data.layermap.end(); ++ai)
            {
                lnames.push_back(ai->first);
            }

            for (size_t i = 0; i < lnames.size(); i++)
            {
                Json::Value jv_lm = Json::objectValue;
                auto fk = data.layermap.find(lnames[i]);
                /*state.json_write_hash();

                state.json_write_string("id");
                state.json_write_string(lnames[i]);*/
                jv_lm["id"] = lnames[i];

                /* state.json_write_string("description");
                 state.json_write_string(fk->second.description);*/
                jv_lm["description"] = fk->second.description;

                /*state.json_write_string("minzoom");
                state.json_write_signed(fk->second.minzoom);*/
                jv_lm["minzoom"] = fk->second.minzoom;

                /*  state.json_write_string("maxzoom");
                  state.json_write_signed(fk->second.maxzoom);*/
                jv_lm["maxzoom"] = fk->second.maxzoom;

                /* state.json_write_string("fields");
                 state.json_write_hash();
                 state.nospace = true;*/
                jv_lm["fields"] = Json::objectValue;

                bool first = true;
                for (auto j = fk->second.file_keys.begin(); j != fk->second.file_keys.end(); ++j)
                {
                    if (first)
                    {
                        first = false;
                    }

                    // state.json_write_string(j->first);
                    std::string strType;

                    auto f = data.attribute_descriptions.find(j->first);
                    if (f == data.attribute_descriptions.end())
                    {
                        int type = 0;
                        for (auto s : j->second.sample_values)
                        {
                            type |= (1 << s.type);
                        }

                        if (type == (1 << mvt_double))
                        {
                            strType = "Number";
                        }
                        else if (type == (1 << mvt_bool))
                        {
                            // state.json_write_string("Boolean");
                            strType = "Boolean";
                        }
                        else if (type == (1 << mvt_string))
                        {
                            // state.json_write_string("String");
                            strType = "String";
                        }
                        else
                        {
                            // state.json_write_string("Mixed");
                            strType = "Mixed";
                        }
                    }
                    /* else
                     {
                         state.json_write_string(f->second);
                     }*/
                    jv_lm["fields"][j->first] = strType;
                }
                jv_vector_layers.append(jv_lm);
            }

            root["vector_layers"] = jv_vector_layers;

            if (data.do_tilestats && elements > 0)
            {
                /* state.nospace = true;
                 state.json_write_string("tilestats");*/

                Json::Value jv_tilestats;
                tilestats(data.layermap, elements, root);
                root["tilestats"] = jv_tilestats;
            }

            buf = silly_jsonpp::to_string(root);
        }

        sql = sqlite3_mprintf("INSERT INTO metadata (name, value) VALUES ('json', %Q);", buf.c_str());
        if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK)
        {
            fprintf(stderr, "set json: %s\n", err);
            if (!data.forcetable)
            {
                return false;
            }
        }
        sqlite3_free(sql);
    }

    if (outdir != nullptr)
    {
        // TODO : dly 需要修改,
        /*std::string metadata = std::string(outdir) + "/metadata.json";

        struct stat st;
        if (stat(metadata.c_str(), &st) == 0)
        {
            // Leave existing metadata in place with --allow-existing
        }
        else
        {
            FILE *fp = fopen(metadata.c_str(), "w");
            if (fp == nullptr)
            {
                perror(metadata.c_str());
                return false;
            }

            json_writer state(fp);

            state.json_write_hash();
            state.json_write_newline();

            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(m_db, "SELECT name, value from metadata;", -1, &stmt, nullptr) == SQLITE_OK)
            {
                while (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    std::string key, value;

                    const char *k = (const char *)sqlite3_column_text(stmt, 0);
                    const char *v = (const char *)sqlite3_column_text(stmt, 1);
                    if (k == nullptr || v == nullptr)
                    {
                        fprintf(stderr, "Corrupt mbtiles file: null metadata\n");
                        return false;
                    }

                    state.json_comma_newline();
                    state.json_write_string(k);
                    state.json_write_string(v);
                }
                sqlite3_finalize(stmt);
            }

            state.json_write_newline();
            state.json_end_hash();
            state.json_write_newline();
            fclose(fp);
        }*/
    }

    if (m_db == nullptr)
    {
        if (sqlite3_close(m_db) != SQLITE_OK)
        {
            fprintf(stderr, "Could not close temp database: %s\n", sqlite3_errmsg(m_db));
            return false;
        }
    }
    return true;
}

bool silly_mbtiles_sqlite3::close()
{
    char *err;

   /* if (sqlite3_exec(m_db, "ANALYZE;", nullptr, nullptr, &err) != SQLITE_OK)
    {
        fprintf(stderr, "%s: ANALYZE failed: %s\n", pgm, err);
        return false;
    }*/
    if (sqlite3_close(m_db) != SQLITE_OK)
    {
        m_err = std::string(sqlite3_errmsg(m_db));

        return false;
    }

    return true;
}

std::map<std::string, layermap_entry> silly_mbtiles_sqlite3::merge_layermaps(std::vector<std::map<std::string, layermap_entry>> const &maps)
{
    return merge_layermaps(maps, false);
}

std::map<std::string, layermap_entry> silly_mbtiles_sqlite3::merge_layermaps(std::vector<std::map<std::string, layermap_entry>> const &maps, bool trunc)
{
    std::map<std::string, layermap_entry> out;

    for (size_t i = 0; i < maps.size(); i++)
    {
        for (auto map = maps[i].begin(); map != maps[i].end(); ++map)
        {
            if (map->second.points + map->second.lines + map->second.polygons + map->second.retain == 0)
            {
                continue;
            }

            std::string layername = map->first;
            if (trunc)
            {
                layername = silly_mbtiles_text::truncate16(layername, 256);
            }

            if (out.count(layername) == 0)
            {
                out.insert(std::pair<std::string, layermap_entry>(layername, layermap_entry(out.size())));
                auto out_entry = out.find(layername);
                out_entry->second.minzoom = map->second.minzoom;
                out_entry->second.maxzoom = map->second.maxzoom;
                out_entry->second.description = map->second.description;
            }

            auto out_entry = out.find(layername);
            if (out_entry == out.end())
            {
                fprintf(stderr, "Internal error merging layers\n");
                return out;
            }

            for (auto fk = map->second.file_keys.begin(); fk != map->second.file_keys.end(); ++fk)
            {
                std::string attribname = fk->first;
                if (trunc)
                {
                    attribname = silly_mbtiles_text::truncate16(attribname, 256);
                }

                auto fk2 = out_entry->second.file_keys.find(attribname);

                if (fk2 == out_entry->second.file_keys.end())
                {
                    out_entry->second.file_keys.insert(std::pair<std::string, type_and_string_stats>(attribname, fk->second));
                }
                else
                {
                    for (auto val : fk->second.sample_values)
                    {
                        auto pt = std::lower_bound(fk2->second.sample_values.begin(), fk2->second.sample_values.end(), val);
                        if (pt == fk2->second.sample_values.end() || *pt != val)
                        {  // not found
                            fk2->second.sample_values.insert(pt, val);

                            if (fk2->second.sample_values.size() > max_tilestats_sample_values)
                            {
                                fk2->second.sample_values.pop_back();
                            }
                        }
                    }

                    fk2->second.type |= fk->second.type;

                    if (fk->second.min < fk2->second.min)
                    {
                        fk2->second.min = fk->second.min;
                    }
                    if (fk->second.max > fk2->second.max)
                    {
                        fk2->second.max = fk->second.max;
                    }
                }
            }

            if (map->second.minzoom < out_entry->second.minzoom)
            {
                out_entry->second.minzoom = map->second.minzoom;
            }
            if (map->second.maxzoom > out_entry->second.maxzoom)
            {
                out_entry->second.maxzoom = map->second.maxzoom;
            }

            out_entry->second.points += map->second.points;
            out_entry->second.lines += map->second.lines;
            out_entry->second.polygons += map->second.polygons;
        }
    }

    return out;
}
bool silly_mbtiles_sqlite3::read_tile(int z, int x, int y, char **data, int &size)
{
    size = 0;
    if (!m_db)
    {
        return false;
    }
    char buff[1024] = {0};
    sprintf(buff, "SELECT tile_data FROM tiles WHERE zoom_level=%d AND tile_column=%d AND tile_row=%d", z, x, y);
    sqlite3_stmt *stmt;
    // 准备 SQL 语句
    int rc = sqlite3_prepare_v2(m_db, buff, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        return false;
    }

    // 执行查询并处理结果
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const void *blob_data = (char *)sqlite3_column_blob(stmt, 0);  // 获取 BLOB 数据
        size = sqlite3_column_bytes(stmt, 0);          // 获取 BLOB 大小
        if(size){
            *data = (char*)malloc(size);
            memcpy(*data, blob_data, size);
        }

    }
    /*
     * 当你从 SQLite 查询中获取 BLOB 数据时，
     * 返回的指针指向的是 SQLite 的内部结构中的数据。
     * 这些数据的生命周期与 sqlite3_stmt 语句的生命周期相同，
     * 即在调用 sqlite3_finalize() 释放准备好的语句之后，
     * SQLite 会自动释放相关的内存。
     * */
    sqlite3_finalize(stmt);
    return size > 0;
}
std::string silly_mbtiles_sqlite3::err()
{
    return m_err;
}
bool silly_mbtiles_sqlite3::create_tables(int forcetable)
{
    char *err = nullptr;
    if (sqlite3_exec(m_db, "PRAGMA synchronous=0", nullptr, nullptr, &err) != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        return false;
    }
    if (sqlite3_exec(m_db, "PRAGMA locking_mode=EXCLUSIVE", nullptr, nullptr, &err) != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        return false;
    }
    if (sqlite3_exec(m_db, "PRAGMA journal_mode=DELETE", nullptr, nullptr, &err) != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        return false;
    }
    if (sqlite3_exec(m_db, "CREATE TABLE metadata (name text, value text);", nullptr, nullptr, &err) != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        if (!forcetable)
        {
            return false;
        }
    }
    if (sqlite3_exec(m_db, "CREATE TABLE tiles (zoom_level integer, tile_column integer, tile_row integer, tile_data blob);", nullptr, nullptr, &err) != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        if (!forcetable)
        {
            return false;
        }
    }
    if (sqlite3_exec(m_db, "create unique index name on metadata (name);", nullptr, nullptr, &err) != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        if (!forcetable)
        {
            return false;
        }
    }
    if (sqlite3_exec(m_db, "create unique index tile_index on tiles (zoom_level, tile_column, tile_row);", nullptr, nullptr, &err) != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        if (!forcetable)
        {
            return false;
        }
    }
    return true;
}

void silly_mbtiles_sqlite3::add_to_file_keys(std::map<std::string, type_and_string_stats> &file_keys, std::string const &attrib, type_and_string const &val)
{
    if (val.type == mvt_null)
    {
        return;
    }

    auto fka = file_keys.find(attrib);
    if (fka == file_keys.end())
    {
        file_keys.insert(std::pair<std::string, type_and_string_stats>(attrib, type_and_string_stats()));
        fka = file_keys.find(attrib);
    }

    if (fka == file_keys.end())
    {
        m_err = "Can't happen (tilestats)\n";
        return;
    }

    if (val.type == mvt_double)
    {
        double d = atof(val.string.c_str());

        if (d < fka->second.min)
        {
            fka->second.min = d;
        }
        if (d > fka->second.max)
        {
            fka->second.max = d;
        }
    }

    auto pt = std::lower_bound(fka->second.sample_values.begin(), fka->second.sample_values.end(), val);
    if (pt == fka->second.sample_values.end() || *pt != val)
    {  // not found
        fka->second.sample_values.insert(pt, val);

        if (fka->second.sample_values.size() > max_tilestats_sample_values)
        {
            fka->second.sample_values.pop_back();
        }
    }

    fka->second.type |= (1 << val.type);
}
bool silly_mbtiles_sqlite3::read_tile(int z, int x, int y, mvt_tile &layer)
{
    bool status = false;
    if (!m_db)
    {
        return status;
    }
    char buff[1024] = {0};
    sprintf(buff, "SELECT tile_data FROM tiles WHERE zoom_level=%d AND tile_column=%d AND tile_row=%d", z, x, y);
    sqlite3_stmt *stmt;
    // 准备 SQL 语句
    int rc = sqlite3_prepare_v2(m_db, buff, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        m_err = sqlite3_errmsg(m_db);
        return status;
    }

    // 执行查询并处理结果
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const void *blob_data = (char *)sqlite3_column_blob(stmt, 0);  // 获取 BLOB 数据
        int size = sqlite3_column_bytes(stmt, 0);          // 获取 BLOB 大小
        std::string data;
        data.resize(size);
        memcpy(&data[0], blob_data, size);
        bool compressed = false;
        status = layer.decode(data, compressed);
    }
    /*
     * 当你从 SQLite 查询中获取 BLOB 数据时，
     * 返回的指针指向的是 SQLite 的内部结构中的数据。
     * 这些数据的生命周期与 sqlite3_stmt 语句的生命周期相同，
     * 即在调用 sqlite3_finalize() 释放准备好的语句之后，
     * SQLite 会自动释放相关的内存。
     * */
    sqlite3_finalize(stmt);
    return status;
}
bool silly_mbtiles_sqlite3::read_tile(int z, std::map<int, std::map<int, mvt_tile>> &x_y_layer)
{
    return false;
}
bool silly_mbtiles_sqlite3::read_tile(std::map<int, std::map<int, std::map<int, mvt_tile>>> &z_x_y_layer)
{
    return false;
}
