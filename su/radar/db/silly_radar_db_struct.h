/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-03
 * @file: silly_radar_db_struct.cpp
 * @description: silly_radar_db_struct 头文件
 * @version: v1.0.1 2025-12-03 dou li yang
 */
#ifndef SILLY_RADAR_DB_STRUCT_H
#define SILLY_RADAR_DB_STRUCT_H
#include "tzx/moisture/silly_moisture.h"
#include <su_marco.h>
// 接口类（不使用pack指令）
class RadarStruct {
public:
    virtual ~RadarStruct() = default;
    virtual std::string GetTypeName() const = 0;
    virtual std::vector<char> Serialize() const = 0;
    virtual bool Deserialize(const std::vector<char>& data) = 0;
    virtual void* GetRawData() = 0;
    virtual const void* GetRawData() const = 0;
};
#pragma pack(push, 1)
class NormalRainRaw
{
public:
    // 对应浮点数值 * 10 转short, 降雨只要保留1位小数即可
    int16_t rate = 0;  // 雨强
    int16_t ohp = 0;   // 过去1小时降雨
    int16_t dhp = 0;   // 过去2小时降雨
    int16_t thp = 0;   // 过去3小时降雨
    int16_t ohpf = 0;  // 预报1小时降雨
    int16_t dhpf = 0;  // 预报2小时降雨
    int16_t thpf = 0;  // 预报3小时降雨
};
#pragma pack(pop)

class NormalDRP : public RadarStruct {
public:
    NormalDRP() = default;

    std::string GetTypeName() const override {
        return "NormalDRP";
    }

    std::vector<char> Serialize() const override {
        std::vector<char> ret(sizeof(NormalRainRaw));
        std::memcpy(ret.data(), &rawData, sizeof(NormalRainRaw));
        return ret;
    }

    bool Deserialize(const std::vector<char>& data) override {
        if (data.size() != sizeof(NormalRainRaw)) {
            return false;
        }
        std::memcpy(&rawData, data.data(), sizeof(NormalRainRaw));
        return true;
    }

    void* GetRawData() override { return &rawData; }
    const void* GetRawData() const override { return &rawData; }

    // 便捷访问器
    int16_t getRate() const { return rawData.rate; }
    void setRate(int16_t rate) { rawData.rate = rate; }

    // 原始数据访问（谨慎使用）
    const NormalRainRaw& getRaw() const { return rawData; }
    NormalRainRaw& getRaw() { return rawData; }

private:
    NormalRainRaw rawData;  // 使用打包的结构体
};

template<typename RawType>
class TypedRadarData : public RadarStruct {
public:
    std::vector<char> Serialize() const override {
        std::vector<char> ret(sizeof(RawType));
        std::memcpy(ret.data(), &data, sizeof(RawType));
        return ret;
    }

    bool Deserialize(const std::vector<char>& data) override {
        if (data.size() != sizeof(RawType)) {
            return false;
        }
        std::memcpy(&this->data, data.data(), sizeof(RawType));
        return true;
    }

    void* GetRawData() override { return &data; }
    const void* GetRawData() const override { return &data; }

protected:
    RawType data;
};


// 特化类型
class NormalDRPTemplate : public TypedRadarData<NormalRainRaw> {
public:
    std::string GetTypeName() const override {
        return "NormalDRP";
    }
};

#endif  // SILLY_RADAR_DB_STRUCT_H
