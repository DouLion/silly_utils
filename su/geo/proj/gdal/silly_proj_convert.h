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

namespace silly
{
namespace geo
{
namespace proj
{

class gdal_convert
{
    friend class silly_projection_define;

  public:
    gdal_convert();
    ~gdal_convert();
    /// <summary>
    /// 根据参数构建转换
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    bool begin(const CRS::type &from, const CRS::type &to);

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
}  // namespace proj
}  // namespace geo
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PROJ_CONVERT_H
