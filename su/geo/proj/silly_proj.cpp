/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/7 13:01
 * @version: 1.0.1
 * @description:
 */
//
// Created by dell on 2024/6/7. 实现
//

#pragma once
#include "silly_proj.h"
#if SU_THIRD_SUPPORT_EIGEN3
#include <Eigen/Dense>
#endif

void suGeoProj::gauss_to_lonlat(const double& gx, const double& gy, double& lon, double& lat, const suGeoProj::param& p)
{
    double centralMeridian = p.central;

    // 辅助项
    double n = (p.major_axis - p.major_axis * (1 - 1 / p.flatten)) / (p.major_axis + p.major_axis * (1 - 1 / p.flatten));
    double semiMinorAxis = p.major_axis * (1 - 1 / p.flatten);
    double firstEccentricitySquared = (p.major_axis * p.major_axis - semiMinorAxis * semiMinorAxis) / (p.major_axis * p.major_axis);
    double secondEccentricitySquared = (p.major_axis * p.major_axis - semiMinorAxis * semiMinorAxis) / (semiMinorAxis * semiMinorAxis);

    // 计算Y坐标对应的纬度
    double mu = (gy - 10000000.0) / (p.scale * semiMinorAxis);
    double phi = mu + (3 * n / 2 - 27 * n * n * n / 32) * sin(2 * mu) + (21 * n * n / 16 - 55 * n * n * n / 32) * sin(4 * mu) + (151 * n * n * n / 96) * sin(6 * mu) + (1097 * n * n * n * n / 512) * sin(8 * mu);

    // 计算X坐标对应的经度
    double lambda = gx - p.easting;
    double lambdaPrime = lambda / (p.major_axis * cos(phi));

    // 迭代计算纬度
    double phiPrime = phi;
    double phiNew = phi + (phiPrime - phi - lambdaPrime * cos(phi)) / (1 - firstEccentricitySquared * sin(phi) * sin(phi) - lambdaPrime * lambdaPrime * cos(phi) * cos(phi));

    while (fabs(phiNew - phiPrime) > 1e-10)
    {
        phiPrime = phiNew;
        phiNew = phi + (phiPrime - phi - lambdaPrime * cos(phiPrime)) / (1 - firstEccentricitySquared * sin(phiPrime) * sin(phiPrime) - lambdaPrime * lambdaPrime * cos(phiPrime) * cos(phiPrime));
    }

    phi = phiNew;

    // 转换为经纬度
    lat = RAD2DEG(phi);
    lon = RAD2DEG((centralMeridian + lambdaPrime * 180.0 / (MATH::PI_ * p.major_axis * cos(phi))));
}

