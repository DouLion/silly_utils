/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-23
 * @file: RadarDataNaRui头文件
 * @description: 纳瑞雷达文件读写
 * @version: v1.0.1 2025-12-23 dou li yang
 */
#ifndef SILLY_NARUI_RADAR_H
#define SILLY_NARUI_RADAR_H
#include <math/silly_matrix.h>
class NaRuiRadar
{
  public:
    /**
     * @brief 读取纳瑞雷达文件
     * @param file 文件路径
     * @param only_head 是否仅读取头部信息
     * @return
     */
    bool read(const suPath& file, const bool& only_head = false);

protected:
    // 栅格数据
    suMatrix<float> m_matrix;
    // 头部信息
    struct
    {
        double deg_scale{10000.};
        char VolumeLabel[4] = {};  // 文件卷标（'RD' 为雷达基数据， 'GD'为衍生数据
        char VersionNo[4] = {};    // 版本号
        int FileLength = 0;        // 字节数
        // 数据文件头
        double bottom = 0.0;  // slat;// 网格区南边界，以1/10000度为计数单位
        double left = 0.0;    // wlon;// 网格区西边界，以1/10000度为计数单位
        double top = 0.0;     // nlat;// 网格区北边界，以1/10000度为计数单位
        double right = 0.0;   //  elon; // 网格区东边界，以1/10000度为计数单位
        int rows = 0;         // 5500 对应的 bottom top dlat 纬度
        int cols = 0;         // 5600 对应   right left dlon 经度
        double dlat = 0.0;    // 网格行间距, 以1/10000度为计数单位
        double dlon = 0.0;    // 网格列间距，以1/10000度为计数单位
        int calt = 0;         // 特征高度，仅CAPPI为海拔高度(米)，其余均为0.

        char varCode[8] = {};   // 产品代码
        char varUnit[8] = {};   // 数据单位
        char varName[32] = {};  // 产品中文名称

        unsigned short varID = 0;  // 产品编号

        // 拼图模式
        short mode = 0;     // 拼图模式，1-最大，2-最近，3-距离权重;
        short range = 0;    // 拼图产品为0.
        short scale = 0;    // 数值的放大倍数
        short offset = 0;   // 数值偏移量
        short clear = 0;    // 晴空区定义值  1
        short missing = 0;  // 无数据的定义值，设为 0
        short minCode = 0;  // 无数据的定义值，设为 0

        int span = 0;  // 单站产品为文件名对齐时间， 拼图产品为拼图时间间隔，单位均为秒。
        // 数据观测时间的年、月、日、时、分、秒（世界时）
        short syear = 0;
        short smonth = 0;
        short sday = 0;
        short shour = 0;
        short sminute = 0;
        short ssecond = 0;
        // 产品生成时间的年、月、日、时、分、秒（世界时）
        short eyear = 0;
        short emonth = 0;
        short eday = 0;
        short ehour = 0;
        short eminute = 0;
        short esecond = 0;

        // 区域 / 雷达ID号
        char rgnID[8] = {};     // 区域/雷达ID号
        char rgnName[52] = {};  // 区域/雷达ID号名称
        char country[20] = {};  // 国家名
        // 保留字
        char reserve[36] = {};  // 保留字节
    } m_header;

private:
    bool read_header(const suPath& file);
    bool read_grid(const suPath& file, const double& dst_scale = 0.0025);
};

#endif  // SILLY_NARUI_RADAR_H
