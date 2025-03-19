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
using namespace silly::geo::proj;

void convert::gauss_to_lonlat(const double& gx, const double& gy, double& lon, double& lat, const convert::param& p)
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
    lon = RAD2DEG((centralMeridian + lambdaPrime * 180.0 / (SU_PI * p.major_axis * cos(phi))));
}

void convert::gauss_to_lonlat(const double& central, const double& gx, const double& gy, double& lon, double& lat)
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

void convert::lonlat_to_gauss(const double& lon, const double& lat, double& gx, double& gy, const convert::param& p)
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

void convert::lonlat_to_gauss(const double& central, const double& lon, const double& lat, double& gx, double& gy)
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

void convert::mercator_to_lonlat(const double& mctx, const double& mcty, double& lon, double& lat)
{
    lon = RAD2DEG(mctx / WGS84::A);
    lat = RAD2DEG(2 * atan(exp(mcty / WGS84::A)) - SU_PI / 2);
}

void convert::lonlat_to_mercator(const double& lon, const double& lat, double& mctx, double& mcty)
{
    double lamb = DEG2RAD(lat);
    double phi = DEG2RAD(lon);

    // 墨卡托投影公式
    mctx = WGS84::A * phi;
    mcty = WGS84::A * log(tan(SU_PI / 4 + lamb / 2));
}

void convert::mercator_to_gauss(const double& mctx, const double& mcty, double& gx, double& gy, const convert::param& p)
{
    double lon, lat;
    mercator_to_lonlat(mctx, mcty, lon, lat);
    lonlat_to_gauss(lon, lat, gx, gy, p);
}

void convert::gauss_to_mercator(const double& gx, const double& gy, double& mctx, double& mcty, const convert::param& p)
{
    double lon, lat;
    gauss_to_lonlat(gx, gy, lon, lat, p);
    lonlat_to_mercator(lon, lat, mctx, mcty);
}

void convert::mercator_to_gauss(const double& central, const double& mctx, const double& mcty, double& gx, double& gy)
{
    double lon, lat;
    mercator_to_lonlat(mctx, mcty, lon, lat);
    lonlat_to_gauss(central, lon, lat, gx, gy);
}

void convert::gauss_to_mercator(const double& central, const double& gx, const double& gy, double& mctx, double& mcty)
{
    double lon, lat;
    gauss_to_lonlat(central, gx, gy, lon, lat);
    lonlat_to_mercator(lon, lat, mctx, mcty);
}

void convert::ecef_to_lonlat(const double& x, const double& y, const double& z, double& lon, double& lat, double& height)
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

void convert::lonlat_to_ecef(const double& lon, const double& lat, const double& height, double& x, double& y, double& z)
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

convert::pfour convert::build(const std::vector<convert::point2d>& measures, const std::vector<convert::point2d>& origins)
{
    pfour params;
    throw std::runtime_error("Not implemented");
    // assert(measures.size() == origins.size() && measures.size() >= 2);

    // const size_t n = measures.size();
    //  std::vector<std::vector<double>> A(2 * n, std::vector<double>(4));
    //  std::vector<double> B(2 * n);

    // // 构造矩阵方程（参考网页2公式3）
    // for (size_t i = 0; i < n; ++i)
    // {
    //     double x = measures[i].x, y = measures[i].y;
    //     double u = origins[i].x, v = origins[i].y;

    //     // 填充矩阵A和向量B
    //     A[2 * i] = {1, 0, x, -y};
    //     A[2 * i + 1] = {0, 1, y, x};
    //     B[2 * i] = u;
    //     B[2 * i + 1] = v;
    // }

    // // 手动计算最小二乘解：X = (A^T * A)^-1 * A^T * B
    // std::vector<std::vector<double>> AT(4, std::vector<double>(2 * n));
    // for (int i = 0; i < 4; ++i)
    //     for (int j = 0; j < 2 * n; ++j)
    //         AT[i][j] = A[j][i];

    // // 计算ATA = A^T * A
    // std::vector<std::vector<double>> ATA(4, std::vector<double>(4));
    // for (int i = 0; i < 4; ++i)
    //     for (int j = 0; j < 4; ++j)
    //         for (int k = 0; k < 2 * n; ++k)
    //             ATA[i][j] += AT[i][k] * A[k][j];

    // // 计算ATB = A^T * B
    // std::vector<double> ATB(4);
    // for (int i = 0; i < 4; ++i)
    //     for (int k = 0; k < 2 * n; ++k)
    //         ATB[i] += AT[i][k] * B[k];

    // // 求逆并解方程
    // auto invATA = inverse4x4(ATA);
    // std::vector<double> X(4);
    // for (int i = 0; i < 4; ++i)
    //     for (int j = 0; j < 4; ++j)
    //         X[i] += invATA[i][j] * ATB[j];

    // // 参数计算（参考网页2公式4）
    //
    // params.dx = X[0];
    // params.dy = X[1];
    // params.s = std::hypot(X[2], X[3]);
    // params.r = std::atan2(X[3], X[2]);
    return params;
}

