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
  public:
    /// <summary>
    /// 根据参数构建转换
    /// </summary>
    /// <param name="from">原始坐标系</param>
    /// <param name="to">目标坐标系</param>
    /// <returns></returns>
    bool begin(const eCrsEpsgCode &from, const eCrsEpsgCode &to);

    bool convert(const double &fromX, const double &fromY, double &toX, double &toY) const;
    bool convert(const std::vector<double> &fromX, const std::vector<double> &fromY, std::vector<double> &toX, std::vector<double> &toY) const;

    /// <summary>
    /// 关闭转换,释放转换对象
    /// </summary>
    /// <returns></returns>
    void close() const;

  private:
#if SU_THIRD_SUPPORT_GDAL
    OGRCoordinateTransformation *poCT{nullptr};
#endif
};
}  // namespace proj
}  // namespace geo
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PROJ_CONVERT_H
