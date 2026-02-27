/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-14
 * @file: silly_dem.c
 * @description: silly_dem实现
 * @version: v1.0.1 2025-08-14 dou li yang
 */
#include "silly_dem.h"

#include "geo/silly_geo_utils.h"
#define CVT_CHECK_G2LL_BOUND(l0, x0, y0, x1, y1, rect0) \
    suGeoProj::gauss_to_lonlat(l0, x0, y0, x1, y1);     \
    rect0.min.x = std::min(x1, rect0.min.x);            \
    rect0.min.y = std::min(y1, rect0.min.y);            \
    rect0.max.x = std::max(x1, rect0.max.x);            \
    rect0.max.y = std::max(y1, rect0.max.y);

#define CVT_CHECK_LL2G_BOUND(l0, x0, y0, x1, y1, rect0) \
    suGeoProj::lonlat_to_gauss(l0, x0, y0, x1, y1);     \
    rect0.min.x = std::min(x1, rect0.min.x);            \
    rect0.min.y = std::min(y1, rect0.min.y);            \
    rect0.max.x = std::max(x1, rect0.max.x);            \
    rect0.max.y = std::max(y1, rect0.max.y);

bool suDem::IsGauss() const
{
    static std::set<int> valid = {75, 78, 81, 84, 87, 90, 93, 96, 99, 102, 105, 108, 111, 114, 117, 120, 123, 126, 129, 132, 135};
    return valid.find(static_cast<int>(info.central)) != std::end(valid);
}

void suDem::Gauss2Mercator(const suDem& rh)
{
    if (this == &rh)
    {
        throw std::runtime_error("对象地址相同,无法转换");
    }
    if (rh.info.central < 75 || rh.info.central > 135)
    {
        throw std::runtime_error("无效的高斯中央经线");
        return;
    }

    info = rh.info;
    suPoint lt, rb, lb, rt;
    GAUSS2LONLAT(rh.info.central, rh.info.bound.min.x, rh.info.bound.max.y, lt.x, lt.y);
    GAUSS2LONLAT(rh.info.central, rh.info.bound.max.x, rh.info.bound.min.y, rb.x, rb.y);
    GAUSS2LONLAT(rh.info.central, rh.info.bound.min.x, rh.info.bound.min.y, lb.x, lb.y);
    GAUSS2LONLAT(rh.info.central, rh.info.bound.max.x, rh.info.bound.max.y, rt.x, rt.y);
    suRect llbd;
    llbd.min.x = std::max(lt.x, lb.x);
    llbd.max.x = std::min(rb.x, rt.x);
    llbd.max.y = std::min(rt.y, lt.y);
    llbd.min.y = std::max(lb.y, rb.y);

    LONLAT2MERCATOR(llbd.min.x, llbd.max.y, info.bound.min.x, info.bound.max.y);
    LONLAT2MERCATOR(llbd.max.x, llbd.min.y, info.bound.max.x, info.bound.min.y);
    double distx = info.bound.max.x - info.bound.min.x;
    double disty = info.bound.max.y - info.bound.min.y;

     if (distx < disty)
    {
        info.dx = distx / rh.info.width;
        info.dy = info.dx;
        info.height = std::floor(disty / info.dy);
    }
    else
    {
        info.dy = disty / rh.info.height;
        info.dx = info.dy;
        info.width = std::floor(distx / info.dx);
    }

    raster.create(info.height, info.width, true);
    raster.set(info.fill);
    float * thisp = raster.data();
    float * rhp = rh.raster.data();

    int maxrh = rh.info.height * rh.info.width;
    int maxthis = info.height * info.width;

    for (size_t r = 0; r < info.height; ++r)
    {
        for (size_t c = 0; c < info.width; ++c)
        {
            suPoint mp, gp;
            mp.x = info.bound.min.x + c * info.dx;
            mp.y = info.bound.max.y - r * info.dy;
            MERCATOR2GAUSS(rh.info.central, mp.x, mp.y, gp.x, gp.y);

            int gcol = static_cast<size_t>(std::round((gp.x - rh.info.bound.min.x) / rh.info.dx));
            int grol = static_cast<size_t>(std::round((rh.info.bound.max.y - gp.y) / rh.info.dy));
            if (gcol >= 0 && grol >= 0 && gcol < rh.info.width && grol < rh.info.height)
            {
                size_t gpos = grol * rh.info.width + gcol;
                size_t pos = r * info.width + c;
                thisp[pos] = rhp[gpos];
            }
        }
    }
}

