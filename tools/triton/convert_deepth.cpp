/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-13
 * @file: convert_deepth.c
 * @description: convert_deepth实现
 * @version: v1.0.1 2024-09-13 dou li yang
 */

#include <image/png_utils.h>
#include <weather/silly_ascii_grid.h>
#include <proj/silly_proj.h>
#include <pool/silly_thread_pool.h>
using namespace silly_image;

std::string root = "//192.168.0.9/webs/dem/server/projects";
int threads = 32;

#define TRITON_DEPTH_TINY   1e-2

void write_depth_png(double mid, const silly_ascii_grid& sag, std::string dst);
void write_q_png(double mid, const silly_ascii_grid& dgrid, const silly_ascii_grid& ugrd, const silly_ascii_grid& vgrid, std::string dst);

double center_x(const std::string& prj);

void convert_depth_by_plan(const std::string& path, double mid);
void convert_q_by_plan(const std::string& path, double mid);

int main(int argc, char** argv)
{
    /*
        silly_ascii_grid sag1;
        sag1.read("Z:/tmp/H_48_00.asc");
        sag1.m_data.release();
        return 0;*/

    /*   double x = 112, y = 34;
       double nx, ny;
       silly_proj::lonlat_to_mercator(x, y, nx, ny);
       std::cout << nx << "," << ny << std::endl;
       // latLonToMercator(y, x, &ny, &nx);
       std::cout << nx << "," << ny << std::endl;
       silly_proj::mercator_to_lonlat(nx, ny, x, y);
       std::cout << x << "," << y << std::endl;
       mercatorToLatLon(nx, ny, &y, &x);
       std::cout << x << "," << y << std::endl;

       return 0;*/

    if (argc >= 2)
    {
        root = __argv[1];
    }

    std::cout << "root:" << root << std::endl;
    if (argc >= 3)
    {
        threads = std::max(threads, std::stoi(__argv[2]));
    }
    std::cout << "threads:" << threads << std::endl;
    {
        silly_thread_pool pools(threads);
        std::filesystem::path path(root);
        for (auto iter1 : std::filesystem::directory_iterator(path))
        {
            auto sfp_tmp = iter1.path();
            std::string proj_name = iter1.path().stem().string();
            if (proj_name != "tjhd")
            {
                continue;
            }
            std::string prj = std::filesystem::path(iter1.path()).append(proj_name.append(".prj")).string();
            double l0 = center_x(prj);

            auto sfp_output = sfp_tmp.append("output");
            if (!std::filesystem::exists(sfp_output))
                continue;
            for (auto iter2 : std::filesystem::directory_iterator(sfp_output))
            {
                // convert_q_by_plan(iter2.path().string(), l0);
                convert_depth_by_plan(iter2.path().string(), l0);
#if 1
                // pools.enqueue(convert_depth_by_plan, iter2.path().string(), l0);
#else
                pools.enqueue(convert_q_by_plan, iter2.path().string(), l0);
#endif
            }
        }
    }
    std::cout << "Done" << std::endl;
    return 0;
}

void convert_depth_by_plan(const std::string& path, double mid)
{
    auto sfp_bin = std::filesystem::path(path).append("asc");
    auto sfp_image = std::filesystem::path(path).append("image");
    if (!std::filesystem::exists(sfp_bin))
        return;
    for (auto iter2 : std::filesystem::directory_iterator(sfp_bin))
    {
        std::string dd = iter2.path().stem().string();
        bool valid = (dd[0] == 'H');
        valid &= (dd[1] == '_');
        valid &= (dd.size() == 7);
        valid &= (iter2.path().extension() == ".asc");
        if (!valid)
        {
            continue;
        }
        if("H_07_00" != iter2.path().stem().string())
        {
            //continue;
        }
        silly_ascii_grid sag;
        auto tmp_file = std::filesystem::path(path).append("bin").append(iter2.path().stem().string().append(".bin"));
        tmp_file = iter2.path().string();
        if (!sag.read(tmp_file.string()))
        {
            std::cerr << "read " << iter2.path().string() << " failed" << std::endl;
            continue;
        }
        std::string dst_png = std::filesystem::path(sfp_image).append(dd + ".png").string();
        std::cout << dst_png << std::endl;
        write_depth_png(mid, sag, dst_png);
        sag.m_data.release();
    }
}

