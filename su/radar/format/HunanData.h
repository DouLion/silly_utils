/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-03-05
 * @file: HunanData.h
 * @description: HunanData 类声明
 * @version: v1.0.1 2025-03-05 dou li yang
 */
#ifndef SILLY_UTILS_HUNANDATA_H
#define SILLY_UTILS_HUNANDATA_H

#include "su_marco.h"

namespace radar
{

#define ELEMENTNUM 10

struct bzHeader
{                                 // 通用头块
    char MagicNumber[4];          // 固定标志，用来指示雷达数据文件。
    unsigned short MajorVersion;  // 主版本号2
    unsigned short MinorVersion;  // 次版本号0
    int GenericType;              // 文件类型
    int ProductType;              // 产品类型
    unsigned char Reserved[16];   // 保留字段
};

struct bzRADARSITE
{                                // 站点配置块
    char SiteCode[8];            // 站号，文本格式输入
    char SiteName[32];           // 站名，文本格式输入
    float latitude;              // 天线所在纬度，FLOAT格式
    float longitude;             // 天线所在经度，FLOAT格式
    int height;                  // 天线的海拔高度以米为计数单位
    int GroundHeight;            // 雷达塔楼地面海拔高度
    float Frequency;             // 工作频率
    float BeamH;                 // 水平波束宽度，以度为计数单位
    float BeamV;                 // 垂直波束宽度，以度为计数单位
    int RDAVer;                  // 雷达数据采集软件版本号
    short int RadarType;         // 雷达类型
    short int AntennaG;          // 天线增益，以0.01db为计数单位
    short int TranLoss;          // 发射馈线损耗，编码为实际损耗 100 倍
    short int ReceLoss;          // 接收馈线损耗，编码为实际损耗 100 倍
    short int OtherLoss;         // 其他损耗，编码为实际损耗 100 倍
    unsigned char Reserved[46];  // 保留字段
};

// bzNewRadarHeader _Header;
// bzSCANPARAM _ScanCut[256];
// bzRadialDataHead _RadioHeader;
// bzRadialDatab _RadialDatab;

struct bzTASKPARAM
{                                        // 任务配置块
    unsigned char TaskName[32];          // 任务名称
    unsigned char TaskDescription[128];  // 任务描述
    int polarizations;                   /*	极化状况
                                             1 = 水平
                                             2 = 垂直
                                             3 = 水平/垂直同时
                                             4 = 水平/垂直交替*/

    int ScanType;                      /*扫描任务类型
                                         0 – 体扫
                                         1–单层PPI
                                         2 – 单层RHI
                                         3 – 单层扇扫
                                         4 – 扇体扫
                                         5 – 多层RHI
                                         6 – 手工扫描
                                         */
    int PulseWidth;                    // 脉冲宽度   ns纳秒
    int ScanStartTime;                 // 扫描开始时间 正确（应为 ScanStartTime，存在拼写错误）
    int CutNumber;                     // 扫描层数1-N
    float HorizontalNoise{0.0};        // 水平通道噪声，单位dBm分贝毫瓦
    float VerticalNoise{0.0};          // 垂直通道噪声，单位dBm分贝毫瓦
    float HorizontalCalibration{0.0};  // 水平通道标定值，单位dB
    float VerticalCalibration{0.0};    // 垂直通道标定值，单位dB
    float HorizontalNoiseTemp{0.0};    // 水平通道噪声温度，单位K
    float VerticalNoiseTemp{0.0};      // 垂直通道噪声温度，单位K
    float ZDRCalibration{0.0};         // ZDR标定偏差，单位dB
    float PHICalibration{0.0};         // 差分相移标定偏差，单位度
    float LDRCalibration{0.0};         // 系统 LDR标定偏差
    unsigned char Reserved[40];        // 保留字段
};

#pragma pack(push, 1)
struct bzSCANPARAM
{
    // 扫描配置块对于扫描任务来说，通常包括不止一个仰角或方位多的配置块依次排
    int ProcessMode;          // 处理模式
    int WaveForm;             // 波形类别
    float PRF1;               // 脉冲重复频率 1
    float PRF2;               // 脉冲重复频率 2
    int DealiasingMode;       // 速度退模糊方法
    float Azimuth;            // RHI模式的方位角，计数单位：度
    float Elevation;          // PPI模式的俯仰角，计数单位：度
    float SAngle;             // 起始方位角
    float EAngle;             // 结束方位角
    float AngularResolution;  // 角分辨率，计数单位：度
    float ScanSpeed;          // 扫描速度，计数单位：度/秒
    int LogResolution;        // 强度数据的距离分辨率 单位：米
    int DopplerResolution;    // 多普勒分辨率 单位：米
    int MaxRange1;            // 最大距离 1
    int MaxRange2;            // 最大距离 2
    int StartRange;           // 起始距离
    int Sample1;              // 采样个数1
    int Sample2;              // 采样个数2
    int PhaseMode;            // 相位编码模式
    float AtmoLoss;           // 双程大气衰减值，精度为小数点后保留 6位
    float NyquistSpeed;       // 最大不模糊速度
    long long MomentMask;     // 数据类型掩码
    long long MomentSize;     // 数据大小掩码
    int MiscFilterMask;       // 滤波设置掩码
    float SQIThreshold;       // SQI门限
    float SIGThreshold;       // SIG门限
    float CSRThreshold;       // CSR门限
    float LOGThreshold;       // LOG门限
    float CPAThreshold;       // CPA门限
    float PMIThreshold;       // PMI门限
    float DPLOGThreshold;     // DPLOG门限
    unsigned char ThresholdReserved[4];
    int dBTMask;            // dBT 质控掩码
    int dBZMask;            // dBZ 质控掩码
    int VelocityMask;       // 速度质控掩码
    int SpectrumWidthMask;  // 谱宽质控掩码
    int DPMask;             // 偏振量 质控掩码
    unsigned char MaskReserved[12];
    unsigned char Reserved[4];
    int Direction;               // 仅对 PPI 模式有效1 – 顺时针  2 – 逆
    short GClutterClasType;      // 地物杂波图
    short GClutterFilterType;    // 地物滤波类型
    short GClutterClasNW;        // 地物滤波宽度
    short GClutterFilterWindow;  // 滤波算法 FFT 窗口类型
    unsigned char Reserved2[72];
};
#pragma pack(pop)

struct bzNewRadarHeader
{
    struct bzHeader SiteInfo;
    struct bzRADARSITE PerformanceInfo;
    struct bzTASKPARAM ObservationInfo;
};

// 径向头
struct bzRadialDataHead
{                         // 径向头块提供数据状态、采集时间等信息，共 64 字节。
    int RadialState;      // 径向数据状态
    int SpotBlank;        // 消隐标志
    int SeqNumber;        // 每个体扫径向从1计数
    int RadialNumber;     // 径向数
    int ElevationNumber;  // 仰角编号
    float Azimuth;        // 方位角 0 - 360.0度
    float Elevation;      // 仰角 -2.0 - 90.0度
    int Seconds;          // 径向数据采集的时间，UTC计数的秒,从1970年1月1日0时开始计数时
    int Microseconds;     // 径向数据采集的时间除去 UTC秒数后，留下的微秒数
    int dataLength;       // 本径向数据块所占用的长度
    int MomentNumber;     // 径向数据类别（如Z，V，W等各 占一种）
    unsigned char Reserved[2];
    short HEstiNoise;  // 径向的水平通道估计噪声
    short VEstiNoise;  // 径向的垂直通道估计噪声
    unsigned char Reserved2[14];
};

// 径向数据头
struct bzRadialDatab
{                     // 径向数据块 用来存储雷达探测的径向数据资料，如反射率
    int DataType;     // 数据类型
    int Scale;        // 数据编码的比例
    int Offset;       // 数据编码的偏移
    short BinLength;  // 保存一个距离库值用的字节数
    short Flags;      // 数据标志位，暂不使用
    int length;       // 距离库数据的长度，不包括当前径向数据头大小
    unsigned char Reserved2[12];
};

struct bzDualRVP9Record
{
    std::vector<short> rawdata[360][ELEMENTNUM];
    double MinVal[ELEMENTNUM];
    double MaxVal[ELEMENTNUM];
    short Offset[ELEMENTNUM];
    short Scale[ELEMENTNUM];

