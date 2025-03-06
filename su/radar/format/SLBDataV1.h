/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * Reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-28
 * @file: SLBDataV1.h
 * @description: 水利部雷达格式1.0, 缺失字段使用默认值, 不做修改
 * @version: v1.0.1 2025-02-28 dou li yang
 */
#ifndef SILLY_UTILS_SLB_DATA_V1_H
#define SILLY_UTILS_SLB_DATA_V1_H

#include "su_marco.h"
#include <radar/format/HunanData.h>

namespace radar
{
static const char VOL_LABEL_RD[4] = {'R', 'D', 0x00, 0x00};  // 雷达基数据
static const char VOL_LABEL_GD[4] = {'G', 'D', 0x00, 0x00};  // 衍生数据
static const char VOL_VER_NO[4] = {'1', '.', '0', 0x00};     // 衍生数据
enum RayOrder : uint8_t
{
    AzimuthFirst = 0,  // 为 0 表示按先方位后俯仰
    PitchFirst = 1     // 为 1 表示按先俯仰后方位方式排序
};

enum ScanMode : int32_t
{
    Pitch = 0,   // 俯仰扫描
    Azimuth = 1  // 方位扫描

};

struct FileVolume  // 文件卷标基本信息结构
{
    char VolumeLabel[4] = {0};  // 雷达基数据固定标识符，‘RD’为雷达基数据，‘GD’为衍生数据
    char VersionNo[4] = {0};
    // char FileLength = 0;  // 无压缩的数据文件字节数?
    uint64_t FileLength = 0;  // 无压缩的数据文件字节数?
    uint32_t RayOrder = PitchFirst;
};

struct SiteInfo  // 达站基本信息
{
    char Country[20] = {0};
    char Province[20] = {0};
    char SiteName[32] = {0};   // 水利测雨雷达站名，文本格式输入
    char SiteID[12] = {0};     // 水利测雨雷达站编码，文本格式输入
    char RadarType[20] = {0};  // 雷达型号
    int32_t Longitude = 0;     // 经度 / 10000.0 度
    int32_t Latitude = 0;      // 纬度 / 10000.0 度
    int32_t Height = 0;        // 高度 /10 m
    char Reserved[44] = {0};
};

struct ObserveTime  // 水利测雨雷达观测起止时间
{
    // 观测开始时间
    int16_t SYear = 0;
    int16_t SMonth = 0;
    int16_t SDay = 0;
    int16_t SHour = 0;
    int16_t SMinute = 0;
    int16_t SSecond = 0;
    // 观测结束时间
    int16_t EYear = 0;
    int16_t EMonth = 0;
    int16_t EDay = 0;
    int16_t EHour = 0;
    int16_t EMinute = 0;
    int16_t ESecond = 0;
    char Reserved[8] = {0};
};

struct OperationInfo  // 运维数据信息
{
    int32_t AntennaCenterHeight = 0;            // 天线阵面中心的海拔高度以1/10米为计数单位
    int32_t RadarTerminalVersion = 0x00000100;  // 雷达终端版本号
    float Frequency = 0;
    char Date[8] = {0};  // YYYYMMDD
    char Time[8] = {0};  // HHmmss

    // 0 – 体扫
    // 1 – 单层PPI
    // 2 – 单层RHI
    // 3 – 多层RHI
    int32_t ScanModel = 0;  // 扫描模式

    // 0-本地控制
    // 1-远程控制
    int32_t ControlFlag = 0;  // 控制权标志

    // 0-正常运行
    // 1-需维护
    // 2-故障
    // 3-正常关机
    // 4-待机
    // 5-异常关机
    int32_t SystemStatus = 0;      // 系统状态
    int32_t version = 0x00000100;  // 雷达极化标记

    // 1-单发单收
    // 2-双发双收
    // 3-单发双收
    // 4-交替发射
    // 5-其它
    int32_t DPRFlag = 1;  // 雷达极化标记

    float RotatingRate = 0.0;  // 天线转速

    // 0-关闭
    // 1-开启
    int32_t TransmitterPower = 0;  // 发射机电源

    // 0-失能
    // 1-使能
    int32_t TransmitterStatus = 0;  // 发射机发射

    // 0-位置模式
    // 1-同步模式
    int32_t MotorMode = 0;  //        电机模式

    float RadarBaseTemperature = 0.0;   // 雷达底座温度
    float RadarBaseHumidity = 0.0;      // 雷达底座湿度
    float MainPower1Temperature = 0.0;  // 主电源1温度
    float MainPower2Temperature = 0.0;  // 主电源2温度
    float MainPower1InVoltage = 0.0;    // 主电源1输入电压
    float MainPower2InVoltage = 0.0;    // 主电源2输入电压
    float MainPower1OutVoltage = 0.0;   // 主电源1输出电压
    float MainPower2OutVoltage = 0.0;   // 主电源2输出电压

    // OFF:关闭
    // ON:打开
    char AirConStatus1[10] = {0};  // 空调设备1工作状态
    char AirConStatus2[10] = {0};  // 空调设备1工作状态

    // Normal:正常
    // Abnormal:异常
    char SabA[10] = {0};  // 防雷器（设备A）状态
    char SabB[10] = {0};  // 防雷器（设备B）状态

    float PeakPower = 0.0;   // w 脉冲峰值功率
    float PulseWidth = 0.0;  // μs 脉冲宽度

    float ReflectivityExpectedValue = 0.0;  // ReflectivityExpectedValue
    float ReflectivityMeasuredValue = 0.0;  // ReflectivityMeasuredValue
    float CalibrationConstant = 0.0;        // CalibrationConstant
    float ZDRCalibrationValue = 0.0;        // ZDRCalibrationValue
    float DPCalibrationValue = 0.0;         // 差分传播相位标定值

