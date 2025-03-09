//
// Created by dell on 2024/7/19.
//

#include <ascii_grid_image.h>
#include <graphics/silly_png.h>
#include <files/silly_file.h>
#include <geo/proj/silly_proj.h>
using namespace silly::geo;

#define DEPTH_TINY 1e-2

void convert_image(double ncols, double nrows, double xllcorner, double yllcorner, double cellsize, double mid, double* data, double* qx, double* qy, char* img_path)
{
    double uMax = -99999.0, uMin = 99999.0, vMax = -99999.0, vMin = 99999.0;
    double hMax = -99999.0, hMin = 99999.0;
    if (data)
    {
        for (size_t r = 0; r < nrows; ++r)
        {
            for (size_t c = 0; c < ncols; ++c)
            {
                int pos = SU_MAX(0, SU_MIN(r * ncols + c, nrows * ncols));
                double h = data[pos];
                hMax = SU_MAX(hMax, h);
                hMin = SU_MIN(hMin, h);
                if (h < DEPTH_TINY)
                {
                    continue;
                }

                double u = qx[pos] / h;
                double v = qy[pos] / h;

                // if (u > DEPTH_TINY)
                {
                    uMax = SU_MAX(uMax, u);
                    uMin = SU_MIN(uMin, u);
                }
                // if (v > DEPTH_TINY)
                {
                    vMax = SU_MAX(vMax, v);
                    vMin = SU_MIN(vMin, v);
                }
            }
        }
    }

    double mct_letf = 0, mct_right = 0, mct_top = 0, mct_bottom = 0;
    const double gauss_left = xllcorner, gauss_right = xllcorner + cellsize * (ncols - 2), gauss_top = yllcorner + cellsize * (nrows - 2), gauss_bottom = yllcorner;
    // std::cout << mid << std::endl;
    {
        double left_top_x, left_top_y;
        proj::gauss_to_lonlat(mid, gauss_left, gauss_top, left_top_x, left_top_y);
        double right_bottom_x, right_bottom_y;
        proj::gauss_to_lonlat(mid, gauss_right, gauss_bottom, right_bottom_x, right_bottom_y);
        double left_bottom_x, left_bottom_y;
        proj::gauss_to_lonlat(mid, gauss_left, gauss_bottom , left_bottom_x, left_bottom_y);
        double right_top_x, right_top_y;
        proj::gauss_to_lonlat(mid, gauss_right, gauss_top , right_top_x, right_top_y);

        double geo_left = SU_MAX(left_top_x, left_bottom_x);

        // 112 .34067;                   // SU_MIN(left_top_x, left_bottom_x)
        double geo_right = SU_MIN(right_bottom_x, right_top_x);

        // 112 .5683;  // SU_MAX(right_top_x, right_bottom_x);
        double geo_top = SU_MIN(right_top_y, left_top_y);
        // 26 .98808;
        // SU_MAX(left_top_y, right_top_y);
        double geo_bottom = SU_MAX(left_bottom_y, right_bottom_y);
        // 26.86;
        // SU_MIN(left_bottom_y, right_bottom_y);

        proj::lonlat_to_mercator(geo_left, geo_top , mct_letf, mct_top);
        proj::lonlat_to_mercator(geo_right, geo_bottom , mct_right, mct_bottom);
        std::cout << mid << std::endl;
        std::cout << geo_left << " " << geo_top << " " << geo_right << " " << geo_bottom << std::endl;
        std::cout << mct_letf << " " << mct_top << " " << mct_right << " " << mct_bottom << std::endl;
    }

    double mct_x_step = (mct_right - mct_letf) / (ncols - 2);
    double mct_y_step = (mct_top - mct_bottom) / (nrows - 2);
    silly::png::data pd, pd2;
    pd.create((nrows - 2), (ncols - 2), silly::color::eptRGB);
    pd2.create((nrows - 2), (ncols - 2), silly::color::eptRGB);
    double hstep = (hMax - hMin) / 255;
    double ustep = (uMax - uMin) / 255;
    double vstep = (vMax - vMin) / 255;
#if 0

    std::cout << hMax << " " << hMin << std::endl;
    std::cout << uMax << " " << uMin << std::endl;
    std::cout << vMax << " " << vMin << std::endl;
    std::cout << hstep << " " << ustep << " " << vstep << std::endl;
#endif
    for (size_t r = 0; r < nrows - 2; ++r)
    {
        for (size_t c = 0; c < ncols - 2; ++c)
        {
            double tmp_mct_x = mct_letf + c * mct_x_step;
            double tmp_mct_y = mct_top - r * mct_y_step;
            double tmp_gauss_x, tmp_gauss_y;
            proj::mercator_to_gauss(mid, tmp_mct_x, tmp_mct_y, tmp_gauss_x, tmp_gauss_y);

            int gcol = std::round((tmp_gauss_x - gauss_left) / cellsize + 1);
            int grol = std::round((gauss_top - tmp_gauss_y) / cellsize);
            double v = 0., qu = 0., qv = 0.;
            if (gcol >= 0 && grol > 0 && gcol < ncols && grol < nrows)
            {
                int pos = SU_MAX(0, SU_MIN(grol * ncols + gcol, nrows * ncols));
                v = data[pos];
                if (v > DEPTH_TINY)
                {
                    qu = qx[pos] / v;
                    qv = qy[pos] / v;
                }
                else
                {
                    v = 0.;
                }
            }
            silly::color cXY, cH;
            cXY.red = (qu - uMin) / ustep;
            cXY.green = (qv - vMin) / vstep;
            pd2.pixel(r, c, cXY);
            // 写入深度数据
            cH.red = static_cast<unsigned char>(std::min(255., (v - hMin) / hstep));
            pd.pixel(r, c, cH);
        }
    }
    std::string h_pd_data;
    {
        h_pd_data.resize(24);
        int ihMax = static_cast<int>(hMax / DEPTH_TINY);
        int ihMin = static_cast<int>(hMin / DEPTH_TINY);
        int iRow = static_cast<int>(nrows);
        int iCol = static_cast<int>(ncols);
        memcpy(&h_pd_data[0], &iRow, sizeof(iRow));
        memcpy(&h_pd_data[0] + sizeof(int) * 1, &iCol, sizeof(iCol));
        memcpy(&h_pd_data[0] + sizeof(int) * 2, &ihMax, sizeof(ihMax));
        memcpy(&h_pd_data[0] + sizeof(int) * 3, &ihMin, sizeof(ihMin));
    }
    std::string h_png_data = pd.encode();
    h_pd_data.append(h_png_data);
    silly_file::write(img_path, h_pd_data);
    pd.release();
    std::string filename = std::filesystem::path(img_path).filename().string();
    filename[0] = 'Q';
    std::string pd2_path = std::filesystem::path(img_path).parent_path().append(filename).string();
    std::string pd2_data;

    pd2_data.resize(24);
    int iUMax = static_cast<int>(uMax / DEPTH_TINY);
    int iVMax = static_cast<int>(vMax / DEPTH_TINY);
    int iUMin = static_cast<int>(uMin / DEPTH_TINY);
    int iVMin = static_cast<int>(vMin / DEPTH_TINY);
    int iRow = static_cast<int>(nrows);
    int iCol = static_cast<int>(ncols);
    memcpy(&pd2_data[0], &iRow, sizeof(iRow));
    memcpy(&pd2_data[0] + sizeof(int) * 1, &iCol, sizeof(iCol));
    memcpy(&pd2_data[0] + sizeof(int) * 2, &iUMax, sizeof(iUMax));
    memcpy(&pd2_data[0] + sizeof(int) * 3, &iUMin, sizeof(iUMin));
    memcpy(&pd2_data[0] + sizeof(int) * 4, &iVMax, sizeof(iVMax));
    memcpy(&pd2_data[0] + sizeof(int) * 5, &iVMin, sizeof(iVMin));
    std::string pd2_content = pd2.encode();
    pd2_data.append(pd2_content);
    silly_file::write(pd2_path, pd2_data);
    pd2.release();
}
