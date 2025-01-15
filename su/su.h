/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-15
 * @file: su.h
 * @description: 包含silly_utils库中所有的头文件
 * @version: v1.0.1 2025-01-15 dou li yang
 */
#ifndef SILLY_UTILS_SU_H
#define SILLY_UTILS_SU_H

///// 日志
//#include <log/silly_log.h>
//
///// json
//#include <json/silly_jsonpp.h> // json文件,json字符串.json对象处理
//
///// 数据库
//#include <database/otl/silly_otl.h>  // otl实现的odbc 数据库操作


// compress (压缩/解压缩)
#include <compress/silly_compress.h> // 压缩 错误码定义
#include <compress/silly_7z.h>	// 7z压缩/解压缩, 未实现
#include <compress/silly_bz2.h> // bz2 压缩/解压缩 (文件,内存数据)
#include <compress/silly_gzip.h> // gzip 压缩/解压缩(内存数据), 文件的压缩解压缩未实现
#include <compress/silly_minizip.h> // minizip 压缩/解压缩(文件,内存数据)
#include <compress/silly_rar.h>  // rar 压缩/解压缩(文件), 内存数据的压缩解压缩未实现
#include <compress/silly_zip.h>  // 未实现

// database (数据库)
#include <database/dm8/dm8_dpi.h> // dm8 数据库登录,登出,错误信息提示
#include <database/dm8/dm8_pro_c.h> // 未实现
#include <database/kb/kb_odbc.h> // 未实现
#include <database/otl/otlv4.h> // otl库源码
#include <database/otl/otl_tools.h> // 未实现
#include <database/otl/silly_otl.h> // otl实现数据库的增删改查模板函数
#include <database/redis/silly_redis_client.h> // 未实现
#include <database/redis/silly_redis_cpool.h> // 未实现
#include <database/silly_sql.h> // 操作不同数据库的SQL语句常量集合
#include <database/sqlite3/silly_sqlite3.h> // sqlite3数据库 打开,备份,关闭,错误信息提示

// datastruct (数据结构)
#include <datastruct/silly_safe_bimap.h> // 双向map模板类
#include <datastruct/silly_safe_deque.h> // 双向队列模板类
#include <datastruct/silly_safe_list.h> // list模板类
#include <datastruct/silly_safe_map.h> // map模板类
#include <datastruct/silly_safe_stack.h> // 栈模板类

// datetime (时间)
#include <datetime/silly_posix_time.h> // 统一时间为时间戳类型, 包含时间点的获取,时间戳和字符串的互转
#include <datetime/silly_timer.h> // 定时器

// encode (编码)
#include <encode/silly_crc.h> // CRC校验 (crc16 crc32 crc64)
#include <encode/silly_encode.h> // 编码转换, 字符串 编码检测 与 编码转换
#include <encode/silly_encrypt.h> // 加密解密 (md5, AES)
#include <encode/silly_pinyin.h> // 汉字转拼音

// files (文件)
#include <files/silly_file.h> // 文件读取(完整读取,读取指定字节,逐行读取,文件大小), 写入(完整写入,逐行写入), 获取/递归获取 目录下的所有文件, 文件最新更新时间, 文件删除,复制
#include <files/silly_file_lock.h> // 文件锁
#include <files/silly_memory_map.h> // 内存映射文件 (读取,写入)
#include <files/silly_schedule_file.h> // 定时生成文件, 结构确定内容未实现
#include <files/silly_xlsx.h> // xlsx文件操作 未实现