void suDem::Gauss2LonLat(const suDem& rh)
{
    if (this == &rh)
    {
        throw std::runtime_error("对象地址相同,无法转换");
    }
    if (rh.info.central < 75 || rh.info.central > 135)
    {
        throw std::runtime_error("无效的高斯中央经线");
        return;
    }

    info = rh.info;
    suPoint lt, rb, lb, rt;
    GAUSS2LONLAT(rh.info.central, rh.info.bound.min.x, rh.info.bound.max.y, lt.x, lt.y);
    GAUSS2LONLAT(rh.info.central, rh.info.bound.max.x, rh.info.bound.min.y, rb.x, rb.y);
    GAUSS2LONLAT(rh.info.central, rh.info.bound.min.x, rh.info.bound.min.y, lb.x, lb.y);
    GAUSS2LONLAT(rh.info.central, rh.info.bound.max.x, rh.info.bound.max.y, rt.x, rt.y);
    
    info.bound.min.x = std::max(lt.x, lb.x);
    info.bound.max.x = std::min(rb.x, rt.x);
    info.bound.max.y = std::min(rt.y, lt.y);
    info.bound.min.y = std::max(lb.y, rb.y);

    double distx = info.bound.max.x - info.bound.min.x;
    double disty = info.bound.max.y - info.bound.min.y;

    if (distx < disty)
    {
        info.dx = distx / rh.info.width;
        info.dy = info.dx;
        info.height = std::floor(disty / info.dy);
    }
    else
    {
        info.dy = disty / rh.info.height;
        info.dx = info.dy;
        info.width = std::floor(distx / info.dx);
    }

    raster.create(info.height, info.width, true);
    raster.set(info.fill);
    float* thisp = raster.data();
    float* rhp = rh.raster.data();

    int maxrh = rh.info.height * rh.info.width;
    int maxthis = info.height * info.width;

    for (size_t r = 0; r < info.height; ++r)
    {
        for (size_t c = 0; c < info.width; ++c)
        {
            suPoint mp, llp;
            mp.x = info.bound.min.x + c * info.dx;
            mp.y = info.bound.max.y - r * info.dy;
            LONLAT2GAUSS(rh.info.central, mp.x, mp.y, llp.x, llp.y);

            int gcol = static_cast<size_t>(std::round((llp.x - rh.info.bound.min.x) / rh.info.dx));
            int grol = static_cast<size_t>(std::round((rh.info.bound.max.y - llp.y) / rh.info.dy));
            if (gcol >= 0 && grol >= 0 && gcol < rh.info.width && grol < rh.info.height)
            {
                size_t gpos = grol * rh.info.width + gcol;
                size_t pos = r * info.width + c;
                thisp[pos] = rhp[gpos];
            }
        }
    }
}