void convert_q_by_plan(const std::string& path, double mid)
{
    auto sfp_bin = std::filesystem::path(path).append("asc");
    auto sfp_image = std::filesystem::path(path).append("image");
    std::cout << sfp_bin.string() << std::endl;
    if (!std::filesystem::exists(sfp_bin))
        return;
    for (auto iter2 : std::filesystem::directory_iterator(sfp_bin))
    {
        std::string dd = iter2.path().stem().string();
        bool valid = (dd[0] == 'H');
        valid &= (dd[1] == '_');
        valid &= (dd.size() == 7);
        valid &= (iter2.path().extension() == ".asc");
        if (!valid)
        {
            continue;
        }
        std::cout << iter2.path().string() << std::endl;

        silly_ascii_grid dgrd, xgrd, ygrd;
        std::string qxname = "QX", qyname = "QY";
        std::string suffix = iter2.path().filename().string().substr(1);
        qxname.append(suffix);
        qyname.append(suffix);
        if (!dgrd.read(iter2.path().string()))
        {
            std::cout << "read " << iter2.path().string() << " failed" << std::endl;
            continue;
        }
        std::string qxpath = iter2.path().parent_path().append(qxname).string();
        if (!xgrd.read(qxpath))
        {
            std::cout << "read " << iter2.path().string() << " failed" << std::endl;
            continue;
        }
        std::string qypath = iter2.path().parent_path().append(qyname).string();
        if (!ygrd.read(qypath))
        {
            std::cout << "read " << iter2.path().string() << " failed" << std::endl;
            continue;
        }
        dd[0] = 'Q';
        std::string dst_png = std::filesystem::path(sfp_image).append(dd + ".png").string();
        std::cout << dst_png << std::endl;
#if 1
        write_q_png(mid, dgrd, xgrd, ygrd, dst_png);
#endif
        dgrd.m_data.release();
        xgrd.m_data.release();
        ygrd.m_data.release();
        // break;
    }
}