// geo (地理矢量)
#include <geo/gpc/gpc.h>  // gpc源码
#include <geo/proj/gdal/silly_projection_define.h> // 投影坐标系的 wtk
#include <geo/proj/gdal/silly_proj_convert.h> // 地理坐标系的转换
#include <geo/proj/silly_proj.h> // (高斯投影, 墨卡托投影)转经纬度坐标
#include <geo/silly_dem.h>  // dem文件读取 未实现
#include <geo/silly_geo.h>  // 自定义点线面多线多面矢量类型
#include <geo/silly_geojson.h> // geojson处理类, 未实现
#include <geo/silly_geo_coll.h> // 复合适量类型定义
#include <geo/silly_geo_const.h> // 地球基本信息常量定义: 平均半径, 赤道半径, 极半径, 长半轴, 扁率
#include <geo/silly_geo_convert.h> // 矢量数据转换: shp与geojson互转, 地理坐标系与墨卡托坐标系互转
#include <geo/silly_geo_prop.h> // 矢量数据的属性定义
#include <geo/silly_geo_utils.h> // 基于gdal的矢量数据工具类: 形心, 方向角, 面积, 矢量的读取写入, 矢量信息的加载, 矢量相交判断, 点线距离, 平滑线, 自定义矢量与gdal中的矢量互转, 矢量缓冲区
#include <geo/silly_iso_poly.h> // 等值面读取和写入
#include <geo/silly_map_layer.h> // 地图图层类
#include <geo/silly_polar.h> // 极坐标与经纬度坐标互转
#include <geo/silly_projection.h> // 经纬度坐标与墨卡托投影的互转
#include <geo/silly_pyramid.h> // 矢量金字塔:经纬度转金字塔层块号,瓦片经纬度范围, 屏幕坐标与经纬度坐标互转, 瓦片转屏幕范围
#include <geo/silly_raster_to_vector.h> // Marching squares 算法
#include <geo/silly_spatialite.h> // spatialite数据库操作: 创建, 删除,  矢量数据的读取写入, 查询, 修改
#include <geo/silly_vector_to_raster.h> // 矢量转栅格,扫描线算法
#include <geo/thiessen/jc_voronoi.h> // voronoi图
#include <geo/thiessen/jvc_struct.h> // jvc数据类型定义
#include <geo/thiessen/thiessen_creator.h> // 泰森多边形（Voronoi 图）
#include <geo/triangular/silly_delaunay.h> // 德劳内三角剖分的基础数据结构和工具类:从字符串中提取双精度浮点数, 从文件或字符串读取和解析点数据, 两点间距离
#include <geo/triangular/silly_delaunay_2d.h> // 未实现
#include <geo/triangular/silly_delaunay_3d.h> // 未实现
#include <geo/triangular/silly_delaunay_bowyer.h> // Bowyer-Watson 算法
#include <geo/triangular/silly_delaunay_divide.h> // 未实现
#include <geo/triangular/silly_delaunay_sweep_line.h> // 德劳内三角扫线算法 (点的扫描线算法)
#include <geo/triangular/silly_iso_line.h> // 等值线: 等值线的生成、插值、裁剪和绘制
#include <geo/vacuate/psimpl.h> // psimpl源码
#include <geo/vacuate/silly_vacuate.h> // 抽稀算法

// image
#include <image/font/silly_font.h>
#include <image/grid/silly_render.h>
#include <image/render/canvas/silly_cairo.h>
#include <image/render/pattern/cairo/silly_cairo_pattern.h>
#include <image/render/shape/silly_render_shapes.h>
#include <image/render/silly_grid_render.h>
#include <image/render/silly_render_brush.h>
#include <image/render/silly_render_pen.h>
#include <image/render/style/silly_render_style.h>
#include <image/render/symbol/silly_render_symbol.h>
#include <image/silly_bmp.h>
#include <image/silly_color.h>
#include <image/silly_gif.h>
#include <image/silly_icon.h>
#include <image/silly_image_base.h>
#include <image/silly_jpeg.h>
#include <image/silly_png.h>
#include <image/silly_raw.h>
#include <image/silly_tif.h>
#include <image/tools/silly_huge_stitcher.h>
#include <image/tools/silly_stitcher.h>

// json
#include <json/silly_jsonpp.h>

// log
#include <log/silly_log.h>

// mapbox
#include <mapbox/mbtiles/dtoa_milo.h>
#include <mapbox/mbtiles/silly_mbtiles_metadata.h>
#include <mapbox/mbtiles/silly_mbtiles_sqlite3.h>
#include <mapbox/mbtiles/silly_mbtiles_text.h>
#include <mapbox/mbtiles/silly_mvt.h>
#include <mapbox/mbtiles/silly_vector_tile.h>
#include <mapbox/mbtiles/vector_tile_21_pb.h>
#include <mapbox/silly_mapbox.h>

// math
#include <math/interpolate/silly_akima.h>
#include <math/interpolate/silly_bicubic.h>
#include <math/interpolate/silly_bilinear.h>
#include <math/silly_bezier_curve.h>
#include <math/silly_interpolate.h>
#include <math/silly_math_const.h>
#include <math/silly_matrix.h>
#include <math/spline/siily_least_squares_fit.h>
#include <math/spline/silly_bezier_spline.h>
#include <math/spline/silly_b_spline.h>
#include <math/spline/silly_catmull_rom_spline.h>
#include <math/spline/silly_cube_spline.h>
#include <math/spline/silly_hermite_spline.h>
#include <math/spline/silly_lagrange_spline.h>
#include <math/spline/silly_parabolic_spline.h>
#include <math/spline/silly_pqs_interp_spline.h>

