//
// Created by dell on 2024/7/19.
//

#ifndef ASCII_GRID_IMAGE_H
#define ASCII_GRID_IMAGE_H


#if IS_WIN32 || IS_WIN64
#define TZX_DLL_EXPORT __declspec(dllexport)
#else
#include <tr1/memory>
// linux导出dll不需要声明
#define TZX_DLL_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif
    TZX_DLL_EXPORT void convert_image(double ncols, double nrows, double xllcorner, double yllcorner, double cellsize, double mid, double* data, double* qx, double* qy, char* img_path);

TZX_DLL_EXPORT void convert_image_test(double ncols, double nrows);

#ifdef __cplusplus
}
#endif


#endif //ASCII_GRID_IMAGE_H
