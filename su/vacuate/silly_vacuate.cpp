/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-28
 * @file: silly_vacuate.cpp
 * @description: silly_vacuate实现
 * @version: v1.0.1 2024-08-28 dou li yang
 */
#include "silly_vacuate.h"

#include "vacuate/psimpl.h"

using namespace psimpl;

#include <iterator>
#include "vacuate/silly_vacuate.h"

bool silly_vacuate::n_point(const int& n, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
{
    try
    {
        auto begin = inputs.begin();
        auto end = inputs.end();
        switch (dims)
        {
            case 2:
                simplify_nth_point<2>(begin, end, n, std::back_inserter(outputs));
                break;
            case 3:
                simplify_nth_point<2>(begin, end, n, std::back_inserter(outputs));
                break;
            default:
                return false;
        }
    }
    catch (std::exception& e)
    {
        printf("%s error occ occurred: %s.", __func__, e.what());
        return false;
    }
    return true;
}

bool silly_vacuate::radial_distance(const double& radialDistance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
{
    try
    {
        auto begin = inputs.begin();
        auto end = inputs.end();
        switch (dims)
        {
            case 2:
                simplify_radial_distance<2>(begin, end, radialDistance, std::back_inserter(outputs));
                break;
            case 3:
                simplify_radial_distance<2>(begin, end, radialDistance, std::back_inserter(outputs));
                break;
            default:
                return false;
        }
    }
    catch (std::exception& e)
    {
        printf("%s error occ occurred: %s.", __func__, e.what());
        return false;
    }
    return true;
}

bool silly_vacuate::perpendicular_distance(const int& repeat, const double& distance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
{
    try
    {
        auto begin = inputs.begin();
        auto end = inputs.end();
        switch (dims)
        {
            case 2:
                simplify_perpendicular_distance<2>(begin, end, distance, repeat, std::back_inserter(outputs));
                break;
            case 3:
                simplify_perpendicular_distance<3>(begin, end, distance, repeat, std::back_inserter(outputs));
                break;
            default:
                return false;
        }
    }
    catch (std::exception& e)
    {
        printf("%s error occ occurred: %s.", __func__, e.what());
        return false;
    }
    return true;
}

bool silly_vacuate::reumann_witkam(const double& distance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
{
    try
    {
        auto begin = inputs.begin();
        auto end = inputs.end();
        switch (dims)
        {
            case 2:
                simplify_reumann_witkam<2>(begin, end, distance, std::back_inserter(outputs));
                break;
            case 3:
                simplify_reumann_witkam<2>(begin, end, distance, std::back_inserter(outputs));
                break;
            default:
                return false;
        }
    }
    catch (std::exception& e)
    {
        printf("%s error occ occurred: %s.", __func__, e.what());
        return false;
    }
    return true;
}

bool silly_vacuate::opheim(const double& minDistance, const double& maxDistance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
{
    try
    {
        auto begin = inputs.begin();
        auto end = inputs.end();
        switch (dims)
        {
            case 2:
                simplify_opheim<2>(begin, end, minDistance, maxDistance, std::back_inserter(outputs));
                break;
            case 3:
                simplify_opheim<3>(begin, end, minDistance, maxDistance, std::back_inserter(outputs));
                break;
            default:
                return false;
        }
    }
    catch (std::exception& e)
    {
        printf("%s error occ occurred: %s.", __func__, e.what());
        return false;
    }
    return true;
}

bool silly_vacuate::lang(const int& lookAhead, const double& distance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
{
    try
    {
        auto begin = inputs.begin();
        auto end = inputs.end();
        switch (dims)
        {
            case 2:
                simplify_lang<2>(begin, end, distance, lookAhead, std::back_inserter(outputs));
                break;
            case 3:
                simplify_lang<3>(begin, end, distance, lookAhead, std::back_inserter(outputs));
                break;
            default:
                return false;
        }
    }
    catch (std::exception& e)
    {
        printf("%s error occ occurred: %s.", __func__, e.what());
        return false;
    }
    return true;
}

bool silly_vacuate::douglas_peucker(const double& distance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
{
    try
    {
        auto begin = inputs.begin();
        auto end = inputs.end();
        switch (dims)
        {
            case 2:
                simplify_douglas_peucker<2>(begin, end, distance, std::back_inserter(outputs));
                break;
            case 3:
                simplify_douglas_peucker<3>(begin, end, distance, std::back_inserter(outputs));
                break;
            default:
                return false;
        }
    }
    catch (std::exception& e)
    {
        printf("%s error occ occurred: %s.", __func__, e.what());
        return false;
    }
    return true;
}

bool silly_vacuate::douglas_peucker_variant(const int& pointNum, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
{
    try
    {
        auto begin = inputs.begin();
        auto end = inputs.end();
        switch (dims)
        {
            case 2:
                simplify_douglas_peucker_n<2>(begin, end, pointNum, std::back_inserter(outputs));
                break;
            case 3:
                simplify_douglas_peucker_n<3>(begin, end, pointNum, std::back_inserter(outputs));
                break;
            default:
                return false;
        }
    }
    catch (std::exception& e)
    {
        printf("%s error occ occurred: %s.", __func__, e.what());
        return false;
    }
    return true;
}