void cuda_depth_cvt(const silly_ascii_grid& sag)
{
    size_t nrows = sag.nrows, ncols = sag.ncols;
    /*double cellsize = sag.cellsize;
    double hstep = std::max(0.0001, (sag.MAXV - sag.MINV) / 255);
    for (size_t r = 0; r < nrows; ++r)
    {
        for (size_t c = 0; c < ncols; ++c)
        {
            double tmp_mct_x = mct_letf + c * mct_x_step;
            double tmp_mct_y = mct_top - r * mct_y_step;
            double tmp_gauss_x, tmp_gauss_y;
            silly_proj::mercator_to_gauss(mid, tmp_mct_x, tmp_mct_y, tmp_gauss_x, tmp_gauss_y);

            int gcol = std::round((tmp_gauss_x - gauss_left) / cellsize + 1);
            int grol = std::round((gauss_top - tmp_gauss_y) / cellsize);
            if (gcol >= 0 && grol > 0 && gcol < ncols && grol < nrows)
            {
                double v = sag.m_data[grol][gcol];
                silly_color sc;
                sc.red = static_cast<unsigned char>(std::min(255., (v - sag.MINV) / hstep));
                pd.set_pixel(r, c, sc);
            }
        }
    }*/
}
void write_depth_png(double mid, const silly_ascii_grid& sag, std::string dst)
{
    size_t nrows = sag.nrows, ncols = sag.ncols;
    double xllcorner = sag.xllcorner, yllcorner = sag.yllcorner;
    double cellsize = sag.cellsize;

    double mct_letf = 0, mct_right = 0, mct_top = 0, mct_bottom = 0;
    const double gauss_left = xllcorner, gauss_right = xllcorner + cellsize * ncols, gauss_top = yllcorner + cellsize * nrows, gauss_bottom = yllcorner;
    {
        double left_top_x, left_top_y;
        silly_proj::gauss_to_lonlat(mid, gauss_left, gauss_top, left_top_x, left_top_y);
        double right_bottom_x, right_bottom_y;
        silly_proj::gauss_to_lonlat(mid, gauss_right, gauss_bottom, right_bottom_x, right_bottom_y);
        double left_bottom_x, left_bottom_y;
        silly_proj::gauss_to_lonlat(mid, gauss_left, gauss_bottom, left_bottom_x, left_bottom_y);
        double right_top_x, right_top_y;
        silly_proj::gauss_to_lonlat(mid, gauss_right, gauss_top, right_top_x, right_top_y);

        double geo_left = SU_MAX(left_top_x, left_bottom_x);
        double geo_right = SU_MIN(right_bottom_x, right_top_x);
        double geo_top = SU_MIN(right_top_y, left_top_y);
        double geo_bottom = SU_MAX(left_bottom_y, right_bottom_y);
        silly_proj::lonlat_to_mercator(geo_left, geo_top, mct_letf, mct_top);
        silly_proj::lonlat_to_mercator(geo_right, geo_bottom, mct_right, mct_bottom);
    }

    double mct_x_step = (mct_right - mct_letf) / ncols;
    double mct_y_step = (mct_top - mct_bottom) / nrows;
    png_data pd = silly_image::png_utils::create_empty(nrows, ncols, PNG_COLOR_TYPE_RGB);
    double hstep = (sag.MAXV - sag.MINV) / 255.;
    for (size_t r = 0; r < nrows; ++r)
    {
        for (size_t c = 0; c < ncols; ++c)
        {
            double tmp_mct_x = mct_letf + c * mct_x_step;
            double tmp_mct_y = mct_top - r * mct_y_step;
            double tmp_gauss_x, tmp_gauss_y;
            silly_proj::mercator_to_gauss(mid, tmp_mct_x, tmp_mct_y, tmp_gauss_x, tmp_gauss_y);

            int gcol = std::round((tmp_gauss_x - gauss_left) / cellsize + 1);
            int grol = std::round((gauss_top - tmp_gauss_y) / cellsize);
            if (gcol >= 0 && grol > 0 && gcol < ncols && grol < nrows)
            {
                double v = sag.m_data[grol][gcol];
                if(v > TRITON_DEPTH_TINY)
                {
                    silly_color sc;
                    sc.red = static_cast<uint8_t>(std::floor(std::min(255., (v - sag.MINV) / hstep)));
                    pd.set_pixel(r, c, sc);
                }

            }
        }
    }

    std::string h_pd_data;
    {
        h_pd_data.resize(24);
        int ihMax = static_cast<int>(sag.MAXV / TRITON_DEPTH_TINY);
        int ihMin = static_cast<int>(sag.MINV / TRITON_DEPTH_TINY);
        int iRow = static_cast<int>(nrows);
        int iCol = static_cast<int>(ncols);
        memcpy(&h_pd_data[0], &iRow, sizeof(iRow));
        memcpy(&h_pd_data[0] + sizeof(int) * 1, &iCol, sizeof(iCol));
        memcpy(&h_pd_data[0] + sizeof(int) * 2, &ihMax, sizeof(ihMax));
        memcpy(&h_pd_data[0] + sizeof(int) * 3, &ihMin, sizeof(ihMin));
    }
    std::string h_png_data;
    png_utils::memory_encode(pd, h_png_data);
    h_pd_data.append(h_png_data);
    silly_file::write(dst, h_pd_data);
    png_utils::write(dst.append(".png"), pd);
    pd.release();
}

