//
// Created by dell on 2023/11/17.
//

#include "silly_polar.h"
#include <cmath>

#define SILLY_POLAR_PI (3.1415926535897932384626433832795028841971693993751f)

void silly_polar::normal_to_polar(const double &x, const double &y, double &r, double &azimuth)
{
    r = sqrt(x * x + y * y);
    azimuth = atan(y / x);
}

void silly_polar::normal_to_polar(const double &x0, const double &y0, const double &x, const double &y, double &r, double &azimuth)
{
    r = sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
    azimuth = atan((y - y0) / (x - x0));
}

void silly_polar::normal_to_coord(const double &r, const double &azimuth, double &x, double &y)
{
    double pi_angle = azimuth * SILLY_POLAR_PI * 2 / 360.;
    x = r * cos(pi_angle);
    y = r * sin(pi_angle);
}

void silly_polar::normal_to_coord(const double &r, const double &azimuth, const double &x0, const double &y0, double &x, double &y)
{
    double pi_angle = azimuth * SILLY_POLAR_PI * 2 / 360.;
    x = r * cos(pi_angle) + x0;
    y = r * sin(pi_angle) + y0;
}