void suDem::LonLat2Gauss(const suDem& rh, const double& cell_size, const double& l0)
{
    if (this == &rh)
    {
        throw std::runtime_error("对象地址相同,无法转换");
    }
    if (this == &rh)
    {
        throw std::runtime_error("对象地址相同,无法转换");
    }
    // TODO: 需要先检查 是否为高斯
    info.bound.min.x = 1.e10;
    info.bound.max.x = -1.e10;
    info.bound.min.y = 1.e10;
    info.bound.max.y = -1.e10;
    double gx, gy;
    CVT_CHECK_LL2G_BOUND(l0, rh.info.bound.min.x, rh.info.bound.max.y, gx, gy, info.bound)
    CVT_CHECK_LL2G_BOUND(l0, rh.info.bound.min.x, rh.info.bound.min.y, gx, gy, info.bound)
    CVT_CHECK_LL2G_BOUND(l0, rh.info.bound.max.x, rh.info.bound.max.y, gx, gy, info.bound)
    CVT_CHECK_LL2G_BOUND(l0, rh.info.bound.max.x, rh.info.bound.min.y, gx, gy, info.bound)
    info.height = std::round((info.bound.max.y - info.bound.min.y) / cell_size);
    info.width = std::round(((info.bound.max.x - info.bound.min.x) / cell_size));
    info.dx = cell_size;
    info.dy = cell_size;
    info.central = l0;
    raster.create(info.height, info.width, true);
    raster.set(rh.info.fill);
    float* dp = raster.data();
    for (int64_t r = 0; r < info.height; r++)
    {
        for (int64_t c = 0; c < info.width; c++)
        {
            double lon, lat;
            gx = info.bound.min.x + c * info.dx;
            gy = info.bound.max.y - r * info.dy;
            GAUSS2LONLAT(info.central, gx, gy, lon, lat);
            int64_t llC = std::round((lon - rh.info.bound.min.x) / rh.info.dx);
            int64_t llR = std::round((rh.info.bound.max.y - lat) / rh.info.dy);
            if (llR >= 0 && llR < rh.raster.rows() && llC >= 0 && llC < rh.raster.cols())
            {
                const int64_t llI = llR * rh.raster.cols() + llC;
                *dp = rh.raster.data()[llI];
            }
            dp++;
        }
    }
}

void suDem::Cover(const suDem& rh)
{
    int xOff = 0, yOff = 0;

    for (int r = 0; r < rh.raster.rows(); r++)
    {
        for (int c = 0; c < rh.raster.cols(); c++)
        {
            int ty = r + yOff;
            int tx = c + xOff;
            if (ty >= 0 && ty < raster.rows() && tx >= 0 && tx < raster.cols())
            {
                raster[ty][tx] = rh.raster[r][c];
            }
        }
    }
}

void suDem::Extra(const suRing& ring)
{
    for (int r = 0; r < info.height; ++r)
    {
        for (int c = 0; c < info.width; ++c)
        {
            suPoint p;
            p.x = info.bound.min.x + c * info.dx;
            p.y = info.bound.max.y - r * info.dy;
            if (!ring.contains(p))  // 点在环外
            {
                raster[r][c] = info.fill;
            }
        }
    }
}
void suDem::Extra(const suDem& rh, const suRing& ring)
{
    if (this != &rh)
    {
        info = rh.info;
        raster.create(info.height, info.width, true);
    }

    Extra(ring);
}

suPoint suDem::ColRow(const suPoint& p) const
{
    suPoint ret;
    ret.x = (p.x - info.bound.min.x) / (info.bound.max.x - info.bound.min.x) * info.width;
    ret.y = (info.bound.max.y - p.y) / (info.bound.max.y - info.bound.min.y) * info.height;
    return ret;
}

double suDem::At(const suPoint& p) const
{
    double ret = NAN;

    auto cr = ColRow(p);
    if (cr.x > 0 && cr.x < info.width && cr.y > 0 && cr.y < info.height)
    {
        ret = raster.at(cr.y, cr.x);
    }
    return ret;
}

