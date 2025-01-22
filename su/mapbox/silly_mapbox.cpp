//
// Created by dell on 2023/8/14.
//

#include "silly_mapbox.h"
#include <mapbox/mbtiles/silly_mbtiles_metadata.h>
#include <mapbox/mbtiles/dtoa_milo.h>

// 从二进数据中提取出属性值
void aprintf(std::string* buf, const char* format, ...);

mvt_tile silly_mapbox::merge(const std::vector<mvt_tile>& tiles)
{
    mvt_tile ret;

    for (const auto& tile : tiles)
    {
        merge(tile, ret);
    }
    return ret;
}

bool silly_mapbox::merge(const mvt_tile& tile, mvt_tile& outtile)
{
    int features_added = 0;
    for (size_t l = 0; l < tile.layers.size(); l++)
    {
        mvt_layer layer = tile.layers[l];

        outtile.layers.push_back(mvt_layer());
        size_t ol = outtile.layers.size() - 1;
        outtile.layers[ol].name = layer.name;
        outtile.layers[ol].version = layer.version;
        outtile.layers[ol].extent = layer.extent;

        mvt_layer& outlayer = outtile.layers[ol];

        for (size_t f = 0; f < layer.features.size(); f++)
        {
            mvt_feature feat = layer.features[f];
            std::set<std::string> exclude_attributes;

            mvt_feature outfeature;
            int matched = 0;
            if (feat.has_id)
            {
                outfeature.has_id = true;
                outfeature.id = feat.id;
            }

            std::map<std::string, std::pair<mvt_value, type_and_string>> attributes;
            std::vector<std::string> key_order;

            for (size_t t = 0; t + 1 < feat.tags.size(); t += 2)
            {
                const char* key = layer.keys[feat.tags[t]].c_str();
                mvt_value& val = layer.values[feat.tags[t + 1]];
                std::string value;
                int type = -1;

                if (val.type == mvt_string)
                {
                    value = val.string_value;
                    type = mvt_string;
                }
                else if (val.type == mvt_int)
                {
                    aprintf(&value, "%lld", (long long)val.numeric_value.int_value);
                    type = mvt_double;
                }
                else if (val.type == mvt_double)
                {
                    aprintf(&value, "%s", milo::dtoa_milo(val.numeric_value.double_value).c_str());
                    type = mvt_double;
                }
                else if (val.type == mvt_float)
                {
                    aprintf(&value, "%s", milo::dtoa_milo(val.numeric_value.float_value).c_str());
                    type = mvt_double;
                }
                else if (val.type == mvt_bool)
                {
                    aprintf(&value, "%s", val.numeric_value.bool_value ? "true" : "false");
                    type = mvt_bool;
                }
                else if (val.type == mvt_sint)
                {
                    aprintf(&value, "%lld", (long long)val.numeric_value.sint_value);
                    type = mvt_double;
                }
                else if (val.type == mvt_uint)
                {
                    aprintf(&value, "%llu", (long long)val.numeric_value.uint_value);
                    type = mvt_double;
                }
                else
                {
                    continue;
                }
                if (type < 0)
                {
                    continue;
                }

                type_and_string tas;
                tas.type = type;
                tas.string = value;
                attributes.insert(std::pair<std::string, std::pair<mvt_value, type_and_string>>(key, std::pair<mvt_value, type_and_string>(val, tas)));
                key_order.push_back(key);
            }

            // if (matched || !ifmatched)
            // 保持属性的原始顺序而不是字母顺序
            for (auto k : key_order)
            {
                auto fa = attributes.find(k);

                if (fa != attributes.end())
                {
                    outlayer.tag(outfeature, k, fa->second.first);
                    // add_to_file_keys(file_keys->second.file_keys, k, fa->second.second);
                    attributes.erase(fa);
                }
            }

            outfeature.type = feat.type;
            outfeature.geometry = feat.geometry;

            features_added++;
            outlayer.features.push_back(outfeature);
        }
    }

    return false;
}

// 从二进数据中提取出属性值
void aprintf(std::string* buf, const char* format, ...)
{
    va_list ap;
    char* tmp = nullptr;
    va_start(ap, format);

    // 先计算需要的缓冲区大小
    int size = vsnprintf(nullptr, 0, format, ap);  // +1 是为了包含字符串终止符
    if (size <= 0)
    {
        fprintf(stderr, "Error calculating the size for the string\n");
        exit(EXIT_FAILURE);
    }

    // 分配内存
    tmp = (char*)malloc(size);
    if (tmp == nullptr)
    {
        fprintf(stderr, "Memory allocation failure\n");
        exit(EXIT_FAILURE);
    }

    // 再次格式化字符串到 tmp
    vsnprintf(tmp, size, format, ap);
    va_end(ap);

    buf->append(tmp, strlen(tmp));
    free(tmp);
}