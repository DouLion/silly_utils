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
bool silly::geo::iso_poly::read(const std::filesystem::path& file, suMultiPoly& mpoly)
{
    bool status = false;
    try
    {
        std::string content;
        if (0 == sufile::read(file, content))
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
        std::vector<suRing> rings;

        for (int i = 0; i < part_num; ++i)
        {
            sstream >> point_num;
            suRing ring;
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
        suMultiPoly tmp;
        tmp.resize(200);
        tmp[0].outer = rings[0];
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
                if (suGeoUtils::intersect(rings[i].points.front(), poly.outer.points))
                {
                    poly.holes.push_back(rings[i]);
                    is_outer = false;
                    break;
                }
            }
            if (is_outer)
            {
                tmp[poly_num].outer = rings[i];
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
bool silly::geo::iso_poly::write(const std::filesystem::path& file, const suMultiPoly& mpoly, const int& precision)
{
    bool status = false;
    int part_num = mpoly.size();
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision);
    try
    {
        std::ofstream ofs(file);
        if (!ofs.is_open())
        {
            return false;
        }
        ofs << part_num;
        for (auto& poly : mpoly)
        {
            ofs << " " << poly.outer.points.size();
            for (auto& p : poly.outer.points)
            {
                ofs << " " << p.x;
                ofs << " " << p.y;
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
