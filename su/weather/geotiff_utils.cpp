//
// Created by dly on 2023/7/10.
//

#include  "geotiff_utils.h"

bool geotiff_utils::get_tif_data(TIFF* tiff, tif_data& res_tif)
{
    // 读取图像基本信息
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &res_tif.tif_width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &res_tif.tif_height);
    std::cout << "宽: " << res_tif.tif_width << std::endl;
    std::cout << "高: " << res_tif.tif_height << std::endl;

    std::cout << "GTIFF_TIEPOINTS 地理坐标 " << std::endl;
    double* tif_coordinate = NULL;
    uint32_t tif_coordinate_count;
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
        return false;
    }


    std::cout << "GTIFF_PIXELSCALE 像素分辨率 " << std::endl;
    double* pixel_scale = NULL;
    uint32_t pixel_scale_count;
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
        return false;
    }

    // 每个点占的位数
    TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &res_tif.tif_bitsPerSample);
    std::cout << "每个点占的位数:  " << res_tif.tif_bitsPerSample << std::endl;

    //// 获取分辨率单位
    //uint16_t resolutionUnit;
    //TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &resolutionUnit);
    //std::cout <<  "分辨率单位:  " << resolutionUnit << std::endl;

    // 获取每个像素的样本数
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &res_tif.tif_samplesPerPixel);
    std::cout << "每个像素的样本数:  " << res_tif.tif_samplesPerPixel << std::endl;

    //----------------------------------------------------
    // 
    // 数据类型 :TIFFTAG_SAMPLEFORMAT
    if (TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &res_tif.tif_sampleFormat))
    {
        if (res_tif.tif_sampleFormat == SAMPLEFORMAT_UINT)
        {
            std::cout << "数据类型：uint " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_INT)
        {
            std::cout << "数据类型：int " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_IEEEFP)
        {
            std::cout << "数据类型：float " << std::endl;
        }
        else if (res_tif.tif_sampleFormat == SAMPLEFORMAT_VOID)
        {
            std::cout << "数据类型：未知数据类型 " << std::endl;
        }
        else
        {
            // 其他未知数据类型
            std::cout << "未知的数据类型 " << std::endl;
        }
    }
    else
    {
        std::cout << "无法获取数据类型 " << std::endl;
        return false;
    }

    // 图像的方向标签
    TIFFGetField(tiff, TIFFTAG_ORIENTATION, &res_tif.tif_orientation);
    std::cout << "图像的方向标签:  " << res_tif.tif_orientation << std::endl;

    // 图像的平面配置标签 planar_config
    TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &res_tif.tif_planarConfig);
    std::cout << "图像的平面配置标签:  " << res_tif.tif_planarConfig << std::endl;

    // 图像的光度标签
    uint16_t photometric;
    TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &res_tif.tif_photometric);
    std::cout << "图像的光度标签:  " << res_tif.tif_photometric << std::endl;
    //----------------------------------

    // 通道数
    TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &res_tif.tif_numChannels);


    res_tif.tif_lineSize = TIFFScanlineSize64(tiff); // 一行多少字节
    res_tif.tif_tileSize = TIFFTileSize64(tiff);

    uint32 nstrips = TIFFNumberOfStrips(tiff); // Number of strips in file
    uint32 rowsperstrip = 0;
    int rc_1 = TIFFGetField(tiff, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
    int rc_2 = TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &res_tif.tif_tileWidth);
    int rc_3 = TIFFGetField(tiff, TIFFTAG_TILELENGTH, &res_tif.tif_tileHeight);


    //TIFFIsTiled(tiff) ? printf("图像存储类型: Tiled\n") : printf("图像存储类型: Strip\n");
    //TIFFIsTiled(tiff) ? (res_tif.tif_type = TILE_TIF) : (res_tif.tif_type = STRIP_TIF);
    // Determine TIFF type
    if (rc_2 && rc_3 && res_tif.tif_tileWidth > 1 && res_tif.tif_tileHeight > 1 && TIFFIsTiled(tiff))
    {
        uint32 tiledepth;
        int rc_4 = TIFFGetField(tiff, TIFFTAG_TILEDEPTH, &tiledepth);
        if (res_tif.tif_tileWidth % 16 == 0 && res_tif.tif_tileHeight % 16 == 0)
        {
            res_tif.tif_type = TILE_TIF;
        }
    }
    else if (rc_1 && nstrips >= 1 && rowsperstrip >= 1)
    {
        res_tif.tif_type = STRIP_TIF;
    }
    else if (res_tif.tif_lineSize > 0)
    {
        res_tif.tif_type = SCANLINE_TIF;
    }

    uint64_t tileSize = TIFFVTileSize64(tiff, res_tif.tif_height);
    uint64_t rowstileSize = TIFFTileRowSize64(tiff);
    int a = 0;


    return true;

    //// 获取图像的存储类型
    //int tileOrStrip = -1;
    //if (TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &tileOrStrip)) {
    //    std::cout << "图像存储类型: Tiled\n"<< std::endl;
    //    std::cout << "瓦片宽度: %d\n"<< tileOrStrip<< std::endl;
    //    if (TIFFGetField(tiff, TIFFTAG_TILELENGTH, &tileOrStrip)) {
    //        std::cout << "瓦片长度: %d" << tileOrStrip << std::endl;
    //    }
    //    else {
    //        // 处理获取瓦片长度失败的情况
    //    }
    //}
    //else {
    //    std::cout << "图像存储类型: Strip"<< std::endl;
    //    if (TIFFGetField(tiff, TIFFTAG_ROWSPERSTRIP, &tileOrStrip)) {
    //        std::cout << "行数每条带: %d"<<tileOrStrip << std::endl;
    //    }
    //    else {
    //        // 处理获取行数每条带失败的情况
    //    }
    //}

}