double suDem::Round(const suPoint& p) const
{
    double ret = NAN;

    auto cr = ColRow(p);
    if (cr.x > 0 && cr.x < info.width && cr.y > 0 && cr.y < info.height)
    {
        ret = raster.at(std::round(cr.y), std::round(cr.x));
    }
    return ret;
}
double suDem::BiLiner(const suPoint& p) const
{
    double ret = NAN;

    auto cr = ColRow(p);
    if (cr.x > 0 && cr.x < info.width && cr.y > 0 && cr.y < info.height)
    {
        ret = raster.bilinear(cr.y, cr.x);
    }
    return ret;
}
double suDem::BiCubic(const suPoint& p) const
{
    double ret = NAN;

    auto cr = ColRow(p);
    if (cr.x > 0 && cr.x < info.width && cr.y > 0 && cr.y < info.height)
    {
        ret = raster.bicubic(cr.y, cr.x);
    }
    return ret;
}
std::vector<std::pair<double, double>> suDem::ProfileElev(const suLine& line) const
{
    std::vector<std::pair<double, double>> ret;
    if (line.size() < 2)
    {
        return ret;
    }
    double dist = 0;
    suPoint p0 = line.front();
    for (const auto& p : line)
    {
        double elev = BiLiner(p);
        ret.push_back({dist, elev});
        dist += p.dist(p0);
        p0 = p;
    }
    return ret;
}
std::vector<std::pair<double, double>> suDem::ProfileElev(const suLine& line, const double& dist) const
{
    // line等间距划分
    std::vector<std::pair<double, double>> ret;
    if (line.size() < 2)
    {
        return ret;
    }
    // double dist = (info.dx + info.dy)/4;
    suLine eqLine = line.equidistant(dist);
    double xl = info.bound.min.x - dist * info.dx;
    double yl = info.bound.max.y - dist * info.dy;
    for (size_t i = 0; i < eqLine.size(); i++)
    {
        const auto& p = eqLine[i];
        double r = (info.bound.max.y - p.y) / yl;
        double c = (info.bound.max.x - p.x) / xl;
        double v = raster.bilinear(r, c);
        ret.push_back({i * dist, v});
    }
    return ret;
}
suFMatrix suDem::SlopeGradient(const int& method) const
{
    suFMatrix ret;
    if (!raster.data())
    {
        return ret;
    }
    // 创建一个与 DEM 尺寸相同的矩阵，用于存储坡度梯度值
    ret.create(info.height, info.width);  // 假设构造函数是 suFMatrix(rows, cols)

    auto func9 = [this](const int& r, const int& c) -> double {
        // 加权差分法（Horn 方法）
        double z1 = raster[r][c];
        double z2 = raster[r][c];
        double z3 = raster[r][c];
        double z4 = raster[r][c];
        double z5 = this->raster[r][c];
        double z6 = raster[r][c];
        double z7 = raster[r][c];
        double z8 = raster[r][c];
        double z9 = raster[r][c];
        double grad_x = ((z3 + 2 * z6 + z9) - (z1 + 2 * z4 + z7)) / 8 * info.dx;
        double grad_y = ((z7 + 2 * z8 + z9) - (z1 + 2 * z2 + z3)) / 8 * info.dy;
        double slope_gradient = std::sqrt(grad_x * grad_x + grad_y * grad_y);
        return slope_gradient;
    };

    auto func5 = [this](const int& r, const int& c) -> double {
        double z_center = raster[r][c] /* 获取 (r, c) 的高程，比如 this->getElevation(r, c) */;
        double z_left = raster[r][c - 1] /* 获取 (r, c - 1) 的高程 */;
        double z_right = raster[r][c + 1] /* 获取 (r, c + 1) 的高程 */;
        double z_up = raster[r - 1][c] /* 获取 (r - 1, c) 的高程 */;
        double z_down = raster[r + 1][c] /* 获取 (r + 1, c) 的高程 */;

        // 【推荐使用中心差分计算 x 和 y 方向的梯度】
        double grad_x = (z_right - z_left) / (2.0 * info.dx);
        double grad_y = (z_down - z_up) / (2.0 * info.dy);

        // 坡度梯度 = sqrt(grad_x^2 + grad_y^2)
        double slope_gradient = std::sqrt(grad_x * grad_x + grad_y * grad_y);
        return slope_gradient;
    };

    for (int r = 1; r < info.height - 1; ++r)
    {
        for (int c = 1; c < info.width - 1; ++c)
        {
            // 获取当前像元以及周围像元的高程值
            // 请根据你实际的高程数据来源进行修改，比如：
            double slope_gradient = 0;
            if (method == 5)
            {
                slope_gradient = func5(r, c);
            }
            else if (method == 9)
            {
                slope_gradient = func9(r, c);
            }

            // 将计算结果存入返回的矩阵对应位置
            ret[r][c] = slope_gradient;  // 假设支持 ret(r,c) = value，否则用一维指针方式
        }
    }
    // 边界处理：边界像元（r=0, r=height-1, c=0, c=width-1）可以不计算，或者用其他方法填充
    // 目前我们只计算了内部区域 (1 ~ height-2, 1 ~ width-2)

    return ret;
}
suFMatrix suDem::SlopeDegree(const int& method) const
{
    suFMatrix ret = SlopeGradient(method);
    float* ptr = ret.data();
    if (ptr)
    {
        for (int r = 1; r < info.height - 1 && ptr; r++)
        {
            for (int c = 1; c < info.width - 1 && ptr; c++)
            {
                *ptr = RAD2DEG(std::atan(*ptr));
                ptr++;
            }
        }
    }

    return ret;
}
suFMatrix suDem::SlopeRadian(const int& method) const
{
    suFMatrix ret = SlopeGradient(method);
    float* ptr = ret.data();
    if (ptr)
    {
        for (int r = 1; r < info.height - 1 && ptr; r++)
        {
            for (int c = 1; c < info.width - 1 && ptr; c++)
            {
                *ptr = std::atan(*ptr);
                ptr++;
            }
        }
    }

    return ret;
}
suFMatrix suDem::SlopePercent(const int& method) const
{
    suFMatrix ret = SlopeGradient(method);
    float* ptr = ret.data();
    if (ptr)
    {
        for (int r = 1; r < info.height - 1 && ptr; r++)
        {
            for (int c = 1; c < info.width - 1 && ptr; c++)
            {
                *ptr = *ptr * 100;
                ptr++;
            }
        }
    }

    return ret;
}
suUCMatrix suDem::HillShade(const double& azimuth_deg, const double& height_deg, const double& z_factor) const
{
    suUCMatrix shade;
    shade.create(info.height, info.width, true);

    const double zenith_deg = 90.0 - height_deg;
    const double zenithRad = DEG2RAD(zenith_deg);  // 转为弧度
    double tmpAzDeg = 360.0 - azimuth_deg + 90.0;
    if (tmpAzDeg >= 360.0)
    {
        tmpAzDeg = tmpAzDeg - 360.0;
    }
    double azimuth_rad = DEG2RAD(tmpAzDeg);

    double zenithRad_cos = cos(zenithRad);
    double zenithRad_sin = sin(zenithRad);
    for (int i = 1; i < info.height - 1; i++)
    {
        for (int j = 1; j < info.width - 1; j++)
        {
            double a = raster.at(i - 1, j - 1);  // e = 中心像素, other letters = neighbours
            double b = raster.at(i, j - 1);
            double c = raster.at(i + 1, j - 1);
            double d = raster.at(i - 1, j);
            double e = raster.at(i, j);  // It was too ugly not to put a letter e, even if it is unused
            double f = raster.at(i + 1, j);
            double g = raster.at(i - 1, j + 1);
            double h = raster.at(i, j + 1);
            double k = raster.at(i + 1, j + 1);  // Not i because of the for (int i = ...)

            double dz_dx = ((c + k + (2 * f)) - (a + g + (2 * d))) / (8 * info.dy);  // Derivatives
            double dz_dy = ((g + k + (2 * h)) - (a + c + (2 * b))) / (8 * info.dx);
            double slope_rad = atan(z_factor * sqrt((dz_dx * dz_dx) + (dz_dy * dz_dy)));
            double aspect_rad = atan2(dz_dy, -dz_dx);
            if (aspect_rad < 0)
            {
                aspect_rad += 2 * MATH::PI_;
            }

            double tmp_shade = 255.0 * (zenithRad_cos * cos(slope_rad)) + (zenithRad_sin * sin(slope_rad) * cos(azimuth_rad - aspect_rad));
            tmp_shade = std::min(255., std::max(0., tmp_shade));

            shade.at(i, j) = static_cast<unsigned char>(tmp_shade);
        }
    }
    return shade;
}

void suDem::Release()
{
    raster.release();
}