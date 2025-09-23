/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/18 9:38
 * @version: 1.0.1
 * @description:
 */
#include "silly_proj_convert.h"
using namespace silly::geo::proj;

bool gdal_convert::begin(const eCrsEpsgCode& from, const eCrsEpsgCode& to)
{
    bool status = true;
#if SU_THIRD_SUPPORT_GDAL
    const OGRSpatialReference srcRef = CRS::reference(from);
    const OGRSpatialReference dstRef = CRS::reference(to);
    poCT = OGRCreateCoordinateTransformation(&srcRef, &dstRef);
    if (poCT == nullptr)
    {
        SLOG_ERROR("\n地理坐标系统转换: {} -> {}, 构建错误错误\n", static_cast<int>(from), static_cast<int>(to))
        status = false;
    }
#endif
    return status;
}
bool gdal_convert::convert(const double& fromX, const double& fromY, double& toX, double& toY) const
{
    bool status = false;
#if SU_THIRD_SUPPORT_GDAL
    if (poCT)
    {
        double tmpX = fromX, tmpY = fromY;
        if (poCT->Transform(1, &tmpX, &tmpY))
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

bool gdal_convert::convert(const std::vector<double>& fromX, const std::vector<double>& fromY, std::vector<double>& toX, std::vector<double>& toY) const
{
    bool status = false;
#if SU_THIRD_SUPPORT_GDAL
    if (poCT)
    {
        std::vector<double> tmpX = fromX;
        std::vector<double> tmpY = fromY;
        int num = SU_MIN(tmpX.size(), fromY.size());
        if ( poCT->Transform(num, tmpX.data(), tmpY.data()))
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
void gdal_convert::close() const
{
#if SU_THIRD_SUPPORT_GDAL
    if (poCT)
    {
        OGRCoordinateTransformation::DestroyCT(poCT);
    }
#endif
}
