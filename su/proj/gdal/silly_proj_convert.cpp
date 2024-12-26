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
#include <log/silly_log.h>
#if ENABLE_GDAL
static OGRSpatialReference spc_build_srs(const spc_srs_param& ssp)
{
    bool status = false;

    OGRSpatialReference res_srs;
    if (!(res_srs.importFromWkt(silly_projection_define::get(ssp.wk_num)) == OGRERR_NONE))
    {
        SLOG_ERROR("\n地理坐标系统: {} , 参数设置错误\n", static_cast<int>(ssp.wk_num))
    }

    return res_srs;
}
#endif
silly_proj_convert::~silly_proj_convert()
{
    close();
}
bool silly_proj_convert::begin(const silly_proj_param& p)
{
    bool status = false;
#if ENABLE_GDAL
    OGRSpatialReference src_srs = spc_build_srs(p.from);
    OGRSpatialReference dst_srs = spc_build_srs(p.to);
    m_poTransform = OGRCreateCoordinateTransformation(&src_srs, &dst_srs);
    if (status = (m_poTransform == nullptr))
    {
        SLOG_ERROR("\n地理坐标系统转换: {} -> {}, 构建错误错误\n", static_cast<int>(p.from.wk_num), static_cast<int>(p.to.wk_num))
    }
#endif
    return status;
}
bool silly_proj_convert::convert(const double& fromX, const double& fromY, double& toX, double& toY)
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

bool silly_proj_convert::convert(const std::vector<double>& fromX, const std::vector<double>& fromY, std::vector<double>& toX, std::vector<double>& toY)
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
bool silly_proj_convert::close()
{
#if ENABLE_GDAL
    if (m_poTransform)
    {
        OCTDestroyCoordinateTransformation(m_poTransform);
    }
#endif

    return true;
}
