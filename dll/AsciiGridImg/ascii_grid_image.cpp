//
// Created by dell on 2024/7/19.
//

#include <ascii_grid_image.h>
#include <graphics/silly_png.h>
#include <files/silly_file.h>
#include <geo/proj/silly_proj.h>
#include <geo/silly_geo.h>
#ifndef NDEBUG
#include <datetime/silly_timer.h>
#endif

#define AG_COMPRESS_LEVEL 3

thread_local void* png_ptr = nullptr;
thread_local size_t* png_index = nullptr;
thread_local size_t png_width = 0;
thread_local size_t png_height = 0;
const double G_SCALE = 100.0;
const double G_MAX_V = 255.0;
const double G_TINY_DEPTH = 0.01;  // 最小水深 1cm, 小于该值都认为0
const int G_FLAG = -1;
// 最小水深, 最大水深, 最小qx, 最大qx, 最小qy, 最大qy
using MinMaxT = std::tuple<double, double, double, double, double, double>;
/**
 * 创建经纬度网格位置到高斯网格位置的映射,输出结果去除ghost cells
 * @ncols 列数 包含 ghost cells 上下左右个一个
 * @nrows 行数 包含 ghost cells 上下左右个一个
 * @xllcorner 左下角经度
 * @yllcorner 左下角纬度
 * @cellsize 网格大小
 * @mid 中央经线
 */
void create_map(double ncols, double nrows, double xllcorner, double yllcorner, double cellsize, double mid)
{
    if (!png_index)
    {
        suRect gaussbd;
        int width = ncols - 2;
        int height = nrows - 2;
        gaussbd.min.x = xllcorner;
        gaussbd.max.x = xllcorner + cellsize * width;
        gaussbd.min.y = yllcorner;
        gaussbd.max.y = yllcorner + cellsize * height;
        suPoint lt, rb, lb, rt;
        GAUSS2LONLAT(mid, gaussbd.min.x, gaussbd.max.y, lt.x, lt.y);
        GAUSS2LONLAT(mid, gaussbd.max.x, gaussbd.min.y, rb.x, rb.y);
        GAUSS2LONLAT(mid, gaussbd.min.x, gaussbd.min.y, lb.x, lb.y);
        GAUSS2LONLAT(mid, gaussbd.max.x, gaussbd.max.y, rt.x, rt.y);
        suRect nbd;
        nbd.min.x = std::max(lt.x, lb.x);
        nbd.max.x = std::min(rb.x, rt.x);
        nbd.max.y = std::min(rt.y, lt.y);
        nbd.min.y = std::max(lb.y, rb.y);

        /*LONLAT2MERCATOR(nbd.min.x, nbd.max.y, nbd.min.x, nbd.max.y);
        LONLAT2MERCATOR(nbd.max.x, nbd.min.y, nbd.max.x, nbd.min.y);*/
        double distx = nbd.max.x - nbd.min.x;
        double disty = nbd.max.y - nbd.min.y;
        double ncell = 0;
        png_width = width;
        png_height = height;

        if (distx < disty)
        {
            ncell = distx / width;
            png_height = std::floor(disty / ncell);
        }
        else
        {
            ncell = disty / height;
            png_width = std::floor(distx / ncell);
        }
        png_index = new size_t[static_cast<size_t>(png_width * png_height)];
        memset(png_index, 0, static_cast<size_t>(png_width * png_height) * sizeof(size_t));

        for (size_t r = 0; r < png_height; ++r)
        {
            for (size_t c = 0; c < png_width; ++c)
            {
                suPoint mp, gp, llp;
                llp.x = nbd.min.x + c * ncell;
                llp.y = nbd.max.y - r * ncell;

                // MERCATOR2GAUSS(mid, mp.x, mp.y, gp.x, gp.y);
                LONLAT2GAUSS(mid, llp.x, llp.y, gp.x, gp.y);

                int gcol = static_cast<size_t>(std::round((gp.x - gaussbd.min.x) / cellsize));
                int grol = static_cast<size_t>(std::round((gaussbd.max.y - gp.y) / cellsize));
                if (gcol >= 0 && grol >= 0 && gcol < width && grol < height)
                {
                    size_t gpos = (grol + 1) * ncols + gcol + 1;
                    size_t pos = r * png_width + c;
                    png_index[pos] = gpos;
                }
            }
        }
    }
}

/**
 * 统计水深和水流速 x y 分量上的 最大最小值
 * @param ncols 列数
 * @param nrows 行数
 * @param cellsize 网格大小
 * @param depth 水深
 * @param qx 水流速 x 分量
 * @param qy 水流速 y 分量
 * @return 最大最小值
 */
