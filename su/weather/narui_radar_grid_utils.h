/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/3/18 17:54
 * @version: 1.0.1
 * @description: 纳睿达网格文件读写工具
 */
#ifndef SILLY_UTILS_NARUI_RADAR_GRID_UTILS_H
#define SILLY_UTILS_NARUI_RADAR_GRID_UTILS_H

#include <vector>
#include <su_marco.h>
#include <math/silly_matrix.h>
using namespace silly_math;

typedef std::vector<std::vector<float>> nrd_grid;

/// <summary>
/// 纳睿达数据头
/// </summary>
struct nrd_header
{
    double deg_scale{10000.};
    char VolumeLabel[4]{{0}};  // 文件卷标（'RD' 为雷达基数据， 'GD'为衍生数据
    char VersionNo[4]{{0}};    // 版本号
    int FileLength{0};         // 字节数
    // 数据文件头
    double bottom{0.};  // slat;// 网格区南边界，以1/10000度为计数单位
    double left{0.};    // wlon;// 网格区西边界，以1/10000度为计数单位
    double top{0.};     // nlat;// 网格区北边界，以1/10000度为计数单位
    double right{0.};   //  elon; // 网格区东边界，以1/10000度为计数单位
    int rows{0};        // 5500 对应的 bottom top dlat 纬度
    int cols{0};        // 5600 对应   right left dlon 经度
    double dlat{0.};    // 网格行间距, 以1/10000度为计数单位
    double dlon{0.};    // 网格列间距，以1/10000度为计数单位
    int calt{0};        // 特征高度，仅CAPPI为海拔高度(米)，其余均为0.

    char varCode[8]{{0}};   //产品代码
    char varUnit[8]{{0}};   //数据单位
    char varName[32]{{0}};  //产品中文名称

    unsigned short varID;  // 产品编号

    // 拼图模式
    short mode{0};     // 拼图模式，1-最大，2-最近，3-距离权重;
    short range{0};    // 拼图产品为0.
    short scale{0};    // 数值的放大倍数
    short offset{0};   // 数值偏移量
    short clear{0};    // 晴空区定义值  1
    short missing{0};  // 无数据的定义值，设为 0
    short minCode{0};  // 无数据的定义值，设为 0

    int span{0};  // 单站产品为文件名对齐时间， 拼图产品为拼图时间间隔，单位均为秒。
    // 数据观测时间的年、月、日、时、分、秒（世界时）
    short syear{0};
    short smonth{0};
    short sday{0};
    short shour{0};
    short sminute{0};
    short ssecond{0};
    // 产品生成时间的年、月、日、时、分、秒（世界时）
    short eyear{0};
    short emonth{0};
    short eday{0};
    short ehour{0};
    short eminute{0};
    short esecond{0};

    // 区域 / 雷达ID号
    char rgnID[8]{{0}};     // 区域/雷达ID号
    char rgnName[52]{{0}};  // 区域/雷达ID号名称
    char country[20]{{0}};  // 国家名
    // 保留字
    char reserve[36]{{0}};  //保留字节
};

class silly_nrd_grid_utils
{
  public:
    /// <summary>
    /// 读取文件
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    bool read(const std::string& file);

  public:
    // 头部信息
    nrd_header head;
    // 栅格数据
    matrix_2d<float> grid;

  private:
    bool read_header(const std::string& file);
    bool read_grid(const std::string& file);
};

#endif  // SILLY_UTILS_NARUI_RADAR_GRID_UTILS_H
