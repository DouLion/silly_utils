//
// Created by dell on 2024/7/19.
//

#include "ascii_grid_image.h"
#include "image/png_utils.h"
#include "files/silly_file.h"
#include "proj/gdal/silly_proj_convert.h"
using namespace silly_image;



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
                double u = qx[pos] / h;
                double v = qy[pos] / h;

                uMax = SU_MAX(uMax, u);
                uMin = SU_MIN(uMin, u);
                vMax = SU_MAX(vMax, v);
                vMin = SU_MIN(vMin, v);
            }
        }
    }

    double mct_letf = 0, mct_right = 0, mct_top = 0, mct_bottom = 0;
    const double gauss_left = xllcorner, gauss_right = xllcorner + cellsize * (ncols - 2), gauss_top = yllcorner + cellsize * (nrows - 2), gauss_bottom = yllcorner;
    // std::cout << mid << std::endl;
    {
        double left_top_x, left_top_y;
        GaussToBL(mid, gauss_top, gauss_left, &left_top_x, &left_top_y);
        double right_bottom_x, right_bottom_y;
        GaussToBL(mid, gauss_bottom, gauss_right, &right_bottom_x, &right_bottom_y);
        double left_bottom_x, left_bottom_y;
        GaussToBL(mid, gauss_bottom, gauss_left, &left_bottom_x, &left_bottom_y);
        double right_top_x, right_top_y;
        GaussToBL(mid, gauss_top, gauss_right, &right_top_x, &right_top_y);

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

        latLonToMercator(geo_top, geo_left, &mct_letf, &mct_top);
        latLonToMercator(geo_bottom, geo_right, &mct_right, &mct_bottom);
        /* std::cout << geo_left << " " << geo_top << " " << geo_right << " " << geo_bottom << std::endl;
         std::cout << mct_letf << " " << mct_top << " " << mct_right << " " << mct_bottom << std::endl;*/
    }

    double mct_x_step = (mct_right - mct_letf) / (ncols - 2);
    double mct_y_step = (mct_top - mct_bottom) / (nrows - 2);
    png_data pd = silly_image::png_utils::create_empty((nrows - 2), (ncols - 2), PNG_COLOR_TYPE_RGB);
    png_data pd2 = silly_image::png_utils::create_empty((nrows - 2), (ncols - 2), PNG_COLOR_TYPE_RGB);
    double hstep = (hMax - hMin) / 255;
    for (size_t r = 0; r < nrows - 2; ++r)
    {
        for (size_t c = 0; c < ncols - 2; ++c)
        {
            double tmp_mct_x = mct_letf + c * mct_x_step;
            double tmp_mct_y = mct_top - r * mct_y_step;
            double tmp_gauss_x, tmp_gauss_y;
            mercator_gauss(tmp_mct_x, tmp_mct_y, tmp_gauss_x, tmp_gauss_y, mid);

            int gcol = std::round((tmp_gauss_x - gauss_left) / cellsize + 1);
            int grol = std::round((gauss_top - tmp_gauss_y) / cellsize);
            if (gcol >= 0 && grol > 0 && gcol < ncols && grol < nrows)
            {
                int pos = SU_MAX(0, SU_MIN(grol * ncols + gcol, nrows * ncols));
                double v = data[pos];
                silly_color tmp(0, 0, 0);
                if (v < 0.00001)
                {
                    tmp.red = 255.0 * (0 - uMin) / (uMax - uMin);
                    tmp.green = 255.0 * (0 - vMin) / (vMax - vMin);
                }
                else
                {
                    double qu = qx[pos] / v;
                    double qv = qy[pos] / v;

                    tmp.red = 255.0 * (qu - uMin) / (uMax - uMin);
                    tmp.green = 255.0 * (qv - vMin) / (vMax - vMin);
                }

                pd2.set_pixel(r, c, tmp);
                silly_color sc;
                sc.red = static_cast<unsigned char>(std::min(255., (v - hMin) / hstep));
                pd.set_pixel(r, c, sc);
            }
            else
            {
                silly_color tmp(0, 0, 0);
                tmp.red = 255.0 * (0 - uMin) / (uMax - uMin);
                tmp.green = 255.0 * (0 - vMin) / (vMax - vMin);
                pd2.set_pixel(r, c, tmp);
            }
        }
    }
    // png_utils::write(img_path, pd);
    std::string h_pd_data;
    {
        h_pd_data.resize(24);
        int ihMax = static_cast<int>(hMax * 100);
        int ihMin = static_cast<int>(hMin * 100);
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
    silly_file::write(img_path, h_pd_data);

    pd.release();
    std::string filename = std::filesystem::path(img_path).filename().string();
    filename[0] = 'Q';
    std::string pd2_path = std::filesystem::path(img_path).parent_path().append(filename).string();
    std::string pd2_data;

    pd2_data.resize(24);
    {
        int iUMax = static_cast<int>(uMax * 100);
        int iVMax = static_cast<int>(vMax * 100);
        int iUMin = static_cast<int>(uMin * 100);
        int iVMin = static_cast<int>(vMin * 100);
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
    silly_file::write(pd2_path, pd2_data);
    pd2.release();
}
void convert_image_test(double ncols, double nrows)
{
    double gx, gy;
    mercator_gauss(12702604.649057379, 3976012.012616653, gx, gy, 114.0);

    std::cout << gx << " " << gy << std::endl;

    return;
    std::cout << "Cols:\t" << ncols << std::endl;
    std::cout << "Rows:\t" << nrows << std::endl;
}