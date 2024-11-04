/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: silly_mbtiles_metadata.cpp
 * @description: silly_mbtiles_metadata实现
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#include "silly_mbtiles_metadata.h"

const char *utf8_next(const char *s, long *c)
{
    if (s == nullptr)
    {
        *c = -1;
        return nullptr;
    }

    if (*s == '\0')
    {
        *c = -1;
        return nullptr;
    }

    if ((s[0] & 0x80) == 0x80)
    {
        if ((s[0] & 0xE0) == 0xC0)
        {
            if ((s[1] & 0xC0) != 0x80)
            {
                *c = 0xFFFD;
                s++;
            }
            else
            {
                *c = ((long)(s[0] & 0x1F) << 6) | ((long)(s[1] & 0x7F));
                s += 2;
            }
        }
        else if ((s[0] & 0xF0) == 0xE0)
        {
            if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80)
            {
                *c = 0xFFFD;
                s++;
            }
            else
            {
                *c = ((long)(s[0] & 0x0F) << 12) | ((long)(s[1] & 0x7F) << 6) | ((long)(s[2] & 0x7F));
                s += 3;
            }
        }
        else if ((s[0] & 0xF8) == 0xF0)
        {
            if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80)
            {
                *c = 0xFFFD;
                s++;
            }
            else
            {
                *c = ((long)(s[0] & 0x0F) << 18) | ((long)(s[1] & 0x7F) << 12) | ((long)(s[2] & 0x7F) << 6) | ((long)(s[3] & 0x7F));
                s += 4;
            }
        }
        else
        {
            *c = 0xFFFD;
            s++;
        }
    }
    else
    {
        *c = s[0];
        s++;
    }

    return s;
}

std::string truncate16(std::string const &s, size_t runes)
{
    const char *cp = s.c_str();
    const char *start = cp;
    const char *lastgood = cp;
    size_t len = 0;
    long c;

    while ((cp = utf8_next(cp, &c)) != nullptr)
    {
        if (c <= 0xFFFF)
        {
            len++;
        }
        else
        {
            len += 2;
        }

        if (len <= runes)
        {
            lastgood = cp;
        }
        else
        {
            break;
        }
    }

    return std::string(s, 0, lastgood - start);
}


bool type_and_string::operator<(const type_and_string &o) const
{
    if (string < o.string)
    {
        return true;
    }
    if (string == o.string && type < o.type)
    {
        return true;
    }
    return false;
}

bool type_and_string::operator!=(const type_and_string &o) const
{
    if (type != o.type)
    {
        return true;
    }
    if (string != o.string)
    {
        return true;
    }
    return false;
}


std::map<std::string, layermap_entry> silly_mbtiles_metadata::merge_layermaps(std::vector<std::map<std::string, layermap_entry>> const &maps) {
    return merge_layermaps(maps, false);
}

std::map<std::string, layermap_entry> silly_mbtiles_metadata::merge_layermaps(std::vector<std::map<std::string, layermap_entry>> const &maps, bool trunc) {
    std::map<std::string, layermap_entry> out;

    for (size_t i = 0; i < maps.size(); i++) {
        for (auto map = maps[i].begin(); map != maps[i].end(); ++map) {
            if (map->second.points + map->second.lines + map->second.polygons + map->second.retain == 0) {
                continue;
            }

            std::string layername = map->first;
            if (trunc) {
                layername = truncate16(layername, 256);
            }

            if (out.count(layername) == 0) {
                out.insert(std::pair<std::string, layermap_entry>(layername, layermap_entry(out.size())));
                auto out_entry = out.find(layername);
                out_entry->second.minzoom = map->second.minzoom;
                out_entry->second.maxzoom = map->second.maxzoom;
                out_entry->second.description = map->second.description;
            }

            auto out_entry = out.find(layername);
            if (out_entry == out.end()) {
                fprintf(stderr, "Internal error merging layers\n");
                exit(EXIT_FAILURE);
            }

            for (auto fk = map->second.file_keys.begin(); fk != map->second.file_keys.end(); ++fk) {
                std::string attribname = fk->first;
                if (trunc) {
                    attribname = truncate16(attribname, 256);
                }

                auto fk2 = out_entry->second.file_keys.find(attribname);

                if (fk2 == out_entry->second.file_keys.end()) {
                    out_entry->second.file_keys.insert(std::pair<std::string, type_and_string_stats>(attribname, fk->second));
                } else {
                    for (auto val : fk->second.sample_values) {
                        auto pt = std::lower_bound(fk2->second.sample_values.begin(), fk2->second.sample_values.end(), val);
                        if (pt == fk2->second.sample_values.end() || *pt != val) {  // not found
                            fk2->second.sample_values.insert(pt, val);

                            if (fk2->second.sample_values.size() > max_tilestats_sample_values) {
                                fk2->second.sample_values.pop_back();
                            }
                        }
                    }

                    fk2->second.type |= fk->second.type;

                    if (fk->second.min < fk2->second.min) {
                        fk2->second.min = fk->second.min;
                    }
                    if (fk->second.max > fk2->second.max) {
                        fk2->second.max = fk->second.max;
                    }
                }
            }

            if (map->second.minzoom < out_entry->second.minzoom) {
                out_entry->second.minzoom = map->second.minzoom;
            }
            if (map->second.maxzoom > out_entry->second.maxzoom) {
                out_entry->second.maxzoom = map->second.maxzoom;
            }

            out_entry->second.points += map->second.points;
            out_entry->second.lines += map->second.lines;
            out_entry->second.polygons += map->second.polygons;
        }
    }

    return out;
}