tif_data geotiff_utils::readGeoTiff(std::string filePath)
{
    tif_data res_tif;
    bool status = true;
    // 打开影像数据
    TIFF* tiff = XTIFFOpen(filePath.c_str(), "r ");
    if (tiff == nullptr)
    {
        std::cout << "打开影像数据失败 " << std::endl;
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
    GTIFFree(gtif);

    // 获取tif的参数属性
    bool get_data = geotiff_utils::get_tif_data(tiff, res_tif);
    if (!get_data)
    {
        // 关闭影像数据和GTIF对象
        XTIFFClose(tiff);
        return res_tif;
    }

    switch (res_tif.tif_type)
    {
    case STRIP_TIF:
        // 逐行读取像素数据
        res_tif.tif_matrix2.create(res_tif.tif_height, res_tif.tif_width);
        for (uint32_t row = 0; row < res_tif.tif_height; ++row)
        {
            TIFFReadScanline(tiff, res_tif.tif_matrix2.get_data()[row], row);
        }
        break;
    case TILE_TIF:
        if (res_tif.tif_lineSize > 0)
        {
            res_tif.tif_matrix2.create(res_tif.tif_height, res_tif.tif_width);
            // 读取图像数据
            //void* line_buf = _TIFFmalloc(res_tif.tif_lineSize);
            unsigned char* line_buf = (unsigned char*)_TIFFmalloc(res_tif.tif_lineSize);
            unsigned char* tilebuf = (unsigned char*)_TIFFmalloc(res_tif.tif_tileSize);
            //std::vector<std::vector<float>> res(res_tif.tif_height, std::vector<float>(res_tif.tif_width));
            for (uint32_t row = 0; row < res_tif.tif_height; row++)
            {
                for (uint32_t col = 0, buf_col = 0; col < res_tif.tif_width; col += res_tif.tif_tileWidth)
                {
                    uint64 bytes_read = TIFFReadTile(tiff, tilebuf, col, row, 0, 1);
                    //-----------------------------------
                    uint32 num_preceding_tile_rows = floor(row / res_tif.tif_tileHeight);
                    uint32 row_in_tile = row - (num_preceding_tile_rows * res_tif.tif_tileHeight);
                    uint32 idx = 0;
                    for (uint32 i = 0; i < res_tif.tif_tileWidth && buf_col < res_tif.tif_width && (idx * res_tif.tif_samplesPerPixel) < res_tif.tif_tileSize; i++)
                    {
                        if (res_tif.tif_planarConfig == PLANARCONFIG_SEPARATE) {
                            idx = row_in_tile * res_tif.tif_tileWidth + i;
                        }
                        else {
                            // PLANARCONFIG_CONTIG
                            idx = row_in_tile * (res_tif.tif_tileWidth * res_tif.tif_samplesPerPixel) + i * res_tif.tif_samplesPerPixel + res_tif.tif_numChannels;
                        }
                        switch (res_tif.tif_bitsPerSample) {
                        case 8:
                            switch (res_tif.tif_sampleFormat) {
                            case SAMPLEFORMAT_UINT:
                                ((uint8*)line_buf)[buf_col] = ((uint8*)tilebuf)[idx];
                                buf_col++;
                                break;
                            case SAMPLEFORMAT_INT:
                                ((int8*)line_buf)[buf_col] = ((int8*)tilebuf)[idx];
                                buf_col++;
                                break;
                            default:
                                printf("Unexpected data type in TIFF file\n");
                                break;
                            }
                            break;
                        case 16:
                            switch (res_tif.tif_sampleFormat) {
                            case SAMPLEFORMAT_UINT:
                                ((uint16*)line_buf)[buf_col] = ((uint16*)tilebuf)[idx];
                                buf_col++;
                                break;
                            case SAMPLEFORMAT_INT:
                                ((int16*)line_buf)[buf_col] = ((int16*)tilebuf)[idx];
                                buf_col++;
                                break;
                            default:
                                printf("Unexpected data type in TIFF file\n");
                                break;
                            }
                            break;
                        case 32:
                            switch (res_tif.tif_sampleFormat) {
                            case SAMPLEFORMAT_UINT:
                                ((uint32*)line_buf)[buf_col] = ((uint32*)tilebuf)[idx];
                                buf_col++;
                                break;
                            case SAMPLEFORMAT_INT:
                                ((int32*)line_buf)[buf_col] = ((int32*)tilebuf)[idx];
                                buf_col++;
                                break;
                            case SAMPLEFORMAT_IEEEFP:
                                ((float*)line_buf)[buf_col] = ((float*)tilebuf)[idx];
                                buf_col++;
                                break;
                            default:
                                printf("Unexpected data type in TIFF file\n");
                                break;
                            }
                            break;
                        default:
                            printf("Usupported bits per sample found in TIFF file\n");
                            break;
                        }

                    }

                }
                memcpy(&(res_tif.tif_matrix2.at(row, 0)), line_buf, res_tif.tif_lineSize);
                // 读取一行
            }
            _TIFFfree(line_buf);
            _TIFFfree(tilebuf);
        }
        else
        {
            std::cout << "tif行数据为空 " << std::endl;
        }

        break;
    default:
        std::cout << "无法处理改类型的tif:  " << std::endl;
        break;
    }

    // 关闭影像数据和GTIF对象

    res_tif.tif_matrix2.destroy();
    XTIFFClose(tiff);
    return res_tif;
}


bool geotiff_utils::writeGeoTiff(std::string filePath, tif_data tif_matrix2)
{
    bool status = true;
    // 获取矩阵的行数和列数
    size_t cols = tif_matrix2.tif_width;
    size_t rows = tif_matrix2.tif_height;

    // 打开 TIFF 文件进行写入
    TIFF* tiff = XTIFFOpen(filePath.c_str(), "w ");
    if (!tiff) {
        std::cerr << "无法打开 TIFF 文件进行写入 " << std::endl;
        status = false;
    }

    // 设置 TIFF 图像的基本信息
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, cols);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, rows);
    TIFFSetField(tiff, TIFFTAG_TILEWIDTH, tif_matrix2.tif_tileWidth);
    TIFFSetField(tiff, TIFFTAG_TILELENGTH, tif_matrix2.tif_tileHeight);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, tif_matrix2.tif_samplesPerPixel);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, tif_matrix2.tif_bitsPerSample);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, tif_matrix2.tif_sampleFormat);
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, tif_matrix2.tif_orientation);       //图像的方向标签
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, tif_matrix2.tif_planarConfig);     //图像的平面配置标签 planar_config
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, tif_matrix2.tif_photometric);    // 图像的光度标签.


    double pixelScale[3] = { tif_matrix2.pixelSizeX, tif_matrix2.pixelSizeY, 1.0 };
    double modelTransform[6] = { 0.0, 0.0, 0.0,tif_matrix2.tif_letf , tif_matrix2.tif_top, 0.0 };

    TIFFSetField(tiff, GTIFF_PIXELSCALE, 3, &pixelScale);
    TIFFSetField(tiff, GTIFF_TIEPOINTS, 6, &modelTransform);

    // 写入像素数据
    for (size_t row = 0; row < rows; ++row) {
        TIFFWriteScanline(tiff, tif_matrix2.tif_matrix2.get_data()[row], row);
    }

    // 关闭 TIFF 文件
    TIFFClose(tiff);

    std::cout << "TIFF 文件写入成功 " << std::endl;
    return status;

}




