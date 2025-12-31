#ifndef SILLY_SCHEDULE_UTILS_H
#define SILLY_SCHEDULE_UTILS_H
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <json/silly_jsonpp.h>
#include <log/silly_log.h>

// 类型大小映射表
static std::unordered_map<std::string, size_t> TYPE_SIZE = {{"int8_t", sizeof(int8_t)},
                                                     {"uint8_t", sizeof(uint8_t)},
                                                     {"int16_t", sizeof(int16_t)},
                                                     {"uint16_t", sizeof(uint16_t)},
                                                     {"int32_t", sizeof(int32_t)},
                                                     {"uint32_t", sizeof(uint32_t)},
                                                     {"int64_t", sizeof(int64_t)},
                                                     {"uint64_t", sizeof(uint64_t)},
                                                     {"float", sizeof(float)},
                                                     {"double", sizeof(double)},
                                                     {"bool", sizeof(bool)},
                                                     {"char", sizeof(char)},
                                                     {"short", sizeof(short)},
                                                     {"int", sizeof(int)},
                                                     {"long", sizeof(long)},
                                                     {"long long", sizeof(long long)}};
struct Desc
{
    std::string key;
    std::string type;
    double scale;
};

class SuScheduleUtils
{
  public:
    SuScheduleUtils(std::string& filepath);

    double get(const std::string& name, const std::string& key, std::vector<char>& data);
    std::map<std::string, double> get(const std::string& name, const std::vector<std::string>& keys, std::vector<char>& data);

  private:
    std::map<std::string, std::vector<Desc>> name2desc;

    template <typename T>
    static double extractValue(const std::vector<char>& data, size_t offset, int scale)
    {
        double ret = 0.0;
        if (offset + sizeof(T) > data.size())
        {
            SLOG_ERROR("数据长度:{},计算长度:{}", data.size(), offset + sizeof(T));
            return ret;
        }

        T value;
        memcpy(&value, data.data() + offset, sizeof(T));

        if (scale > 0)
        {
            ret = value / static_cast<double>(scale);
        }
        return ret;
    }
};

#endif  // SILLY_SCHEDULE_UTILS_H