/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/9/11 10:24
 * @version: 1.0.1
 * @software: Project
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_STATION_RAIN_H
#define SILLY_UTILS_SILLY_STATION_RAIN_H
#include <iostream>
#include <vector>

#include <geo/thiessen/thiessen_creator.h>

struct station_info
{
    std::string code;
    std::string name;
    double lgtd{0};
    double lttd{0};
    double drp{0};
    station_info& operator=(const station_info& other)
    {
        this->code = other.code;
        this->name = other.name;
        this->lgtd = other.lgtd;
        this->lttd = other.lttd;
        this->drp = other.drp;
        return *this;
    }
};

/// <summary>
/// 测站雨量计算相关
/// </summary>
class silly_station_rain
{
  public:
    /// <summary>
    /// 设置测站雨量信息
    /// </summary>
    /// <param name="stations"></param>
    void set(const std::vector<station_info>& stations);

    /// <summary>
    /// 获取平均面雨量
    /// </summary>
    /// <param name="left"></param>
    /// <param name="top"></param>
    /// <param name="right"></param>
    /// <param name="bottom"></param>
    /// <returns></returns>
    double average_surface_rain(const double& left, const double& top, const double& right, const double& bottom);

    /// <summary>
    /// 插值计算某个点的点雨量
    /// </summary>
    /// <param name="lgtd"></param>
    /// <param name="lttd"></param>
    /// <returns></returns>
    double point_rain(const double& lgtd, const double& lttd);

  private:
    thiessen_creator m_creater;
    bool m_created{false};
    std::map<std::string, station_info> m_marked_station;
};

#endif  // SILLY_UTILS_SILLY_STATION_RAIN_H