MinMaxT summary_minmax(size_t ncols, size_t nrows, double cellsize, double* depth, double* qx, double* qy)
{
    double hMax = -99999.0, hMin = 99999.0;
    double uMax = -99999.0, uMin = 99999.0;
    double vMax = -99999.0, vMin = 99999.0;
    vMax = -99999.0, vMin = 99999.0;
    if (!depth)
    {
        return std::make_tuple(hMin, hMax, uMin, uMax, vMin, vMax);
    }
    size_t pos = 0;
    size_t max = ncols * nrows - 1;

    while (pos++ < max)
    {
        double h = depth[pos];
        if (isnan(h) || h <= G_TINY_DEPTH)
        {
            depth[pos] = 0;
            h = 0;
            continue;
        }
        hMax = SU_MAX(hMax, h);
        hMin = SU_MIN(hMin, h);

        if (qx && qy)
        {
            double u = qx[pos] / (h * cellsize);
            double v = qy[pos] / (h * cellsize);

            // if (u > G_TINY_DEPTH)
            {
                uMax = SU_MAX(uMax, u);
                uMin = SU_MIN(uMin, u);
            }
            // if (v > G_TINY_DEPTH)
            {
                vMax = SU_MAX(vMax, v);
                vMin = SU_MIN(vMin, v);
            }
        }
    }
    hMin = 0;
    hMax = std::ceil(hMax * G_SCALE);
    uMax = std::ceil(uMax * G_SCALE);
    uMin = std::floor(uMin * G_SCALE);
    vMax = std::ceil(vMax * G_SCALE);
    vMin = std::floor(vMin * G_SCALE);
    return std::make_tuple(hMin, hMax, uMin, uMax, vMin, vMax);
}

/**
*  高斯网格数据转换为图片表示
*/
void convert_image(double ncols, double nrows, double xllcorner, double yllcorner, double cellsize, double mid, double* Hdata, double* qx, double* qy, char* img_path, bool verbose)
{
    if (!Hdata)
    {
        return;
    }

    auto [hMin, hMax, uMin, uMax, vMin, vMax] = summary_minmax(ncols, nrows, cellsize, Hdata, qx, qy);

    suPNG pdH, pdXY;
    pdH.compress_level(AG_COMPRESS_LEVEL);
    pdXY.compress_level(AG_COMPRESS_LEVEL);
    pdH.create(png_width, png_height, eColorType::RGB);
    pdXY.create(png_width, png_height, eColorType::RGB);

    size_t* pi = png_index;

    double udist = uMax - uMin;
    double vdist = vMax - vMin;

    for (size_t r = 0; r < png_height; ++r)
    {
        for (size_t c = 0; c < png_width; ++c)
        {
            size_t pos = *pi;
            pi++;
            if (pos == 0)
            {
                continue;
            }
            double h = 0., qu = 0., qv = 0.;

            h = Hdata[pos];
            pi++;
            if (h > G_TINY_DEPTH)
            {
                qu = qx[pos] * G_SCALE / (h * cellsize);
                qv = qy[pos] * G_SCALE / (h * cellsize);
                {  // qx qy
                    suColor pc;
                    pc.red = G_MAX_V * (qu - uMin) / (uMax - uMin);
                    pc.green = G_MAX_V * (qv - vMin) / (vMax - vMin);
                    pdXY.pixel(r, c, pc);
                }
                {
                    // depth
                    suColor pc;
                    pc.red = static_cast<uint8_t>(G_MAX_V * h / hMax);
                    pdH.pixel(r, c, pc);
                }
            }
        }
    }
    int iRow = static_cast<int>(png_height);
    int iCol = static_cast<int>(png_width);
    int ihMax = static_cast<int>(hMax);
    int ihMin = static_cast<int>(hMin);
    int iUMax = static_cast<int>(uMax);
    int iVMax = static_cast<int>(vMax);
    int iUMin = static_cast<int>(uMin);
    int iVMin = static_cast<int>(vMin);
    int HEAD_SIZE = 28;
    {
        std::string hPNGData;
        hPNGData.resize(HEAD_SIZE);

        char* hpd = hPNGData.data();
        memcpy(hpd, &G_FLAG, sizeof(G_FLAG));
        hpd += sizeof(G_FLAG);
        memcpy(hpd, &iRow, sizeof(iRow));
        hpd += sizeof(iRow);
        memcpy(hpd, &iCol, sizeof(iCol));
        hpd += sizeof(iCol);
        memcpy(hpd, &ihMax, sizeof(ihMax));
        hpd += sizeof(ihMax);
        memcpy(hpd, &ihMin, sizeof(ihMin));
        hpd += sizeof(ihMin);
        std::string hPNG = pdH.encode();
        hPNGData.append(hPNG);
        sufile::write(std::string(img_path), hPNGData);
        pdH.release();
    }
    {
        std::string filename = std::filesystem::path(img_path).filename().string();
        filename[0] = 'Q';
        std::string pd2_path = std::filesystem::path(img_path).parent_path().append(filename).string();
        std::string qPNGData;

        qPNGData.resize(HEAD_SIZE);
        char* qpd = qPNGData.data();
        memcpy(qpd, &G_FLAG, sizeof(G_FLAG));
        qpd += sizeof(G_FLAG);
        memcpy(qpd, &iRow, sizeof(iRow));
        qpd += sizeof(iRow);
        memcpy(qpd, &iCol, sizeof(iCol));
        qpd += sizeof(iCol);
        memcpy(qpd, &iUMax, sizeof(iUMax));
        qpd += sizeof(iUMax);
        memcpy(qpd, &iUMin, sizeof(iUMin));
        qpd += sizeof(iUMin);
        memcpy(qpd, &iVMax, sizeof(iVMax));
        qpd += sizeof(iVMax);
        memcpy(qpd, &iVMin, sizeof(iVMin));
        qpd += sizeof(iVMin);
        std::string qPNG = pdXY.encode();
        qPNGData.append(qPNG);
        sufile::write(pd2_path, qPNGData);
        pdXY.release();
    }
}

