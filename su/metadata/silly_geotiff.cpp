//
// Created by dell on 2023/11/17.
//

#include "silly_geotiff.h"
#include <geotiffio.h>
#include <xtiffio.h>
#include <geo_tiffp.h>
bool sugeotiff::write(const std::filesystem::path& outfile)
{
    // 同步宽度/高度 (防护不一致)
    m_width = m_data.col();
    m_height = m_data.row();
    m_header.width = m_width;
    m_header.height = m_height;

    TIFF* tif = XTIFFOpen(sufile::realpath(outfile).string().c_str(), "w");  // 假设sufile是自定义
    if (!tif)
    {
        m_err = "Failed to create output file";
        return false;
    }

    // 1. 设置必需TIFF标签 (加返回值检查)
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, m_width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, m_height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, m_header.bits_per_sample);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 64);                  // double类型使用64位
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);  // IEEE浮点格式
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    // 2. 设置地理信息
    double tiepoints[6] = {};
    tiepoints[3] = m_rect.min.x;  // 左上角绑定
    tiepoints[4] = m_rect.max.y;

    double pixscale[3] = {};
    pixscale[0] = (m_rect.max.x - m_rect.min.x) / m_header.width;
    pixscale[1] = -(m_rect.max.y - m_rect.min.y) / m_header.height;
    TIFFSetField(tif, GTIFF_TIEPOINTS, 6, tiepoints);
    TIFFSetField(tif, GTIFF_PIXELSCALE, 3, pixscale);

    // 3. 写入GeoTIFF坐标系
    GTIF* gtif = GTIFNew(tif);
    // 设置WGS84地理坐标系
    GTIFKeySet(gtif, GTModelTypeGeoKey, TYPE_SHORT, 1, ModelTypeGeographic);
    GTIFKeySet(gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1, RasterPixelIsArea);
    GTIFKeySet(gtif, GeographicTypeGeoKey, TYPE_SHORT, 1, KvUserDefined);
    GTIFKeySet(gtif, GeogCitationGeoKey, TYPE_ASCII, 0, "WGS 84");
    GTIFKeySet(gtif, GeogAngularUnitsGeoKey, TYPE_SHORT, 1, Angular_Degree);

    char* p = (char*)m_data.data();
    for (uint32 row = 0; row < m_height; ++row)
    {
        void* offP = p + row * m_width * sizeof(double);
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
std::string sugeotiff::err() const
{
    return m_err;
}