void write_q_png(double mid, const silly_ascii_grid& dgrid, const silly_ascii_grid& ugrd, const silly_ascii_grid& vgrd, std::string dst)
{
    size_t nrows = dgrid.nrows, ncols = dgrid.ncols;
    double xllcorner = dgrid.xllcorner, yllcorner = dgrid.yllcorner;
    double cellsize = dgrid.cellsize;
    double uMax = -99999.0, uMin = 99999.0, vMax = -99999.0, vMin = 99999.0;
    {
        for (size_t r = 0; r < nrows; ++r)
        {
            for (size_t c = 0; c < ncols; ++c)
            {
                int pos = SU_MAX(0, SU_MIN(r * ncols + c, nrows * ncols));
                double h = dgrid.m_data[r][c];
                if (h > SILLY_IGNORE_WATER_DEPTH_METER)
                {
                    double u = ugrd.m_data[r][c] / h;
                    double v = vgrd.m_data[r][c] / h;

                    uMax = SU_MAX(uMax, u);
                    uMin = SU_MIN(uMin, u);
                    vMax = SU_MAX(vMax, v);
                    vMin = SU_MIN(vMin, v);
                }
            }
        }
    }

    double mct_letf = 0, mct_right = 0, mct_top = 0, mct_bottom = 0;
    const double gauss_left = xllcorner, gauss_right = xllcorner + cellsize * ncols, gauss_top = yllcorner + cellsize * nrows, gauss_bottom = yllcorner;
    {
        double left_top_x, left_top_y;
        silly_proj::gauss_to_lonlat(mid, gauss_left, gauss_top, left_top_x, left_top_y);
        double right_bottom_x, right_bottom_y;
        silly_proj::gauss_to_lonlat(mid, gauss_right, gauss_bottom, right_bottom_x, right_bottom_y);
        double left_bottom_x, left_bottom_y;
        silly_proj::gauss_to_lonlat(mid, gauss_left, gauss_bottom, left_bottom_x, left_bottom_y);
        double right_top_x, right_top_y;
        silly_proj::gauss_to_lonlat(mid, gauss_right, gauss_top, right_top_x, right_top_y);

        double geo_left = SU_MAX(left_top_x, left_bottom_x);
        double geo_right = SU_MIN(right_bottom_x, right_top_x);
        double geo_top = SU_MIN(right_top_y, left_top_y);
        double geo_bottom = SU_MAX(left_bottom_y, right_bottom_y);
        silly_proj::lonlat_to_mercator(geo_left, geo_top, mct_letf, mct_top);
        silly_proj::lonlat_to_mercator(geo_right, geo_bottom, mct_right, mct_bottom);
    }

    double mct_x_step = (mct_right - mct_letf) / ncols;
    double mct_y_step = (mct_top - mct_bottom) / nrows;
    png_data pd2 = silly_image::png_utils::create_empty(nrows, ncols, PNG_COLOR_TYPE_RGB);
    for (size_t r = 0; r < nrows; ++r)
    {
        for (size_t c = 0; c < ncols; ++c)
        {
            double tmp_mct_x = mct_letf + c * mct_x_step;
            double tmp_mct_y = mct_top - r * mct_y_step;
            double tmp_gauss_x, tmp_gauss_y;
            silly_proj::mercator_to_gauss(mid, tmp_mct_x, tmp_mct_y, tmp_gauss_x, tmp_gauss_y);
            int gcol = std::round((tmp_gauss_x - gauss_left) / cellsize + 1);
            int grol = std::round((gauss_top - tmp_gauss_y) / cellsize);
            if (gcol >= 0 && grol > 0 && gcol < ncols && grol < nrows)
            {
                double d = dgrid.m_data[grol][gcol];
                double u = ugrd.m_data[grol][gcol];
                double v = ugrd.m_data[grol][gcol];
                silly_color tmp(0, 0, 0);
                if (d < SILLY_IGNORE_WATER_DEPTH_METER)
                {
                    tmp.red = 255.0 * (0 - uMin) / (uMax - uMin);
                    tmp.green = 255.0 * (0 - vMin) / (vMax - vMin);
                }
                else
                {
                    double qu = u / d;
                    double qv = v / d;

                    tmp.red = 255.0 * (qu - uMin) / (uMax - uMin);
                    tmp.green = 255.0 * (qv - vMin) / (vMax - vMin);
                }
                pd2.set_pixel(r, c, tmp);
            }
        }
    }
    std::string pd2_data;

    {
        pd2_data.resize(24);
        int iUMax = static_cast<int>(uMax * TRITON_DEPTH_TINY);
        int iVMax = static_cast<int>(vMax * TRITON_DEPTH_TINY);
        int iUMin = static_cast<int>(uMin * TRITON_DEPTH_TINY);
        int iVMin = static_cast<int>(vMin * TRITON_DEPTH_TINY);
        int iRow = static_cast<int>(nrows);
        int iCol = static_cast<int>(ncols);
        memcpy(&pd2_data[0], &iRow, sizeof(iRow));
        memcpy(&pd2_data[0] + sizeof(int) * 1, &iCol, sizeof(iCol));
        memcpy(&pd2_data[0] + sizeof(int) * 2, &iUMax, sizeof(iUMax));
        memcpy(&pd2_data[0] + sizeof(int) * 3, &iUMin, sizeof(iUMin));
        memcpy(&pd2_data[0] + sizeof(int) * 4, &iVMax, sizeof(iVMax));
        memcpy(&pd2_data[0] + sizeof(int) * 5, &iVMin, sizeof(iVMin));
    }

    std::string png_data;
    png_utils::memory_encode(pd2, png_data);
    pd2_data.append(png_data);
    silly_file::write(dst, pd2_data);
    pd2.release();
}

double center_x(const std::string& prj)
{
    std::vector<std::string> lines;
    silly_file::read(prj, lines);
    if (lines.size() == 11)
    {
        return std::stod(lines[7]);
    }

    return 0;
}
