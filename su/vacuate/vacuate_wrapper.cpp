//
// Created by dly on 2023/1/16.
//
#include "vacuate/psimpl.h"

using namespace psimpl;

#include <iterator>
#include "vacuate/vacuate_wrapper.h"

bool vacuate_wrapper::VacuateNPoint(const int& n, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
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

bool vacuate_wrapper::VacuateRadialDistance(const double& radialDistance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
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

bool vacuate_wrapper::VacuatePerpendicularDistance(const int& repeat, const double& distance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
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

bool vacuate_wrapper::VacuateReumannWitkam(const double& distance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
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

bool vacuate_wrapper::VacuateOpheim(const double& minDistance, const double& maxDistance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
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

bool vacuate_wrapper::VacuateLang(const int& lookAhead, const double& distance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
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

bool vacuate_wrapper::VacuateDouglasPeucker(const double& distance, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
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

bool vacuate_wrapper::VacuateDouglasPeuckerVariant(const int& pointNum, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims)
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
