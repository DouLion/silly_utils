
#ifndef SILLY_TURN_TOOL_H
#define SILLY_TURN_TOOL_H

#include <su_marco.h>

#define P2INT8(x) ((int8_t*)((x)))[0]
#define P2INT16(x) ((int16_t*)((x)))[0]
#define P2INT32(x) ((int32_t*)((x)))[0]
#define P2INT64(x) ((int64_t*)((x)))[0]
#define P2UINT8(x) ((int8_t*)((x)))[0]
#define P2UINT16(x) ((int16_t*)((x)))[0]
#define P2UINT32(x) ((int32_t*)((x)))[0]
#define P2UINT64(x) ((int64_t*)((x)))[0]
#define P2FLOAT(x) ((float*)((x)))[0]
#define P2DOUBLE(x) ((double*)((x))[0]

static std::string v2bin(uint8_t v)
{
    std::string result;
    result.resize(sizeof(v));
    memcpy(&result[0], &v, sizeof(v));
    return result;
}

static std::string v2bin(double v)
{
    std::string result;
    result.resize(sizeof(v));
    memcpy(&result[0], &v, sizeof(v));
    return result;
}

static std::string v2bin(uint32_t v)
{
    std::string result;
    result.resize(sizeof(v));
    memcpy(&result[0], &v, sizeof(v));
    return result;
}

static std::string v2bin(int32_t v)
{
    std::string result;
    result.resize(sizeof(v));
    memcpy(&result[0], &v, sizeof(v));
    return result;
}

static std::string str2bin(std::string str)
{
    std::string result;
    result.resize(1);
    result[0] = static_cast<uint8_t>(str.size());
    result.append(str);
    return result;
}
static std::string bin2str(const char* str)
{
    uint8_t size = str[0];
    std::string result;
    result.resize(size);
    memcpy(&result[0], &str[1], size);
    return result;
}

#endif  // SILLY_TURN_TOOL_H