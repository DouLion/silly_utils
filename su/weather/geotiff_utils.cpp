//
// Created by dly on 2023/7/10.
//

#include  "geotiff_utils.h"


tif_data geotiff_utils::readGeoTiff(std::string filePath)
{
    tif_data res_tif;
    bool status = true;
    // 打开影像数据
    TIFF* tiff = XTIFFOpen(filePath.c_str(),  "r ");
    if (tiff == nullptr) 
    {
        std::cout <<  "打开影像数据失败 " << std::endl;
        return res_tif;
    }

    // 获取影像属性信息
    GTIF* gtif = GTIFNew(tiff);
    if (gtif == nullptr) {
        std::cout << "获取影像属性信息失败 " << std::endl;
        XTIFFClose(tiff);
        status = false;
    }
    GTIFPrint(gtif, 0, 0); // 打印属性信息

    //// 获取图像起始点的投影坐标
    //GTIFImageToPCS(gtif, &res_tif.tif_letf, &res_tif.tif_top);
    //std::cout << "tif_letf: " << res_tif.tif_letf << std::endl;
    //std::cout << "tif_top: " << res_tif.tif_top << std::endl;

    // 读取图像基本信息
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &res_tif.tif_width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &res_tif.tif_height);
    std::cout <<  "宽: " << res_tif.tif_width << std::endl;
    std::cout <<  "高: " << res_tif.tif_height << std::endl;

    std::cout << "GTIFF_TIEPOINTS 地理坐标 " << std::endl;
    double* tif_coordinate = NULL;
    int16_t tif_coordinate_count;
    TIFFGetField(tiff, GTIFF_TIEPOINTS, &tif_coordinate_count, &tif_coordinate);
    if (tif_coordinate_count > 4)
    {
        res_tif.tif_letf = tif_coordinate[3];
        res_tif.tif_top = tif_coordinate[4];
        std::cout << "tif_letf: " << res_tif.tif_letf << std::endl;
        std::cout << "tif_top: " << res_tif.tif_top << std::endl;
    }
    else
    {
        std::cout << "获取地理坐标失败 " << std::endl;
        return res_tif;
    }


    std::cout << "GTIFF_PIXELSCALE 像素分辨率 " << std::endl;
    double* pixel_scale = NULL;
    int16_t pixel_scale_count;
    TIFFGetField(tiff, GTIFF_PIXELSCALE, &pixel_scale_count, &pixel_scale);
    if (pixel_scale_count > 2)
    {
        res_tif.pixelSizeX = pixel_scale[0];
        res_tif.pixelSizeY = pixel_scale[1];
        std::cout << "pixelSizeX: " << res_tif.pixelSizeX << std::endl;
        std::cout << "pixelSizeY: " << res_tif.pixelSizeY << std::endl;
    }
    else
    {
        std::cout << "像素分辨率失败 " << std::endl;
        return res_tif;
    }

    // 每个点占的位数
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &res_tif.tif_bitsPerSample);
    std::cout <<  "每个点占的位数:  " << res_tif.tif_bitsPerSample << std::endl;

    //// 获取分辨率单位
    //uint16_t resolutionUnit;
    //TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &resolutionUnit);
    //std::cout <<  "分辨率单位:  " << resolutionUnit << std::endl;

    // 获取每个像素的样本数
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &res_tif.tif_samplesPerPixel);
    std::cout <<  "每个像素的样本数:  " << res_tif.tif_samplesPerPixel << std::endl;


    //----------------------------------------------------
    // 
    // 数据类型 :TIFFTAG_SAMPLEFORMAT
    if (TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &res_tif.tif_sampleFormat))
    {
        if (res_tif.tif_sampleFormat == SAMPLEFORMAT_UINT)
        {
            std::cout <<  "数据类型：uint " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_INT)
        {
            std::cout <<  "数据类型：int " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_IEEEFP)
        {
            std::cout <<  "数据类型：float " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_VOID)
        {
            std::cout <<  "数据类型：未知数据类型 " << std::endl;
        }
        else 
        {
            // 其他未知数据类型
            std::cout <<  "未知的数据类型 " << std::endl;
        }
    }
    else 
    {
        std::cout <<  "无法获取数据类型 " << std::endl;
        return res_tif;
    }

    // 图像的方向标签
    TIFFGetField(tiff, TIFFTAG_ORIENTATION, &res_tif.tif_orientation);
    std::cout <<  "图像的方向标签:  " << res_tif.tif_orientation << std::endl;

    // 图像的平面配置标签
    TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &res_tif.tif_planarConfig);
    std::cout <<  "图像的平面配置标签:  " << res_tif.tif_planarConfig << std::endl;

    // 图像的光度标签
    uint16_t photometric;
    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &res_tif.tif_photometric);
    std::cout <<  "图像的光度标签:  " << res_tif.tif_photometric << std::endl;


    // 逐行读取像素数据
    res_tif.tif_matrix2.create(res_tif.tif_height , res_tif.tif_width);
    for (uint32_t row = 0; row < res_tif.tif_height; ++row)
    {
        TIFFReadScanline(tiff, &(res_tif.tif_matrix2.at(row, 0)), row);
    }
    std::cout << "******** " << static_cast<float>(res_tif.tif_matrix2.at(1, 1)) << std::endl;


    // 关闭影像数据和GTIF对象
    XTIFFClose(tiff);
    GTIFFree(gtif);
    return res_tif;
}


bool geotiff_utils::writeGeoTiff(std::string filePath, tif_data tif_matrix2)
{
    bool status = true;
    // 获取矩阵的行数和列数
    size_t cols = tif_matrix2.tif_width;
    size_t rows = tif_matrix2.tif_height;

    // 打开 TIFF 文件进行写入
    TIFF* tiff = TIFFOpen(filePath.c_str(),  "w ");
    if (!tiff) {
        std::cerr <<  "无法打开 TIFF 文件进行写入 " << std::endl;
        status = false;
    }

    // 设置 TIFF 图像的基本信息
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, cols);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, rows);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, tif_matrix2.tif_samplesPerPixel);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, tif_matrix2.tif_bitsPerSample);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, tif_matrix2.tif_sampleFormat);
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);       //图像的方向标签
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);      // 图像的平面配置标签
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);    // 图像的光度标签


    double pixelScale[3] = { tif_matrix2.pixelSizeX, tif_matrix2.pixelSizeY, 1.0 };
    double modelTransform[6] = { 0.0, 0.0, 0.0,tif_matrix2.tif_letf , tif_matrix2.tif_top, 0.0 };

    TIFFSetField(tiff, GTIFF_PIXELSCALE,3, &pixelScale);
    TIFFSetField(tiff, GTIFF_TIEPOINTS, 6, &modelTransform);

    // 写入像素数据
    for (size_t row = 0; row < rows; ++row) {
        TIFFWriteScanline(tiff, tif_matrix2.tif_matrix2.get_data()[row], row);
    }

    // 关闭 TIFF 文件
    TIFFClose(tiff);

    std::cout <<  "TIFF 文件写入成功 " << std::endl;
    return status;

}