void suGeoProj::gauss_to_lonlat(const double& central, const double& gx, const double& gy, double& lon, double& lat)
{
    double Y = gx, X = gy;

    Y -= 500000;
    double e1 = (1 - sqrt(1 - WGS84::E2)) / (1 + sqrt(1 - WGS84::E2));
    double M = X;
    double mu = M / (WGS84::A * (1 - WGS84::E2 / 4.0 - 3 * WGS84::E2 * WGS84::E2 / 64.0 - 5 * WGS84::E2 * WGS84::E2 * WGS84::E2 / 256.0));
    double phi1 = mu + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * sin(2 * mu) + (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * sin(4 * mu) + (151 * e1 * e1 * e1 / 96) * sin(6 * mu) + (1097 * e1 * e1 * e1 * e1 / 512) * sin(8 * mu);

    double C1 = WGS84::E2 * cos(phi1) * cos(phi1) / (1 - WGS84::E2);
    double T1 = tan(phi1) * tan(phi1);
    double N1 = WGS84::A / sqrt(1 - WGS84::E2 * sin(phi1) * sin(phi1));
    double R1 = WGS84::A * (1 - WGS84::E2) / pow(1 - WGS84::E2 * sin(phi1) * sin(phi1), 1.5);
    double D = Y / N1;

    // 经纬度计算
    double phi = phi1 - (N1 * tan(phi1) / R1) * (D * D / 2 - (5 + 3 * T1 + 10 * C1 - 4 * C1 * C1 - 9 * e1) * D * D * D * D / 24 + (61 + 90 * T1 + 298 * C1 + 45 * T1 * T1 - 252 * e1 - 3 * C1 * C1) * D * D * D * D * D * D / 720);

    double lambda = DEG2RAD(central) + (D - (1 + 2 * T1 + C1) * D * D * D / 6 + (5 - 2 * C1 + 28 * T1 - 3 * C1 * C1 + 8 * e1 + 24 * T1 * T1) * D * D * D * D * D / 120) / cos(phi1);

    // 转换为度
    lat = RAD2DEG(phi);
    lon = RAD2DEG(lambda);
}

void suGeoProj::lonlat_to_gauss(const double& lon, const double& lat, double& gx, double& gy, const suGeoProj::param& p)
{
    // 中央经线
    double L0 = p.central;

    // 将经纬度转换为弧度
    double lon_rad = DEG2RAD(lon);
    double lat_rad = DEG2RAD(lat);

    // 计算经差
    double lambda = (lon_rad - DEG2RAD(L0));

    // 辅助项
    double n = (p.major_axis - p.major_axis * (1 - 1 / p.flatten)) / (p.major_axis + p.major_axis * (1 - 1 / p.flatten));
    double semiMinorAxis = p.major_axis * (1 - 1 / p.flatten);
    double firstEccentricitySquared = (p.major_axis * p.major_axis - semiMinorAxis * semiMinorAxis) / (p.major_axis * p.major_axis);
    double secondEccentricitySquared = (p.major_axis * p.major_axis - semiMinorAxis * semiMinorAxis) / (semiMinorAxis * semiMinorAxis);

    // 计算N和T
    double radiusOfCurvature = p.major_axis / sqrt(1 - firstEccentricitySquared * sin(lat_rad) * sin(lat_rad));
    double tangentSquared = tan(lat_rad) * tan(lat_rad);
    double meridianCurvature = secondEccentricitySquared * cos(lat_rad) * cos(lat_rad);

    // 计算A和B
    double A = lambda * cos(lat_rad);
    double B = lambda * lambda * cos(lat_rad) * cos(lat_rad) / 2;

    // X坐标
    gx = radiusOfCurvature * (lambda + (1 - tangentSquared + meridianCurvature) * pow(A, 3) / 6 +
                              (5 - 18 * tangentSquared + tangentSquared * tangentSquared + 72 * meridianCurvature - 58 * (1 - firstEccentricitySquared) / (1 + firstEccentricitySquared)) * pow(A, 5) / 120) +
         p.easting;

    // Y坐标
    gy = p.scale * (semiMinorAxis * (lat_rad - (1 - n + (5 * n * n) / 4 - (5 * n * n * n) / 4) * sin(2 * lat_rad) / 2 + (1 - (3 * n) / 2 + (21 * n * n) / 16 - (55 * n * n * n) / 32) * sin(4 * lat_rad) / 24 -
                                     (1 - (11 * n) / 4 + (413 * n * n) / 96 - (1231 * n * n * n) / 256) * sin(6 * lat_rad) / 720 + (1 - (15 * n) / 4 + (517 * n * n) / 64 - (5147 * n * n * n) / 256) * sin(8 * lat_rad) / 40320)) +
         10000000.0;
}

void suGeoProj::lonlat_to_gauss(const double& central, const double& lon, const double& lat, double& gx, double& gy)
{
    double lamb = DEG2RAD(lat);
    double phi = DEG2RAD(lon);

    // 将中央子午线转换为弧度
    double centralMeridianRad = DEG2RAD(central);

    // 计算高斯-克吕格投影公式中的参数
    double N = WGS84::A / sqrt(1 - WGS84::E2 * sin(lamb) * sin(lamb));
    double T = tan(lamb) * tan(lamb);
    double C = WGS84::E2 * cos(lamb) * cos(lamb) / (1 - WGS84::E2);
    double A = (phi - centralMeridianRad) * cos(lamb);

    double M = WGS84::A * ((1 - WGS84::E2 / 4.0 - 3.0 * WGS84::E2 * WGS84::E2 / 64.0 - 5.0 * WGS84::E2 * WGS84::E2 * WGS84::E2 / 256.0) * lamb -
                           (3.0 * WGS84::E2 / 8.0 + 3.0 * WGS84::E2 * WGS84::E2 / 32.0 + 45.0 * WGS84::E2 * WGS84::E2 * WGS84::E2 / 1024.0) * sin(2.0 * lamb) +
                           (15.0 * WGS84::E2 * WGS84::E2 / 256.0 + 45.0 * WGS84::E2 * WGS84::E2 * WGS84::E2 / 1024.0) * sin(4.0 * lamb) - (35.0 * WGS84::E2 * WGS84::E2 * WGS84::E2 / 3072.0) * sin(6.0 * lamb));

    // 计算 X, Y 坐标
    gy = M + N * tan(lamb) * (A * A / 2.0 + (5.0 - T + 9.0 * C + 4.0 * C * C) * A * A * A * A / 24.0 + (61.0 - 58.0 * T + T * T + 600.0 * C - 330.0 * WGS84::E2) * A * A * A * A * A * A / 720.0);
    gx = N * (A + (1.0 - T + C) * A * A * A / 6.0 + (5.0 - 18.0 * T + T * T + 72.0 * C - 58.0 * WGS84::E2) * A * A * A * A * A / 120.0) + 500000.0;  // 中央子午线偏移+500000.0
}

void suGeoProj::mercator_to_lonlat(const double& mctx, const double& mcty, double& lon, double& lat)
{
    lon = RAD2DEG(mctx / WGS84::A);
    lat = RAD2DEG(2 * atan(exp(mcty / WGS84::A)) - MATH::PI_ / 2);
}

void suGeoProj::lonlat_to_mercator(const double& lon, const double& lat, double& mctx, double& mcty)
{
    double lamb = DEG2RAD(lat);
    double phi = DEG2RAD(lon);

    // 墨卡托投影公式
    mctx = WGS84::A * phi;
    mcty = WGS84::A * log(tan(MATH::PI_ / 4 + lamb / 2));
}

void suGeoProj::mercator_to_gauss(const double& mctx, const double& mcty, double& gx, double& gy, const suGeoProj::param& p)
{
    double lon, lat;
    mercator_to_lonlat(mctx, mcty, lon, lat);
    lonlat_to_gauss(lon, lat, gx, gy, p);
}

void suGeoProj::gauss_to_mercator(const double& gx, const double& gy, double& mctx, double& mcty, const suGeoProj::param& p)
{
    double lon, lat;
    gauss_to_lonlat(gx, gy, lon, lat, p);
    lonlat_to_mercator(lon, lat, mctx, mcty);
}

void suGeoProj::mercator_to_gauss(const double& central, const double& mctx, const double& mcty, double& gx, double& gy)
{
    double lon, lat;
    mercator_to_lonlat(mctx, mcty, lon, lat);
    lonlat_to_gauss(central, lon, lat, gx, gy);
}

void suGeoProj::gauss_to_mercator(const double& central, const double& gx, const double& gy, double& mctx, double& mcty)
{
    double lon, lat;
    gauss_to_lonlat(central, gx, gy, lon, lat);
    lonlat_to_mercator(lon, lat, mctx, mcty);
}

void suGeoProj::ecef_to_lonlat(const double& x, const double& y, const double& z, double& lon, double& lat, double& height)
{
    // 计算经度
    lon = std::atan2(y, x);

    // 计算初始纬度估计
    double p = std::sqrt(x * x + y * y);
    double theta = std::atan2(z, p * (1 - WGS84::F));
    lat = std::atan2(z + WGS84::E2 * WGS84::B * std::pow(std::sin(theta), 3), p - WGS84::E2 * WGS84::B * std::pow(std::cos(theta), 3));

    // 迭代计算纬度，直到收敛
    double previousLatitude;
    do
    {
        previousLatitude = lat;
        double N = WGS84::A / std::sqrt(1 - WGS84::E2 * std::sin(lat) * std::sin(lat));
        height = p / std::cos(lat) - N;
        lat = std::atan2(z + WGS84::E2 * N * sin(lat), p);
    } while (std::fabs(lat - previousLatitude) > 1e-12);  // 收敛条件

    // 将纬度和经度转换为度
    lat = RAD2DEG(lat);
    lon = RAD2DEG(lon);
}

void suGeoProj::lonlat_to_ecef(const double& lon, const double& lat, const double& height, double& x, double& y, double& z)
{
    double lamb = DEG2RAD(lat);
    double phi = DEG2RAD(lon);

    // 计算N（曲率半径）
    double N = WGS84::A / std::sqrt(1 - WGS84::E2 * std::sin(lamb) * std::sin(lamb));

    // 计算XYZ坐标
    x = (N + height) * std::cos(lamb) * std::cos(phi);
    y = (N + height) * std::cos(lamb) * std::sin(phi);
    z = (WGS84::B * WGS84::B / (WGS84::A * WGS84::A) * N + height) * std::sin(lamb);
}

suGeoProj::pfour suGeoProj::build(const std::vector<suGeoProj::point2d>& measures, const std::vector<suGeoProj::point2d>& origins)
{
    pfour result;
#if SU_THIRD_SUPPORT_EIGEN3
    if (measures.size() < 2 || origins.size() < 2 || measures.size() != origins.size())
    {
        throw std::invalid_argument("至少需要 2 对匹配的二维点，且数量一致");
    }

    const size_t n = measures.size();

    // 构造超定线性方程组：A * [dx, dy, s*cos(r), s*sin(r)]^T = L
    // 为了求解：dx, dy, s*cos(r), s*sin(r)
    Eigen::MatrixXd A(2 * n, 4);
    Eigen::VectorXd L(2 * n);

    for (size_t i = 0; i < n; ++i)
    {
        const auto& p = measures[i];  // 源点 (x, y)
        const auto& q = origins[i];   // 目标点 (xp, yp)

        const double x = p.x;
        const double y = p.y;
        const double xp = q.x;
        const double yp = q.y;

        const int row = 2 * i;

        // 第1行：x'
        A(row, 0) = 1.0;  // dx
        A(row, 1) = 0.0;  // dy
        A(row, 2) = x;    // s*cos(r) * x
        A(row, 3) = y;    // s*sin(r) * x

        L(row) = xp;

        // 第2行：y'
        A(row + 1, 0) = 0.0;  // dx
        A(row + 1, 1) = 1.0;  // dy
        A(row + 1, 2) = -y;   // s*cos(r) * (-y)
        A(row + 1, 3) = x;    // s*sin(r) * y

        L(row + 1) = yp;
    }

    // 最小二乘求解：[dx, dy, s*cos(r), s*sin(r)]^T = (A^T A)^-1 A^T L
    Eigen::VectorXd params = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(L);

    const double dx_est = params(0);
    const double dy_est = params(1);
    const double scos_r = params(2);
    const double ssin_r = params(3);

    // 计算旋转角度 r（弧度）和缩放 s
    const double r_est = std::atan2(ssin_r, scos_r);                    // r = atan2(sin, cos)
    const double s_est = std::sqrt(scos_r * scos_r + ssin_r * ssin_r);  // s = sqrt(cos² + sin²)

    // 构造并返回 4 参数

    result.dx = dx_est;
    result.dy = dy_est;
    result.r = r_est;
    result.s = s_est;
#else
    throw std::runtime_error("需要支持EIGEN3");
#endif

    return result;
}

suGeoProj::point2d suGeoProj::trans(const suGeoProj::point2d& p, const suGeoProj::pfour& p4)
{
    const double x = p.x;
    const double y = p.y;

    const double dx = p4.dx;
    const double dy = p4.dy;
    const double r = p4.r;
    const double s = p4.s;

    // 二维相似变换公式：
    const double x_new = s * (std::cos(r) * x - std::sin(r) * y) + dx;
    const double y_new = s * (std::sin(r) * x + std::cos(r) * y) + dy;

    return point2d{x_new, y_new};
}

suGeoProj::helmert suGeoProj::build(const std::vector<suGeoProj::point3d>& measures, const std::vector<suGeoProj::point3d>& origins)
{
    // 构造返回的 helmert 结构体
    helmert result;
#if SU_THIRD_SUPPORT_EIGEN3
    // 至少需要3对点，推荐更多
    if (measures.size() < 3 || origins.size() < 3 || measures.size() != origins.size())
    {
        throw std::invalid_argument("measures 和 origins 必须包含至少 3 对匹配点，且数量相同");
    }
    const size_t n = measures.size();  // 点对数
    Eigen::MatrixXd A(3 * n, 7);       // 3n 行（每个点3个方程），7列（7参数）
    Eigen::VectorXd L(3 * n);          // 残差向量

    for (size_t i = 0; i < n; ++i)
    {
        // 当前点对
        const auto& m = measures[i];  // 测量点 (X, Y, Z)
        const auto& o = origins[i];   // 参考点 (X', Y', Z')

        const double X = m.x;
        const double Y = m.y;
        const double Z = m.z;

        const double Xp = o.x;
        const double Yp = o.y;
        const double Zp = o.z;

        const int row = 3 * i;

        // 第1行：X' 方向
        A(row, 0) = 1.0;  // dx
        A(row, 1) = 0.0;
        A(row, 2) = 0.0;
        A(row, 3) = Z;   // R_y * Z （小角度模型中的旋转项）
        A(row, 4) = -Y;  // -R_x * Y
        A(row, 5) = 0.0;
        A(row, 6) = X;  // s * X

        L(row) = Xp - X;

        // 第2行：Y' 方向
        A(row + 1, 0) = 0.0;
        A(row + 1, 1) = 1.0;
        A(row + 1, 2) = 0.0;
        A(row + 1, 3) = -Z;  // -R_z * Y
        A(row + 1, 4) = 0.0;
        A(row + 1, 5) = X;  // R_x * X
        A(row + 1, 6) = Y;  // s * Y

        L(row + 1) = Yp - Y;

        // 第3行：Z' 方向
        A(row + 2, 0) = 0.0;
        A(row + 2, 1) = 0.0;
        A(row + 2, 2) = 1.0;
        A(row + 2, 3) = 0.0;
        A(row + 2, 4) = Z;   // R_z * X
        A(row + 2, 5) = -X;  // -R_y * X
        A(row + 2, 6) = Z;   // s * Z

        L(row + 2) = Zp - Z;
    }

    // 最小二乘求解：参数 = (A^T A)^{-1} A^T L
    Eigen::VectorXd params = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(L);

    result.dx = params(0);
    result.dy = params(1);
    result.dz = params(2);
    result.rx = params(3);  // 弧度
    result.ry = params(4);  // 弧度
    result.rz = params(5);  // 弧度
    result.s = params(6);   // 通常是 ppm，可直接使用，或 *1e6 转为 ppm 数值
#else
    throw std::runtime_error("需要支持EIGEN3");
#endif
    return result;
}

suGeoProj::point3d suGeoProj::trans(const suGeoProj::point3d& measures, const suGeoProj::helmert& p7)
{
    const double& X = measures.x;
    const double& Y = measures.y;
    const double& Z = measures.z;

    const double& dx = p7.dx;
    const double& dy = p7.dy;
    const double& dz = p7.dz;
    const double& rx = p7.rx;
    const double& ry = p7.ry;
    const double& rz = p7.rz;
    const double& s = p7.s;

    // 七参数公式（小旋转，线性近似）：
    const double scale = 1.0 + s * 1e-6;  // 缩放因子

    // 旋转项（小角度，线性模型）
    const double X_rot = (X - rz * Y + ry * Z) * scale + dx;
    const double Y_rot = (Y + rz * X - rx * Z) * scale + dy;
    const double Z_rot = (Z - ry * X + rx * Y) * scale + dz;

    // 返回转换后的点
    return point3d{X_rot, Y_rot, Z_rot};
}

void suGeoProj::cartesian_to_polar(double x, double y, double& r, double& th, double x0, double y0)
{
    const double dx = x - x0;  // 计算原点偏移后的坐标差
    const double dy = y - y0;

    r = std::hypot(dx, dy);                    // 等效于 sqrt(dx*dx + dy*dy)，精度更高
    th = (r == 0) ? 0.0 : std::atan2(dy, dx);  // 处理零半径情况
}

void suGeoProj::polar_to_cartesian(double r, double th, double& x, double& y, double x0, double y0)
{
    x = x0 + r * std::cos(th);  // 极径投影到x轴
    y = y0 + r * std::sin(th);  // 极径投影到y轴
}