void convert_hq(double ncols, double nrows, double xllcorner, double yllcorner, double cellsize, double mid, double* hData, double* qxData, double* qyData, char* img_path, bool verbose)
{
    if (!hData || !qxData || !qyData)
    {
        return;
    }

    auto [hMin, hMax, uMin, uMax, vMin, vMax] = summary_minmax(ncols, nrows, cellsize, hData, qxData, qyData);
    suPNG pdA;
    pdA.create(png_width, png_height, eColorType::RGB);
    pdA.compress_level(AG_COMPRESS_LEVEL);
    size_t* pi = png_index;

    double udist = uMax - uMin;
    double vdist = vMax - vMin;
    for (size_t r = 0; r < png_height; ++r)
    {
        for (size_t c = 0; c < png_width; ++c)
        {
            size_t pos = *pi;
            pi++;
            if (pos == 0)
            {
                continue;
            }
            double h = 0., qu = 0., qv = 0.;
            h = hData[pos];

            if (h > G_TINY_DEPTH)
            {
                qu = qxData[pos] * G_SCALE / (h * cellsize);
                qv = qyData[pos] * G_SCALE / (h * cellsize);
                suColor cXYH;
                //
                cXYH.red = static_cast<unsigned char>(G_MAX_V * h / hMax);
                cXYH.green = static_cast<unsigned char>(std::round(G_MAX_V * (qu - uMin) / udist));
                cXYH.blue = static_cast<unsigned char>(std::round(G_MAX_V * (qv - vMin) / vdist));
                pdA.pixel(r, c, cXYH);
            }
        }
    }
    std::string hqPNGData;
    const size_t HEAD_SIZE = 32 + 4;
    int iRow = static_cast<int>(png_height);
    int iCol = static_cast<int>(png_width);
    int ihMax = static_cast<int>(hMax);
    int ihMin = static_cast<int>(hMin);
    int iUMax = static_cast<int>(uMax);
    int iVMax = static_cast<int>(vMax);
    int iUMin = static_cast<int>(uMin);
    int iVMin = static_cast<int>(vMin);
    {
        // 构建前缀信息
        hqPNGData.resize(HEAD_SIZE);
        char* hqp = hqPNGData.data();
        memcpy(hqp, &G_FLAG, sizeof(G_FLAG));
        hqp += sizeof(G_FLAG);
        memcpy(hqp, &iRow, sizeof(iRow));
        hqp += sizeof(iRow);
        memcpy(hqp, &iCol, sizeof(iCol));
        hqp += sizeof(iCol);
        memcpy(hqp, &ihMax, sizeof(ihMax));
        hqp += sizeof(ihMax);
        memcpy(hqp, &ihMin, sizeof(ihMin));
        hqp += sizeof(ihMin);
        memcpy(hqp, &iUMax, sizeof(iUMax));
        hqp += sizeof(iUMax);
        memcpy(hqp, &iUMin, sizeof(iUMin));
        hqp += sizeof(iUMin);
        memcpy(hqp, &iVMax, sizeof(iVMax));
        hqp += sizeof(iVMax);
        memcpy(hqp, &iVMin, sizeof(iVMin));
    }
    std::string hqPNG = pdA.encode();
    hqPNGData.append(hqPNG);
    sufile::write(std::string(img_path), hqPNGData);
    pdA.release();
}

