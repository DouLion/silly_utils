//
// Created by dell on 2023/11/17.
//

#include "silly_geotiff.h"
#include <geotiffio.h>
#include <xtiffio.h>
#include <geo_tiffp.h>
bool suGeoTiff::write(const std::filesystem::path& outfile)
{
    // 同步宽度/高度 (防护不一致)
    info.width = raster.col();
    info.height = raster.row();
    m_header.width = info.width;
    m_header.height = info.height;

    TIFF* tif = XTIFFOpen(sufile::realpath(outfile).string().c_str(), "w");  // 假设sufile是自定义
    if (!tif)
    {
        m_err = "Failed to create output file";
        return false;
    }

    // 1. 设置必需TIFF标签 (加返回值检查)
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, info.width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, info.height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, m_header.bits_per_sample);  // double类型使用64位
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);        // IEEE浮点格式
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, m_header.samples_per_pixel);

    // TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZMA);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    // 2. 设置地理信息
    double tiepoints[6] = {0, 0, 0, 0, 0, 0};
    tiepoints[3] = info.bound.min.x;  // 左上角绑定
    tiepoints[4] = info.bound.max.y;

    double pixscale[3] = {0, 0, 0};
    pixscale[0] = (info.bound.max.x - info.bound.min.x) / m_header.width;
    pixscale[1] = -(info.bound.max.y - info.bound.min.y) / m_header.height;
    TIFFSetField(tif, GTIFF_TIEPOINTS, 6, tiepoints);
    TIFFSetField(tif, GTIFF_PIXELSCALE, 3, pixscale);

    // 3. 写入GeoTIFF坐标系
    GTIF* gtif = GTIFNew(tif);
    /*GTIFKeySet(gtif, ModelPixelScaleTag, TYPE_DOUBLE, 3, pixscale);
    GTIFKeySet(gtif, ModelTiepointTag, TYPE_DOUBLE, 6, tiepoints);*/
    // 设置WGS84地理坐标系
    GTIFKeySet(gtif, GTModelTypeGeoKey, TYPE_SHORT, 1, ModelTypeGeographic);
    GTIFKeySet(gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1, RasterPixelIsArea);
    GTIFKeySet(gtif, GeographicTypeGeoKey, TYPE_SHORT, 1, GCS_WGS_84);
    GTIFKeySet(gtif, GeogCitationGeoKey, TYPE_ASCII, 0, "WGS 84");
    GTIFKeySet(gtif, GeogAngularUnitsGeoKey, TYPE_SHORT, 1, Angular_Degree);

    char* p = (char*)raster.data();
    for (uint32 row = 0; row < info.height; ++row)
    {
        void* offP = p + row * info.width * sizeof(double);
        if (TIFFWriteScanline(tif, offP, row, 0) < 0)
        {
            std::cerr << "写入扫描线失败: " << row << std::endl;
            GTIFFree(gtif);
            XTIFFClose(tif);
            return false;
        }
    }

    // 写入GeoTIFF标签
    GTIFWriteKeys(gtif);
    GTIFFree(gtif);
    XTIFFClose(tif);
    return true;
}
std::string suGeoTiff::err() const
{
    return m_err;
}