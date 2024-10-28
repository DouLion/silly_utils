#include "silly_pyramid.h"

silly_rect silly_google_pyramid::get_rect(const pyramid_index& index)
{
    silly_rect ret;
    size_t ceil_num = static_cast<size_t>(2 << index.layer);  // 该层的行数列数
    if (!(index.row < ceil_num && index.col < ceil_num))
    {
        throw std::runtime_error("行列号不能超过 " + std::to_string(ceil_num));
    }
    double x_delta = (SILLY_GLOBAL_RIGHT - SILLY_GLOBAL_LEFT) / ceil_num;
    double y_delta = (SILLY_GLOBAL_TOP - SILLY_GLOBAL_BOTTOM) / ceil_num;
    ret.min.x = index.col * x_delta + SILLY_GLOBAL_LEFT;  // 原点x方向上的偏移量
    ret.max.x = ret.min.x + x_delta;
    ret.max.y = SILLY_GLOBAL_TOP - y_delta * index.row;  // 方向上由上到下, 与经纬度表示的相反, 并且减去y方向上的偏移量
    ret.min.y = ret.max.y - y_delta;

    return ret;
}
pyramid_index silly_google_pyramid::get_index(const size_t& layer, const silly_point& point)
{
    if (point.x < SILLY_GLOBAL_LEFT || point.x > SILLY_GLOBAL_RIGHT || point.y < SILLY_GLOBAL_BOTTOM || point.y > SILLY_GLOBAL_TOP)
    {
        throw std::runtime_error("经纬度超出范围 x:" + std::to_string(point.x) + " y:" + std::to_string(point.y));
    }
    pyramid_index ret;
    ret.layer = layer;
    int ceil_num = (2 << layer);  // 该层的行数列数
    double x_delta = (SILLY_GLOBAL_RIGHT - SILLY_GLOBAL_LEFT) / ceil_num;
    double y_delta = (SILLY_GLOBAL_TOP - SILLY_GLOBAL_BOTTOM) / ceil_num;
    ret.col = ceil((point.x - SILLY_GLOBAL_LEFT) / x_delta);
    ret.row = ceil((SILLY_GLOBAL_TOP - point.y) / y_delta);
    return ret;
}

pyramid_index silly_pyramid_proj::geo2pyramid(const uint8_t& layer, const silly_point& gp, const uint64_t& tilesize)
{
    pyramid_index ret;

    screen_point sp = geo2screen(layer, gp, tilesize);
    ret = screen2pyramid(layer, sp, tilesize);
    return ret;
}

silly_rect silly_pyramid_proj::pyramid2geo(const pyramid_index& index, const uint64_t& tilesize)
{
    silly_rect ret;
    size_t ceil_num = static_cast<size_t>(2 << index.layer);  // 该层的行数列数
    double x_delta = (SILLY_GLOBAL_RIGHT - SILLY_GLOBAL_LEFT) / ceil_num;
    double y_delta = (SILLY_GLOBAL_TOP - SILLY_GLOBAL_BOTTOM) / ceil_num;
    ret.min.x = index.col * x_delta + SILLY_GLOBAL_LEFT;  // 原点x方向上的偏移量
    ret.max.x = ret.min.x + x_delta;
    ret.max.y = SILLY_GLOBAL_TOP - y_delta * index.row;  // 方向上由上到下, 与经纬度表示的相反, 并且减去y方向上的偏移量
    ret.min.y = ret.max.y - y_delta;

    return ret;
}

screen_point silly_pyramid_proj::geo2screen(const uint8_t& layer, const silly_point& gp, const uint64_t& tilesize)
{
    screen_point ret;
    double total_tilesize = tilesize << layer;  // 第layer层的瓦片大小
    ret.layer = layer;
    ret.sr = std::floor(total_tilesize * (1.0 - (log(tan((gp.y * PI / 180.0) / 2.0 + PI / 4.0)) / PI)) / 2.0);
    ret.sc = std::floor((gp.x + 180.0) / 360.0 * total_tilesize);
    return ret;
}

screen_rect silly_pyramid_proj::geo2screen(const uint8_t& layer, const silly_rect& rect, const uint64_t& tilesize)
{
    screen_rect ret;
    double total_tilesize = tilesize << layer;  // 第layer层的瓦片大小
    ret.layer = layer;
    ret.min.sr = std::floor(total_tilesize * (1.0 - (log(tan((rect.min.y * PI / 180.0) / 2.0 + PI / 4.0)) / PI)) / 2.0);
    ret.min.sc = std::floor((rect.min.x + 180.0) / 360.0 * total_tilesize);
    ret.max.sr = std::ceil(total_tilesize * (1.0 - (log(tan((rect.max.y * PI / 180.0) / 2.0 + PI / 4.0)) / PI)) / 2.0);
    ret.max.sc = std::ceil((rect.max.x + 180.0) / 360.0 * total_tilesize);

    return ret;
}

silly_point silly_pyramid_proj::screen2geo(const uint8_t& layer, const screen_point& sp, const uint64_t& tilesize)
{
    silly_point ret;
    double total_tilesize = tilesize << layer;
    // 计算经度
    ret.x = (sp.sc / total_tilesize) * 360.0 - 180.0;

    // 计算纬度
    double n = PI - (2.0 * PI * sp.sr) / total_tilesize;
    double latRad = atan(sinh(n));
    ret.y = latRad * 180.0 / PI;
    return ret;
}

pyramid_index silly_pyramid_proj::screen2pyramid(const uint8_t& layer, const screen_point& sp, const uint64_t& tilesize)
{
    pyramid_index ret;
    ret.row = sp.sr % tilesize;
    ret.col = sp.sc % tilesize;

    return ret;
}

screen_rect silly_pyramid_proj::pyramid2screen(const pyramid_index& index, const uint64_t& tilesize)
{
    screen_rect ret;
    ret.layer = index.layer;
    ret.min.sr = index.row * tilesize;
    ret.min.sc = index.col * tilesize;
    ret.max.sr = (index.row + 1) * tilesize;
    ret.max.sc = (index.col + 1) * tilesize;
    return ret;
}