// metadata
#include <metadata/geotiff_utils.h>
#include <metadata/grib_utils.h>
#include <metadata/narui_radar_grid_utils.h>
#include <metadata/netcdf_utils.h>
#include <metadata/silly_ascii_grid.h>
#include <metadata/silly_egm.h>
#include <metadata/silly_geotiff.h>
#include <metadata/silly_grib2.h>
#include <metadata/silly_micaps.h>
#include <metadata/silly_netcdf.h>
#include <metadata/silly_nmea.h>
#include <metadata/silly_tzx_grid.h>

// network
#include <network/ftp/silly_ftp_client.h>
#include <network/ftp/silly_ftp_server.h>
#include <network/http/silly_http_client.h>
#include <network/http/silly_http_headers.h>
#include <network/http/silly_http_server.h>
#include <network/lora/silly_lora_client.h>
#include <network/lora/silly_lora_gateway.h>
#include <network/lora/silly_lora_server.h>
#include <network/mail/CSendMail.h>
#include <network/mail/silly_imap.h>
#include <network/mail/silly_mail.h>
#include <network/mail/silly_mail_receive.h>
#include <network/mail/silly_mail_send.h>
#include <network/mail/silly_pop3.h>
#include <network/mail/silly_smtp.h>
#include <network/mqtt/silly_mqtt_client.h>
#include <network/mqtt/silly_mqtt_server.h>
#include <network/socket/silly_socket.h>
#include <network/tcp/ftp_utils.h>
#include <network/tcp/silly_curl.h>
#include <network/tcp/silly_ftp.h>
#include <network/tcp/silly_http.h>
#include <network/tools/silly_captcha.h>
#include <network/websocket/silly_websocket_client.h>
#include <network/websocket/silly_websocket_data.h>
#include <network/websocket/silly_websocket_server.h>

// parser
#include <parser/ini/INIReader.h>
#include <parser/ini/silly_boost_ini_parser.h>
#include <parser/ini/silly_ini_parser_base.h>
#include <parser/ini/silly_simple_ini_parser.h>
#include <parser/ini/silly_tzx_ini_parser.h>

// pool
#include <pool/silly_bst_pool.h>
#include <pool/silly_bst_pool_utils.h>
#include <pool/silly_obj_pool.h>
#include <pool/silly_pool_manager.h>
#include <pool/silly_thread_pool.h>

// pretty
#include <pretty/wrapper/silly_decorator.h>

// radar
#include <radar/silly_radar_grid.h>
#include <radar/silly_radar_polar.h>
#include <radar/silly_radar_proj.h>
#include <radar/silly_radar_proj_ez.h>

// singleton
#include <singleton/silly_singleton.h>

// string
#include <string/silly_algorithm.h>
#include <string/silly_format.h>
#include <string/silly_regex.h>

// system
#include <system/silly_disk.h>
#include <system/silly_dll.h>
#include <system/silly_process.h>
#include <system/silly_uuid.h>

// test
#include <test/silly_unit_test.h>

// tzx
#include <tzx/dynamic_rule/silly_dynamic_rule.h>
#include <tzx/moisture/silly_moisture.h>
#include <tzx/pyramid/silly_pyramid_base.h>
#include <tzx/pyramid/silly_pyramid_block.h>
#include <tzx/pyramid/silly_pyramid_data.h>
#include <tzx/pyramid/silly_pyramid_full_text.h>
#include <tzx/pyramid/silly_pyramid_handle.h>
#include <tzx/pyramid/silly_pyramid_index.h>
#include <tzx/pyramid/silly_pyramid_info.h>
#include <tzx/rwdb/silly_pptn.h>
#include <tzx/rwdb/silly_river.h>
#include <tzx/rwdb/silly_rsvr.h>
#include <tzx/rwdb/silly_rwdb_record.h>
#include <tzx/rwdb/silly_stbprp.h>
#include <tzx/rwdb/silly_turn_tool.h>
#include <tzx/silly_adcd.h>
#include <tzx/silly_grid_index.h>
#include <tzx/warn/silly_simple_warn.h>
#include <tzx/warn/silly_ultimate_warn.h>

#endif  // SILLY_UTILS_SU_H