unsigned char* get_image_data(double ncols, double nrows, double xllcorner, double yllcorner, double cellsize, double mid, double* hData, double* qxData, double* qyData, int* length, bool verbose)
{
    *length = 0;
    if (!hData || !qxData || !qyData)
    {
        return nullptr;
    }

    auto [hMin, hMax, uMin, uMax, vMin, vMax] = summary_minmax(ncols, nrows, cellsize, hData, qxData, qyData);
    suPNG pdA;
    pdA.create(png_width, png_height, eColorType::RGB);
    pdA.compress_level(AG_COMPRESS_LEVEL);
    size_t* pi = png_index;

    double udist = uMax - uMin;
    double vdist = vMax - vMin;
    for (size_t r = 0; r < png_height; ++r)
    {
        for (size_t c = 0; c < png_width; ++c)
        {
            size_t pos = *pi;
            pi++;
            if (pos == 0)
            {
                continue;
            }
            double h = 0., qu = 0., qv = 0.;
            h = hData[pos];

            if (h > G_TINY_DEPTH)
            {
                qu = qxData[pos] * G_SCALE / (h * cellsize);
                qv = qyData[pos] * G_SCALE / (h * cellsize);
                suColor cXYH;
                //
                cXYH.red = static_cast<unsigned char>(G_MAX_V * h / hMax);
                cXYH.green = static_cast<unsigned char>(std::round(G_MAX_V * (qu - uMin) / udist));
                cXYH.blue = static_cast<unsigned char>(std::round(G_MAX_V * (qv - vMin) / vdist));
                pdA.pixel(r, c, cXYH);
            }
        }
    }
    std::string hqPNGData;
    const size_t HEAD_SIZE = 32 + 4;
    int iRow = static_cast<int>(png_height);
    int iCol = static_cast<int>(png_width);
    int ihMax = static_cast<int>(hMax);
    int ihMin = static_cast<int>(hMin);
    int iUMax = static_cast<int>(uMax);
    int iVMax = static_cast<int>(vMax);
    int iUMin = static_cast<int>(uMin);
    int iVMin = static_cast<int>(vMin);
    {
        // 构建前缀信息
        hqPNGData.resize(HEAD_SIZE);
        char* hqp = hqPNGData.data();
        memcpy(hqp, &G_FLAG, sizeof(G_FLAG));
        hqp += sizeof(G_FLAG);
        memcpy(hqp, &iRow, sizeof(iRow));
        hqp += sizeof(iRow);
        memcpy(hqp, &iCol, sizeof(iCol));
        hqp += sizeof(iCol);
        memcpy(hqp, &ihMax, sizeof(ihMax));
        hqp += sizeof(ihMax);
        memcpy(hqp, &ihMin, sizeof(ihMin));
        hqp += sizeof(ihMin);
        memcpy(hqp, &iUMax, sizeof(iUMax));
        hqp += sizeof(iUMax);
        memcpy(hqp, &iUMin, sizeof(iUMin));
        hqp += sizeof(iUMin);
        memcpy(hqp, &iVMax, sizeof(iVMax));
        hqp += sizeof(iVMax);
        memcpy(hqp, &iVMin, sizeof(iVMin));
    }
    std::string hqPNG = pdA.encode();
    pdA.release();
    if (!hqPNG.empty())
    {
        *length = static_cast<int>(hqPNG.size() + HEAD_SIZE);
        unsigned char* tmp = (unsigned char*)malloc(*length);
        if (tmp)
        {
            memcpy(tmp, hqPNGData.data(), HEAD_SIZE);
            memcpy(tmp + HEAD_SIZE, hqPNG.data(), hqPNG.size());
            png_ptr = tmp;
        }
        else
        {
            SU_ERROR_PRINT("malloc error")
            *length = 0;
            png_ptr = nullptr;
        }
    }
    return (unsigned char*)png_ptr;
}
void free_image_data()
{
    if (png_ptr)
    {
        free(png_ptr);
        png_ptr = nullptr;
    }
}

void free_map()
{
    if (png_index)
    {
        delete[] png_index;
        png_index = nullptr;
    }
}