    void ScanData()
    {
        int i = 0, Degree;
        for (i = 0; i < ELEMENTNUM; i++)
        {
            MinVal[i] = 32767;
            MaxVal[i] = -32768;
            if (i == 6)
            {
                MinVal[i] = (0.0 - Offset[i]) * 1.0 / Scale[i];
                MaxVal[i] = (65536.0 - Offset[i]) * 1.0 / Scale[i];
            }
            else
            {
                MinVal[i] = (0.0 - Offset[i]) * 1.0 / Scale[i];
                MaxVal[i] = (256.0 - Offset[i]) * 1.0 / Scale[i];
            }
        }
        // for (Degree = 0; Degree < 360; Degree++) {
        //	for (i = 0; i < ELEMENTNUM; i++) {
        //		for (auto data : rawdata[Degree][i]) {
        //			if (data <= -32768) {
        //				continue;
        //			}
        //			MinVal[i] = min(MinVal[i], data);
        //			MaxVal[i] = max(MaxVal[i], data);
        //		}
        //	}
        // }
    }
    void clear()
    {
        for (int Degree = 0; Degree < 360; Degree++)
        {
            for (int i = 0; i < ELEMENTNUM; i++)
            {
                rawdata[Degree][i].clear();
            }
        }
    }
};

struct radialData
{
    bzRadialDataHead RadioHeader;  // 径向数据头
    bzRadialDatab RadialDatab;     // 径向数据头
    std::vector<bzDualRVP9Record> Data;
};

class HunanData
{
  public:
    HunanData() = default;
    ~HunanData() = default;

    bool LoadData(const char *FilePath);
    void Clear();
    double GetLjVals(int Degree, int index, int Layer, double &dh, double &ddr);
    double GetVal(int Degree, int cType, int index, int Layer);
    double GetVal(int Degree, int cType, int index);
    int GetClass(int Degree, int cType, int index, int Layer);
    // double GetRain(int Degree, int index, int Layer, int Type);
    // int GetRainClass(int Degree, int index, int Layer, int Type);
    static int GetRainClass(double val);
    std::vector<double> GetAllLayer();

    // double CalcAzimuthAngleFromGeo(double LGTD, double LTTD);
    // double CalcDistance(double LGTD, double LTTD);
    // int GetIndex(double LGTD, double LTTD);
    int GetIndex(double dis);
    void GetSiteCoord(double &LGTD, double &LTTD);
    int Merge();
    int MergeLayer();

    std::shared_ptr<std::mutex> m_Mutex;
    bool ProccessMerge(int ElementID, int *FinishedNum, void *pVoid);

  public:
    // ---- 公共数据块 ----
    bzNewRadarHeader m_Header;
    bzSCANPARAM m_ScanCut[256];

    // ---- 径向数据块 ----
    bzRadialDataHead m_RadioHeader;  // 径向数据头
    bzRadialDatab m_RadialDatab;     // 径向数据头
    std::vector<bzDualRVP9Record> m_Data;

    //std::vector<radialData> m_RadialBlocks;

  private:
};





}  // namespace radar
#endif  // SILLY_UTILS_HUNANDATA_H