    char Reserved[92] = {0};
};

struct AlertInfo
{
    char AlarmStartTime[14] = {0};  // 格式：YYYYMMDDHHmmss
    char AlarmEndTime[14] = {0};    // 格式：YYYYMMDDHHmmss
    int32_t AlarmNumber = 0;        // 告警代码表
    char AlarmContent[100] = {0};
    int32_t AlarmType = 3;   // 1-分系统；2-组件；3-其他
    int32_t AlarmLevel = 1;  // 1-一般；2-重要；3-严重；4-取消告警
    int32_t AlarmMode = 2;   // 1-短信；2-其他
    char Reserved[16] = {0};
};

struct Alerts
{
    int32_t AlertCounts = 0;
    char Reserved[8] = {0};
    std::vector<AlertInfo> AlertInfoList;
};

// 雷达层参数结构
struct LayerParam
{
    uint32_t Dbegin = 0;            // 本层数据记录开始位置
    int16_t ElevationAngle = 0;
    uint16_t MaxV = 0;
    uint16_t MinL = 0;
    int16_t VarCounts = 0;
    std::vector<int16_t> VarCode;     // VarCode[VarCounts]变量标识码
    std::vector<int16_t> GateCounts;  // GateCounts[VarCounts]	本层变量库数
    char Reserved2[8] = {0};
};

// 雷达数据记录块信息
struct DataRecordBlock
{
    uint16_t LayerCounts = 0;       // 仰角层数
    uint16_t RadialCounts = 0;      // 扫描线根数
    float AngularResolution = 0.0;  // 方位角分辨率
    uint32_t DataWidth = 0;         // 数据库长,以米为单位
    char Reserved[4] = {0};

    std::vector<LayerParam> LayerParamList;
};

struct RadialDataBlock
{
    // 0–仰角开始
    // 1–中间数据
    // 2–仰角结束
    // 3–体扫开始
    // 4–体扫结束
    // 5–RHI开始
    // 6–RHI结束
    int32_t RadialState = 0;

    int32_t SequenceNumber = 0;

    int32_t RadialNumber = 0;  // 方位编号

    int32_t ElevationNumber = 0;  // 仰角层编号，每个体扫从1计数
    float Azimuth = 0.0;          // 扫描的方位角度
    float Elevation = 0.0;        // 扫描的俯仰角度
    uint32_t Seconds = 0;         // UTC计数的秒数,从1970年1月1日0时开始计数;
    int32_t LengthOfData = 0;     // 仅本径向数据块所占用的长度
    int32_t VarCounts = 0;

    // 1-BYTE
    // 2-USHORT
    // 3-SHORT
    // 4-UINT
    // 5-INT
    // 6-FLOAT
    // 7-LONG
    std::vector<int32_t> DateType;  // 数值类型
    std::vector<int32_t> Scale;     // 数据编码的比例
    std::vector<int32_t> Offset;    // 数据编码的偏移

    char Reserved[12] = {0};
    int8_t OriginTh = 0;  // 原始水平反射率因子Th

    // 当[数值]为1时，表示距离模糊数据
    // 当[数值]为4时，表示无回波数据
    std::vector<int8_t> Child;   // 解码方式:([数值]-66)/2=[实际值]dBZ
    std::vector<int8_t> Zh;      // 水平反射率因子Zh;([数值]-66)/2=[实际值]dBZ
    std::vector<int8_t> V;       // 径向速度V;([数值]-129)/2=[实际值]米/秒
    std::vector<int8_t> W;       // 谱宽W ;([数值]-129)/2=[实际值]米/秒
    std::vector<int8_t> ZDR;     // 差分反射率因子ZDR;([数值]-130)/16=[实际值]dB
    std::vector<int16_t> PHIDP;  // 差分传播相位PHIDP;([数值]-50)/100=[实际值]度
    std::vector<int8_t> KDP;     // 差分传播相位率KDP;([数值]-50)/10=[实际值]度/千米
    std::vector<int8_t> CC;      // 相关系数CC;([数值]-5)/200=[实际值]
    std::vector<int8_t> Tv;      //  原始垂直反射率因子Tv;([数值]-66)/2=[实际值]dBZ
};

//struct CommonBlock
//{
//    // ---- 公共数据块 ----
//    FileVolume fileVol; // 文件卷信息
//    SiteInfo siteInfo; // 雷达站基本信息
//    ObserveTime obTime; // 观测时间
//    OperationInfo opInfo; // 运维数据信息
//    Alerts alerts;// 1~M 个告警数据块
//    std::vector<DataRecordBlock> dataRecords;// 1~N个层结构信息
//
//    // ---- 径向数据块 ----
//    std::vector<RadialDataBlock> radialData;  // 径向数据
//};

class SLBDataV1
{
  public:
    SLBDataV1() = default;
    ~SLBDataV1() = default;

    /// <summary>
    /// 由国家标准数据转为国家标准
    /// </summary>
    /// <param name="src"></param>
    /// <returns></returns>
    bool convert(const HunanData& src);

 public:

    // ---- 公共数据块 ----
    FileVolume fileVol;                        // 文件卷信息
    SiteInfo siteInfo;                         // 雷达站基本信息
    ObserveTime obTime;                        // 观测时间
    OperationInfo opInfo;                      // 运维数据信息
    Alerts alerts;                             // 1~M 个告警数据块
    std::vector<DataRecordBlock> dataRecords;  // 1~N个层结构信息

    // ---- 径向数据块 ----
    std::vector<RadialDataBlock> radialData;  // 径向数据

  private:
};

}  // namespace radar

#endif  // SILLY_UTILS_SLB_DATA_V1_H
