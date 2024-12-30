/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: ColVal.h
 * @description: ColVal 类声明
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#ifndef SILLY_UTILS_COLVAL_H
#define SILLY_UTILS_COLVAL_H
#include <def/base/DateTime.h>
#include <def/ColType.h>
#include <encode/silly_encode.h>

namespace X
{
template <typename T>
static std::string& binary(const T& t)
{
    std::string ret;
    ret.resize(sizeof(T));
    memcpy(ret.data(), &t, sizeof(T));
    return ret;
}
class ColVal
{
  public:
    /// constractor
    ColVal();
    ColVal(std::string v);
    ColVal(double_t v);
    ColVal(float_t v);
    ColVal(int32_t v);
    ColVal(uint32_t v);
    ColVal(int16_t v);
    ColVal(int64_t v);
    ColVal(otl_long_string v);
    // ColVal(otl_value<otl_lob_stream> v);
    ColVal(DateTime v);
    ColVal(otl_datetime v);

    /// destructor
    ~ColVal();

    ///
    std::string AsBin() const;
    bool FromBin(const std::string& bin);


    // setter
    void Double(double_t v);
    void Float(float_t v);
    void Int32(int32_t v);
    void UInt32(uint32_t v);
    void Int16(int16_t v);
    void Int64(int64_t v);
    void Str(std::string v);
    void LStr(otl_long_string v);
    // void Blob(otl_value<otl_lob_stream> v);
    void Time(DateTime v);
    void Time(otl_datetime v);

    // getter
    void Double(otl_value<double_t> v);
    void Float(otl_value<float_t> v);
    void Int16(otl_value<int16_t> v);
    void Int32(otl_value<int32_t> v);
    void UInt32(otl_value<uint32_t> v);
    void Int64(otl_value<int64_t> v);
    void Str(otl_value<std::string> v);
    void LStr(otl_value<otl_long_string>  v);
    void Blob(otl_value<std::string> v);
    // void Blob(otl_value<otl_lob_stream> v);
    void Time(otl_value<otl_datetime> v);


  public:
    // union
    //{
        std::string str;   // 定长字符串
        double_t d;
        //float_t f;  // 是否真的需要这个
        int32_t i32;
        uint32_t u32;
        int16_t i16;
        int64_t i64;
        DateTime dt;
    //};

    bool null{false};
    ColType type{otl_var_none};
};
}
#endif  // SILLY_UTILS_COLVAL_H
