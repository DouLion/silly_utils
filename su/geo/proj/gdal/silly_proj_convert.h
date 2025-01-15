/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/18 9:38
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_PROJ_CONVERT_H
#define SILLY_UTILS_SILLY_PROJ_CONVERT_H
#include <geo/proj/gdal/silly_projection_define.h>
#if ENABLE_GDAL
#include <gdal_priv.h>
#include <gdal_alg.h>
#include <ogr_spatialref.h>
#include <cpl_conv.h>
#endif

struct spc_srs_param
{
    silly_proj_def_enum wk_num;
    double central{114.0};
};

struct silly_proj_param
{
    spc_srs_param from;
    spc_srs_param to;
};
class silly_proj_convert
{
    friend class silly_projection_define;

  public:
    silly_proj_convert() = default;
    ~silly_proj_convert();
    /// <summary>
    /// 根据参数构建转换
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    bool begin(const silly_proj_param &p);

    bool convert(const double &fromX, const double &fromY, double &toX, double &toY);
    bool convert(const std::vector<double> &fromX, const std::vector<double> &fromY, std::vector<double> &toX, std::vector<double> &toY);

    /// <summary>
    /// 关闭转换,释放转换对象
    /// </summary>
    /// <returns></returns>
    bool close();

  private:
#if ENABLE_GDAL
    OGRCoordinateTransformation *m_poTransform{nullptr};
#endif
};

#endif  // SILLY_UTILS_SILLY_PROJ_CONVERT_H
