/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/22 13:49
 * @version: 1.0.1
 * @software: silly_utils
 * @description: 从 https://github.com/mapbox/tippecanoe 项目中摘录过来
 */
#ifndef SILLY_MAPBOX_VECTOR_TILE_H
#define SILLY_MAPBOX_VECTOR_TILE_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include <zlib.h>

struct MVTValue;
struct MVTLayer;

enum class eMVTOperation: int
{
    Invalid = 0,
    MoveTo = 1,
    LineTo = 2,
    ClosePath = 7
};

inline bool operator==(const int& lhs, const eMVTOperation& rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}
inline bool operator==(const eMVTOperation& lhs, const int& rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}

struct MVTGeometry
{
    long long x = 0;
    long long y = 0;
    eMVTOperation /* mvt_operation */ op = eMVTOperation::Invalid;
    MVTGeometry(int op, long long x, long long y);
    MVTGeometry(eMVTOperation op, long long x, long long y);

    bool operator<(MVTGeometry const &s) const
    {
        if (y < s.y || (y == s.y && x < s.x))
        {
            return true;
        }
        return false;
    }

    bool operator==(MVTGeometry const &s) const
    {
        return y == s.y && x == s.x;
    }
};

enum class eMVTGeometryType: int
{
    Point = 1,
    LineString = 2,
    Polygon = 3
};

struct MVTFeature
{
    std::vector<unsigned> tags{};
    std::vector<MVTGeometry> geometry{};
    int /* eMVTGeometryType */ type = 0;
    unsigned long long id = 0;
    bool has_id = false;
    bool dropped = false;

    MVTFeature()
    {
        has_id = false;
        id = 0;
    }
};

enum class eMVTValueType : int
{
    mvt_string,
    mvt_float,
    mvt_double,
    mvt_int,
    mvt_uint,
    mvt_sint,
    mvt_bool,
    mvt_null,
};

inline bool operator==(const int& lhs, const eMVTValueType& rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}
inline bool operator==(const eMVTValueType& lhs, const int& rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}

struct MVTValue
{
    eMVTValueType type;
    std::string string_value;
    union
    {
        float float_value;
        double double_value;
        long long int_value;
        unsigned long long uint_value;
        long long sint_value;
        bool bool_value;
        int null_value;
    } numeric_value;

    bool operator<(const MVTValue &o) const;
    std::string toString();

    MVTValue()
    {
        this->type = eMVTValueType::mvt_double;
        this->string_value = "";
        this->numeric_value.double_value = 0;
    }
};

struct MVTLayer
{
    int version = 0;
    std::string name = "";
    std::vector<MVTFeature> features{};
    std::vector<std::string> keys{};
    std::vector<MVTValue> values{};
    long long extent = 0;

    // Add a key-value pair to a feature, using this layer's constant pool
    void tag(MVTFeature &feature, std::string key, MVTValue value);

    // For tracking the key-value constants already used in this layer
    std::map<std::string, size_t> key_map{};
    std::map<MVTValue, size_t> value_map{};
};
struct MVTTile
{
    std::vector<MVTLayer> layers{};

    std::string encode(const bool &compressed = true) const;
    bool decode(const std::string &message, bool &was_compressed);
    // https://github.com/mapbox/mapnik-vector-tile/blob/master/src/vector_tile_compression.hpp
    static bool is_compressed(std::string const &data)
    {
        return data.size() > 2 && (((uint8_t)data[0] == 0x78 && (uint8_t)data[1] == 0x9C) || ((uint8_t)data[0] == 0x1F && (uint8_t)data[1] == 0x8B));
    }

    // https://github.com/mapbox/mapnik-vector-tile/blob/master/src/vector_tile_compression.hpp
    static bool decompress(std::string const &input, std::string &output)
    {
        z_stream inflate_s;
        inflate_s.zalloc = Z_NULL;
        inflate_s.zfree = Z_NULL;
        inflate_s.opaque = Z_NULL;
        inflate_s.avail_in = 0;
        inflate_s.next_in = Z_NULL;
        if (inflateInit2(&inflate_s, 32 + 15) != Z_OK)
        {
            fprintf(stderr, "Decompression error: %s\n", inflate_s.msg);
        }
        inflate_s.next_in = (Bytef *)input.data();
        inflate_s.avail_in = input.size();
        inflate_s.next_out = (Bytef *)output.data();
        inflate_s.avail_out = output.size();

        while (true)
        {
            size_t existing_output = inflate_s.next_out - (Bytef *)output.data();

            output.resize(existing_output + 2 * inflate_s.avail_in + 100);
            inflate_s.next_out = (Bytef *)output.data() + existing_output;
            inflate_s.avail_out = output.size() - existing_output;

            int ret = inflate(&inflate_s, 0);
            if (ret < 0)
            {
                fprintf(stderr, "Decompression error: ");
                if (ret == Z_DATA_ERROR)
                {
                    fprintf(stderr, "data error");
                }
                if (ret == Z_STREAM_ERROR)
                {
                    fprintf(stderr, "stream error");
                }
                if (ret == Z_MEM_ERROR)
                {
                    fprintf(stderr, "out of memory");
                }
                if (ret == Z_BUF_ERROR)
                {
                    fprintf(stderr, "no data in buffer");
                }
                fprintf(stderr, "\n");
                return false;
            }

            if (ret == Z_STREAM_END)
            {
                break;
            }

            // ret must be Z_OK or Z_NEED_DICT;
            // continue decompresing
        }

        output.resize(inflate_s.next_out - (Bytef *)output.data());
        inflateEnd(&inflate_s);
        return true;
    }

    // https://github.com/mapbox/mapnik-vector-tile/blob/master/src/vector_tile_compression.hpp
    static bool compress(std::string const &input, std::string &output)
    {
        z_stream deflate_s;
        deflate_s.zalloc = Z_NULL;
        deflate_s.zfree = Z_NULL;
        deflate_s.opaque = Z_NULL;
        deflate_s.avail_in = 0;
        deflate_s.next_in = Z_NULL;
        deflateInit2(&deflate_s, Z_BEST_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
        deflate_s.next_in = (Bytef *)input.data();
        deflate_s.avail_in = input.size();
        size_t length = 0;
        do
        {
            size_t increase = input.size() / 2 + 1024;
            output.resize(length + increase);
            deflate_s.avail_out = increase;
            deflate_s.next_out = (Bytef *)(output.data() + length);
            int ret = deflate(&deflate_s, Z_FINISH);
            if (ret != Z_STREAM_END && ret != Z_OK && ret != Z_BUF_ERROR)
            {
                return false;
            }
            length += (increase - deflate_s.avail_out);
        } while (deflate_s.avail_out == 0);
        deflateEnd(&deflate_s);
        output.resize(length);
        return true;
    }


};

#endif  // SILLY_MAPBOX_VECTOR_TILE_H