bool geotiff_utils::writeGeoTiffTile(std::string filePath, tif_data tif_matrix2)
{
    bool status = true;
    // 获取矩阵的行数和列数
    size_t cols = tif_matrix2.tif_width;
    size_t rows = tif_matrix2.tif_height;

    // 打开 TIFF 文件进行写入
    TIFF* tiff = TIFFOpen(filePath.c_str(), "w ");
    if (!tiff) {
        std::cerr << "无法打开 TIFF 文件进行写入 " << std::endl;
        status = false;
    }

    // 设置 TIFF 图像的基本信息
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, cols);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, rows);
    TIFFSetField(tiff, TIFFTAG_TILEWIDTH, tif_matrix2.tif_tileWidth);
    TIFFSetField(tiff, TIFFTAG_TILELENGTH, tif_matrix2.tif_tileHeight);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, tif_matrix2.tif_samplesPerPixel);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, tif_matrix2.tif_bitsPerSample);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, tif_matrix2.tif_sampleFormat);
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, tif_matrix2.tif_orientation);       //图像的方向标签
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, tif_matrix2.tif_planarConfig);     //图像的平面配置标签 planar_config
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, tif_matrix2.tif_photometric);    // 图像的光度标签.


    double pixelScale[3] = { tif_matrix2.pixelSizeX, tif_matrix2.pixelSizeY, 1.0 };
    double modelTransform[6] = { 0.0, 0.0, 0.0,tif_matrix2.tif_letf , tif_matrix2.tif_top, 0.0 };

    TIFFSetField(tiff, GTIFF_PIXELSCALE, 3, &pixelScale);
    TIFFSetField(tiff, GTIFF_TIEPOINTS, 6, &modelTransform);

    // 写入像素数据
    // 将图像数据写入 TIFF 文件
    for (uint32_t row = 0; row < tif_matrix2.tif_height; row += tif_matrix2.tif_tileHeight) {
        for (uint32_t col = 0; col < tif_matrix2.tif_width; col += tif_matrix2.tif_tileWidth) {
            TIFFWriteTile(tiff, &(tif_matrix2.tif_matrix2.at(row, col)), col, row, 0, 0);
        }
    }

    // 关闭 TIFF 文件
    XTIFFClose(tiff);

    std::cout << "TIFF 文件写入成功 " << std::endl;
    return status;

}