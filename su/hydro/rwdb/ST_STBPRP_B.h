/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: ST_STBPRP_B
 * @description: 测站基本信息表(Station Basic Property)
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef ST_STBPRP_B_H
#define ST_STBPRP_B_H
#include <su_marco.h>
namespace RWDB
{
// 测站类型
enum eStationType : uint16_t
{
    INVALID = 0,            // 类型不明确  (Invalid or undefined type)
    MM = ('M' << 8) | 'M',  // 气象站  (Meteorological Station)
    ZZ = ('Z' << 8) | 'Z',  // 河道水位水文站  (River Stage (Stream Gauging) Station)
    RR = ('R' << 8) | 'R',  // 水库水文站  (Reservoir Hydrological Station)
    PP = ('P' << 8) | 'P',  // 雨量站  (Precipitation (Rainfall) Station)
    BB = ('B' << 8) | 'B',  // 蒸发站  (Evaporation Station)
    DD = ('D' << 8) | 'D',  // 堰闸水文站  (Sluice/Gate Hydrological Station)
    TT = ('T' << 8) | 'T',  // 潮位站  (Tide Gauge Station)
    DP = ('D' << 8) | 'P',  // 泵站  (Pumping Station)
    SS = ('S' << 8) | 'S',  // 墒情站  (Soil Moisture Station)
    ZG = ('Z' << 8) | 'G',  // 地下水站  (Groundwater Monitoring Station)
    ZB = ('Z' << 8) | 'B'   // 分洪水位站  (Flood Diversion Stage Station)
};
// 报讯等级
enum class eFloodReportGrade : uint8_t
{
    Invalid = 0,
    CentralCommittee = 1,  // 中央报讯站
    ProvinceKey = 2,       // 省级重点报汛站
    ProvinceNormal = 3,    // 省级一般报汛
    Others = 4,             // 其它报汛站报(气象)
    FlashFlood = 5         // 山洪报汛站
};

struct ST_STBPRP
{
    std::string STCD;                                     // char[8] 测站编码
    std::string STNM;                                     // char[30] 测站名称
    std::string RVNM;                                     // char[30] 河川名称
    std::string HNNM;                                     // char[30] 水系名称
    std::string BANM;                                     // char[30] 流域名称
    double LGTD = 0.0;                                    // double 经度, 原本是 char[7] 度分秒的格式
    double LTTD = 0.0;                                    // double 纬度
    std::string STLC;                                     // char[30] 站址
    std::string ADDVCD;                                   // char[6] 行政区划代码
    double MDBZ = 0.0;                                    // N(6, 2) 修正基值  米
    double MDPR = 0.0;                                    // N(4, 2) 修正参数
    std::string DTMNM;                                    // char[16] 基面名称
    std::string STMEL;                                    // N(7, 3) 基面高程 米
    eStationType STTP = eStationType::INVALID;            // char[2] 测站类型
    eFloodReportGrade FRGRD = eFloodReportGrade::Invalid;  // char[1] 报讯等级
    double DRNA = 0.0;                                    // N(7) 集水面积
};
}  // namespace RWDB
#endif  // ST_STBPRP_B_H
