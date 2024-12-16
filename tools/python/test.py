#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
@copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights reserved.
            北京天智祥信息科技有限公司版权所有
@website: http://www.tianzhixiang.com.cn/
@author: dou li yang
@date: 2024-12-11
@file: test.py
@description: test实现
@version: v1.0.1 2024-12-11 dou li yang
"""

import math

import csv


# DMS to DD conversion function
def dms_to_dd(dms):
    parts = dms.replace('°', ' ').replace("'", ' ').replace('"', '').split()
    degrees = float(parts[0])
    minutes = float(parts[1])
    seconds = float(parts[2])
    dd = degrees + minutes / 60 + seconds / 3600
    return dd


# Input data
data = [
    ["34°16'09.79882\"", "111°25'55.99181\"", 439.19],
    ["34°16'14.47674\"", "111°25'56.35512\"", 438.439],
    ["34°16'14.60581\"", "111°25'56.41448\"", 438.484],
    ["34°15'37.52717\"", "111°25'36.81602\"", 459.637],
    ["34°15'37.12203\"", "111°25'36.94451\"", 458.284],
    ["34°15'36.31125\"", "111°25'36.99039\"", 461.018],
    ["34°14'47.48998\"", "111°25'28.31334\"", 495.027],
    ["34°14'47.07089\"", "111°25'30.15844\"", 494.877],
    ["34°14'31.08926\"", "111°25'34.06462\"", 502.868],
    ["34°14'29.98506\"", "111°25'35.43157\"", 503.789],
    ["34°14'30.08042\"", "111°25'33.60321\"", 502.216],
    ["34°14'13.07024\"", "111°25'52.67467\"", 521.897],
    ["34°14'12.91455\"", "111°25'52.53755\"", 521.578],
    ["34°14'12.50369\"", "111°25'54.01729\"", 523.001],
    ["34°14'11.92574\"", "111°25'54.99459\"", 522.19],
    ["34°14'11.27400\"", "111°25'55.69780\"", 519.917],
    ["34°14'11.64305\"", "111°25'55.09844\"", 519.037]
]

# Convert and write to CSV
with open('converted_coordinates.csv', mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['纬度', '经度', '84H'])  # Writing headers
    for row in data:
        latitude_dd = dms_to_dd(row[0])
        longitude_dd = dms_to_dd(row[1])
        h84 = row[2]
        writer.writerow([latitude_dd, longitude_dd, h84])

print("Conversion complete. Data saved to converted_coordinates.csv")

from PIL import Image


def latlon_to_pixel(lat, lon, image_bounds, image_size):
    """
    将纬度/经度坐标转换为PGM图像中的像素坐标。

    参数:
        lat (float): 纬度
        lon (float): 经度
        image_bounds (tuple): 图像覆盖的地理边界 (min_lat, min_lon, max_lat, max_lon)
        image_size (tuple): 图像大小 (width, height)

    返回:
        tuple: (x, y) 像素坐标
    """
    min_lat, min_lon, max_lat, max_lon = image_bounds
    width, height = image_size

    # 计算经度到X轴像素坐标的比例
    lon_ratio = (lon - min_lon) / (max_lon - min_lon)
    x = int(lon_ratio * width)

    # 计算纬度到Y轴像素坐标的比例
    lat_ratio = (max_lat - lat) / (max_lat - min_lat)
    y = int(lat_ratio * height)

    return (x, y)


def get_elevation_from_pgm(pgm_file_path, lat, lon, image_bounds, image_size):
    """
    从PGM文件中获取指定经纬度坐标的高程值。

    参数:
        pgm_file_path (str): PGM文件路径
        lat (float): 纬度
        lon (float): 经度
        image_bounds (tuple): 图像覆盖的地理边界 (min_lat, min_lon, max_lat, max_lon)
        image_size (tuple): 图像大小 (width, height)

    返回:
        float: 高程值
    """
    # 打开PGM图像
    with Image.open(pgm_file_path) as img:
        # 转换经纬度为像素坐标
        pixel_coords = latlon_to_pixel(lat, lon, image_bounds, image_size)

        # 获取灰度值作为高程
        elevation = img.getpixel(pixel_coords)

        return elevation


# 示例调用
if __name__ == "__main__":
    pgm_file_path = 'Z:/egm2008-1.pgm'  # 替换为你的PGM文件路径
    lat = 34.2694  # 示例纬度
    lon = 111.4322  # 示例经度
    image_bounds = (34.0, 111.0, 35.0, 112.0)  # 替换为PGM文件的实际地理边界
    image_size = (1000, 1000)  # 替换为PGM文件的实际尺寸

    elevation = get_elevation_from_pgm(pgm_file_path, lat, lon, image_bounds, image_size)
    print(f"The elevation at ({lat}, {lon}) is {elevation}")
