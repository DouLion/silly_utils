/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/18 9:38
 * @version: 1.0.1
 * @description:
 */
//
// Created by dell on 2024/6/18. 实现
//

#include "silly_proj_convert.h"
using namespace silly::geo::proj;
gdal_convert::~gdal_convert()
{
    close();
}
bool gdal_convert::begin(const CRS::type& from, const CRS::type& to)
{
    bool status = false;
#if ENABLE_GDAL
    const OGRSpatialReference src_srs = CRS::reference(from);
    const OGRSpatialReference dst_srs = CRS::reference(to);
    m_poTransform = OGRCreateCoordinateTransformation(&src_srs, &dst_srs);
    if (status = (m_poTransform == nullptr))
    {
        SLOG_ERROR("\n地理坐标系统转换: {} -> {}, 构建错误错误\n", static_cast<int>(from), static_cast<int>(to))
    }
#endif
    return status;
}
bool gdal_convert::convert(const double& fromX, const double& fromY, double& toX, double& toY)
{
    bool status = false;
#if ENABLE_GDAL
    if (m_poTransform)
    {
        double tmpX = fromX, tmpY = fromY;
        if (m_poTransform->Transform(1, &tmpX, &tmpY))
        {
            toX = tmpX;
            toY = tmpY;
            status = true;
        }
        else
        {
            SLOG_ERROR("坐标转换失败")
        }
    }
#endif
    return status;
}

bool gdal_convert::convert(const std::vector<double>& fromX, const std::vector<double>& fromY, std::vector<double>& toX, std::vector<double>& toY)
{
    bool status = false;
#if ENABLE_GDAL
    if (m_poTransform)
    {
        std::vector<double> tmpX = fromX;
        std::vector<double> tmpY = fromY;
        int num = SU_MIN(tmpX.size(), fromY.size());
        if (status = m_poTransform->Transform(num, &tmpX[0], &tmpY[0]))
        {
            /*toX.resize(num);
            toY.resize(num);
            memcpy()*/
            toX = tmpX;
            toY = tmpY;
            status = true;
        }
        else
        {
            SLOG_ERROR("坐标转换失败")
        }
    }
#endif
    return status;
}
bool gdal_convert::close()
{
#if ENABLE_GDAL
    if (m_poTransform)
    {
        OCTDestroyCoordinateTransformation(m_poTransform);
    }
#endif

    return true;
}
