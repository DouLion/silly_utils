//
// Created by dell on 2023/8/14.
//

#include "silly_mapbox.h"
#include <mapbox/mbtiles/silly_mbtiles_metadata.h>
#include <mapbox/mbtiles/dtoa_milo.h>

MVTTile suMapBox::Merge(const std::vector<MVTTile>& tiles)
{
    MVTTile ret;

    for (const auto& tile : tiles)
    {
        Merge(tile, ret);
    }
    return ret;
}

bool suMapBox::Merge(const MVTTile& src, MVTTile& dst)
{
    int features_added = 0;
    for (size_t l = 0; l < src.layers.size(); l++)
    {
        MVTLayer layer = src.layers[l];

        dst.layers.emplace_back();  // 直接默认构造
        MVTLayer& outlayer = dst.layers.back();
        outlayer.name = layer.name;
        outlayer.version = layer.version;
        outlayer.extent = layer.extent;

        for (size_t f = 0; f < layer.features.size(); f++)
        {
            MVTFeature feat = layer.features[f];
            std::set<std::string> exclude_attributes;

            MVTFeature outfeature;
            int matched = 0;
            if (feat.has_id)
            {
                outfeature.has_id = true;
                outfeature.id = feat.id;
            }

            std::map<std::string, std::pair<MVTValue, TypeAndString>> attributes;
            std::vector<std::string> key_order;

            for (size_t t = 0; t + 1 < feat.tags.size(); t += 2)
            {
                const char* key = layer.keys[feat.tags[t]].c_str();
                MVTValue& val = layer.values[feat.tags[t + 1]];
                std::string value;
                int type = -1;

                switch (val.type)
                {
                    case eMVTValueType::mvt_string:
                        value = val.string_value;
                        type = static_cast<int>(eMVTValueType::mvt_string);
                        break;

                    case eMVTValueType::mvt_int:
                        value = std::to_string(val.numeric_value.int_value);
                        type = static_cast<int>(eMVTValueType::mvt_double);
                        break;

                    case eMVTValueType::mvt_sint:
                        value = std::to_string(val.numeric_value.sint_value);
                        type = static_cast<int>(eMVTValueType::mvt_double);
                        break;

                    case eMVTValueType::mvt_uint:
                        value = std::to_string(val.numeric_value.uint_value);
                        type = static_cast<int>(eMVTValueType::mvt_double);
                        break;

                    case eMVTValueType::mvt_bool:
                        value = val.numeric_value.bool_value ? "true" : "false";
                        type = static_cast<int>(eMVTValueType::mvt_bool);
                        break;

                    case eMVTValueType::mvt_float:
                        value = milo::dtoa_milo(val.numeric_value.float_value);
                        type = static_cast<int>(eMVTValueType::mvt_double);
                        break;

                    case eMVTValueType::mvt_double:
                        value = milo::dtoa_milo(val.numeric_value.double_value);
                        type = static_cast<int>(eMVTValueType::mvt_double);
                        break;

                    default:
                        continue;  // 跳过不支持的类型
                }

                TypeAndString tas;
                tas.type = type;
                tas.string = value;
                attributes[key] = {val, tas};
                key_order.push_back(key);
            }

            // 保持属性的原始顺序而不是字母顺序
            for (auto k : key_order)
            {
                if (attributes.find(k) != std::end(attributes))
                {
                    outlayer.tag(outfeature, k, std::move(attributes.at(k).first));
                    attributes.erase(k);
                }
            }

            outfeature.type = feat.type;
            outfeature.geometry = feat.geometry;

            features_added++;
            outlayer.features.emplace_back(std::move(outfeature));
        }
    }
    return false;
}