convert::point2d convert::trans(const convert::point2d& meas, const convert::pfour& p4)
{
    const double c = p4.s * cos(p4.r);
    const double d = p4.s * sin(p4.r);
    return {p4.dx + c * meas.x - d * meas.y, p4.dy + d * meas.x + c * meas.y};
}

convert::helmert convert::build(const std::vector<convert::point3d>& measures, const std::vector<convert::point3d>& origins)
{
    helmert params;
    throw std::runtime_error("Not implemented");
    // assert(measures.size() == origins.size() && measures.size() >= 3);

    //// 构造最小二乘方程（参考网页6、7的矩阵方法）
    // Eigen::MatrixXd A(3 * measures.size(), 7);
    // Eigen::VectorXd L(3 * measures.size());

    // for (size_t i = 0; i < measures.size(); ++i)
    //{
    //     double X = measures[i].x;
    //     double Y = measures[i].y;
    //     double Z = measures[i].z;
    //     double x = origins[i].x;
    //     double y = origins[i].y;
    //     double z = origins[i].z;

    //    // 对应网页6的转换方程展开项
    //    A.block<3, 7>(3 * i, 0) << 1, 0, 0, 0, -Z, Y, X, 0, 1, 0, Z, 0, -X, Y, 0, 0, 1, -Y, X, 0, Z;

    //    L.segment<3>(3 * i) << x - X, y - Y, z - Z;
    //}

    // Eigen::VectorXd X = A.BDCSVD(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(L);

    // params.dx = X[0];  // X平移
    // params.dy = X[1];  // Y平移
    // params.dz = X[2];  // Z平移
    // params.rx = X[3];  // X旋转（弧度）
    // params.ry = X[4];  // Y旋转（弧度）
    // params.rz = X[5];  // Z旋转（弧度）
    // params.s = X[6];   // 尺度因子

    return params;
}

convert::point3d convert::trans(const convert::point3d& meas, const convert::helmert& p7)
{
    const double rx = p7.rx, ry = p7.ry, rz = p7.rz;
    const double s = 1 + p7.s;

    const double R[3][3] = {{s * (cos(ry) * cos(rz)), s * (cos(ry) * sin(rz)), s * (-sin(ry))},
                            {s * (sin(rx) * sin(ry) * cos(rz) - cos(rx) * sin(rz)), s * (sin(rx) * sin(ry) * sin(rz) + cos(rx) * cos(rz)), s * (sin(rx) * cos(ry))},
                            {s * (cos(rx) * sin(ry) * cos(rz) + sin(rx) * sin(rz)), s * (cos(rx) * sin(ry) * sin(rz) - sin(rx) * cos(rz)), s * (cos(rx) * cos(ry))}};

    return {p7.dx + R[0][0] * meas.x + R[0][1] * meas.y + R[0][2] * meas.z, p7.dy + R[1][0] * meas.x + R[1][1] * meas.y + R[1][2] * meas.z, p7.dz + R[2][0] * meas.x + R[2][1] * meas.y + R[2][2] * meas.z};
}