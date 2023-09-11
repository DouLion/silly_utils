//
// Created by dell on 2023/9/11.
//

#pragma once
#include "silly_station_rain.h"

void silly_station_rain::set(const std::vector<station_info>& stations)
{
    std::map<std::string, jcv_point> marked_point;
    for (auto& st : stations)
    {
        jcv_point jp;
        jp.x = (jcv_real)st.lgtd;
        jp.y = (jcv_real)st.lttd;
        marked_point[st.code] = jp;
        m_marked_station[st.code] = st;
    }
    m_creater.set_marked_ctrl_points(marked_point);
}

double silly_station_rain::average_surface_rain(const double& left, const double& top, const double& right, const double& bottom)
{
    double drp = 0.0;
    if (!m_created)
    {
        jcv_rect jr;
        jr.max.x = right;
        jr.max.y = top;
        jr.min.x = left;
        jr.min.y = bottom;
        m_creater.set_bounding_box(jr);
        m_created = m_creater.generate_thiessen(false, true);
    }

    if (m_created)
    {
        for (auto [k, p] : m_creater.GetMarkedAreaPercent())
        {
            drp += (p * m_marked_station[k].drp);
        }
    }
    return drp;
}

double silly_station_rain::point_rain(const double& lgtd, const double& lttd)
{
    return 0.0;
}
