/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-19
 * @file: silly_iso_poly.cpp
 * @description: silly_iso_poly实现
 * @version: v1.0.1 2024-12-19 dou li yang
 */
#include "silly_iso_poly.h"
#include <files/silly_file.h>
#include <geo/silly_geo_utils.h>
bool silly::geo::iso_poly::read(const std::string& u8file, silly_multi_poly& mpoly)
{
    bool status = false;
    try
    {
        std::string content;
        if (0 == silly_file::read(u8file, content))
        {
            return status;
        }
        std::stringstream sstream(content);
        int part_num = 0;
        int point_num = 0;
        sstream >> part_num;

        if (part_num <= 0)
        {
            return status;
        }
        std::vector<silly_ring> rings;

        for (int i = 0; i < part_num; ++i)
        {
            sstream >> point_num;
            silly_ring ring;
            while (point_num && !sstream.eof())
            {
                double x, y;
                sstream >> x >> y;
                ring.points.emplace_back(x, y);
                point_num--;
            }

            if (point_num)
            {
                return status;
            }
            rings.push_back(ring);
        }

        if (rings.empty())
        {
            return status;
        }

        // 检查内外环
        silly_multi_poly tmp;
        tmp.resize(200);
        tmp[0].outer_ring = rings[0];
        int poly_num = 1;
        for (int i = 1; i < rings.size(); ++i)
        {
            if (rings[i].points.empty())
            {
                continue;
            }
            bool is_outer = true;
            for (auto& poly : tmp)
            {
                if (silly::geo::utils::intersect(rings[i].points.front(), poly.outer_ring.points))
                {
                    poly.inner_rings.push_back(rings[i]);
                    is_outer = false;
                    break;
                }
            }
            if (is_outer)
            {
                tmp[poly_num].outer_ring = rings[i];
                poly_num++;
            }
        }
        for (int i = 0; i < poly_num; ++i)
        {
            mpoly.push_back(tmp[i]);
        }

        status = true;
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return status;
    }

    return status;
}
bool silly::geo::iso_poly::write(const std::string& u8file, const silly_multi_poly& mpoly, const int& precision)
{
    bool status = false;
    int part_num = mpoly.size();
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision);
    try
    {
        std::ofstream ofs(u8file);
        if (!ofs.is_open())
        {
            return false;
        }
        ofs << part_num;
        for (auto& poly : mpoly)
        {
            ofs << " " << poly.outer_ring.points.size();
            for (auto& p : poly.outer_ring.points)
            {
                ofs << " "  << p.x;
                ofs << " "  << p.y;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return status;
    }
    return status;
}
