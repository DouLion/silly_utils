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

// json
#include <json/silly_jsonpp.h>  // json文件,json字符串,json对象处理

// log
#include <log/silly_log.h>  // 日志

// compress (压缩/解压缩)
#include <compress/silly_compress.h>  // 压缩 错误码定义
#include <compress/silly_7z.h>        // 7z压缩/解压缩, 未实现
#include <compress/silly_bz2.h>       // bz2 压缩/解压缩 (文件,内存数据)
#include <compress/silly_gzip.h>      // gzip 压缩/解压缩(内存数据), 文件的压缩解压缩未实现
#include <compress/silly_minizip.h>   // minizip 压缩/解压缩(文件,内存数据)
#include <compress/silly_rar.h>       // rar 压缩/解压缩(文件), 内存数据的压缩解压缩未实现
#include <compress/silly_zip.h>       // 未实现

// database (数据库)
#include <database/dm8/dm8_dpi.h>               // 达梦数据库dpi接口
#include <database/dm8/dm8_pro_c.h>             // 未实现
#include <database/kb/kb_odbc.h>                // 未实现
#include <database/otl/otlv4.h>                 // otl库源码
#include <database/otl/silly_otl.h>             // 基于otl的数据库操作
#include <database/redis/silly_redis_client.h>  // 未实现
#include <database/redis/silly_redis_cpool.h>   // 未实现
#include <database/silly_sql.h>                 // 常用数据库的SQL
#include <database/sqlite3/silly_sqlite3.h>     // sqlite3 的一些常用用法

// datastruct (数据结构)
#include <datastruct/silly_safe_bimap.h>  // 双向map模板类
#include <datastruct/silly_safe_deque.h>  // 线程安全的deque模板类
#include <datastruct/silly_safe_list.h>   // 线程安全的list模板类
#include <datastruct/silly_safe_map.h>    // 线程安全的map模板类
#include <datastruct/silly_safe_stack.h>  // 线程安全的栈模板类

// datetime (时间)
#include <datetime/silly_posix_time.h>  // 时间工具类 (统一时间为时间戳类型)
#include <datetime/silly_timer.h>       // 定时器

// encode (编码)
#include <encode/silly_crc.h>      // CRC校验 (crc16 crc32 crc64)
#include <encode/silly_encode.h>   // 编码转换, 字符串 编码检测 与 编码转换
#include <encode/silly_encrypt.h>  // 加密解密 (md5, AES)
#include <encode/silly_pinyin.h>   // 汉字转拼音

// files (文件)
#include <files/silly_file.h>           // 文件/目录 工具类(读写,复制,删除)
#include <files/silly_file_lock.h>      // 文件锁
#include <files/silly_memory_map.h>     // 内存映射文件 (读取,写入)
#include <files/silly_schedule_file.h>  // 定时生成文件, 结构确定内容未实现
#include <files/silly_xlsx.h>           // xlsx文件操作 未实现

// 几何(平面几何,地理空间空间)
#include <geo/silly_geo_const.h>                       // 地球基本信息常量定义
#include <geo/gpc/gpc.h>                               // gpc源码
#include <geo/proj/gdal/silly_projection_define.h>     // 投影坐标系的 wtk
#include <geo/proj/gdal/silly_proj_convert.h>          // 地理坐标系的转换
#include <geo/proj/silly_proj.h>                       // (高斯投影, 墨卡托投影)转经纬度坐标
#include <geo/silly_dem.h>                             // dem数据读写 未实现
#include <geo/silly_geo.h>                             // 自定义点线面多线多面矢量类型
#include <geo/silly_geojson.h>                         // geojson处理类, 未实现
#include <geo/silly_geo_coll.h>                        // 复合矢量类型定义
#include <geo/silly_geo_convert.h>                     // 矢量数据转换: shp与geojson互转, 地理坐标系与墨卡托坐标系互转
#include <geo/silly_geo_prop.h>                        // 矢量数据的属性定义
#include <geo/silly_geo_utils.h>                       // 基于gdal的矢量读写和算法
#include <geo/silly_iso_poly.h>                        // 一种txt文件格式的矢量面
#include <geo/silly_map_layer.h>                       // 地图图层类
#include <geo/silly_polar.h>                           // 极坐标与经纬度坐标互转
#include <geo/silly_projection.h>                      // 经纬度坐标与墨卡托投影的互转
#include <geo/silly_pyramid.h>                         // 矢量金字塔:经纬度转金字塔层块号,瓦片经纬度范围, 屏幕坐标与经纬度坐标互转, 瓦片转屏幕范围
#include <geo/silly_raster_to_vector.h>                // Marching squares 算法
#include <geo/silly_spatialite.h>                      // spatialite数据库操作: 创建, 删除,  矢量数据的读取写入, 查询, 修改
#include <geo/silly_vector_to_raster.h>                // 矢量转栅格,扫描线算法
#include <geo/triangular/silly_delaunay.h>             // 德劳内三角剖分的基础数据结构和工具类
#include <geo/vacuate/psimpl.h>                        // psimpl源码
#include <geo/vacuate/silly_vacuate.h>                 // 抽稀算法

// 图像处理
#include <graphics/font/silly_font.h>                           // 处理字体
#include <graphics/grid/silly_render.h>                         // 未实现
#include <graphics/render/canvas/silly_cairo.h>                 // cairo库处理图片工具
#include <graphics/render/pattern/cairo/silly_cairo_pattern.h>  // 未实现
#include <graphics/render/shape/silly_render_shapes.h>          // 未实现
#include <graphics/render/silly_grid_render.h>                  // png 图片渲染
#include <graphics/render/silly_render_brush.h>                 // 未实现
#include <graphics/render/silly_render_pen.h>                   // 未实现
#include <graphics/render/style/silly_render_style.h>           // 未实现
#include <graphics/render/symbol/silly_render_symbol.h>         // 未实现
#include <graphics/silly_bmp.h>                                 // 未实现
#include <graphics/silly_color.h>                               // color类 自定义的统一颜色类型
#include <graphics/silly_gif.h>                                 // 未实现
#include <graphics/silly_icon.h>                                // 未实现
#include <graphics/silly_image_base.h>                          // 处理图片类的基类
#include <graphics/silly_jpeg.h>                                // jpeg图片处理
#include <graphics/silly_png.h>                                 // png图片处理
#include <graphics/silly_raw.h>                                 // 未实现
#include <graphics/silly_tif.h>                                 // 为实现
#include <graphics/tools/silly_huge_stitcher.h>                 // 超大图像拼接 未实现
#include <graphics/tools/silly_stitcher.h>                      // 图像拼接 未实现

// mapbox
#include <mapbox/mbtiles/dtoa_milo.h>               // 高效且精确的 double 到 ASCII 字符串（以及反向）的转换
#include <mapbox/mbtiles/silly_mbtiles_metadata.h>  // mapbox中metadata数据
#include <mapbox/mbtiles/silly_mbtiles_sqlite3.h>   // mapbox中mbtiles读取写入
#include <mapbox/mbtiles/silly_mbtiles_text.h>      //
#include <mapbox/mbtiles/silly_mvt.h>               // mapbox中mvt_tile类
#include <mapbox/mbtiles/silly_vector_tile.h>       // 未实现
#include <mapbox/mbtiles/vector_tile_21_pb.h>       // 未实现
#include <mapbox/silly_mapbox.h>                    // mapbox工具

// math (数学)
#include <math/interpolate/silly_akima.h>          // akima插值
#include <math/interpolate/silly_bicubic.h>        // 双三次插值
#include <math/interpolate/silly_bilinear.h>       // 双线性插值
#include <math/silly_bezier_curve.h>               // 贝塞尔曲线
#include <math/silly_interpolate.h>                // 插值函数
#include <math/silly_math_const.h>                 // 数学计算中的常量
#include <math/silly_matrix.h>                     // 矩阵类
#include <math/spline/siily_least_squares_fit.h>   // 未实现
#include <math/spline/silly_bezier_spline.h>       // 未实现
#include <math/spline/silly_b_spline.h>            // 二次 B 样条插值
#include <math/spline/silly_catmull_rom_spline.h>  // 未实现
#include <math/spline/silly_cube_spline.h>         // 未实现
#include <math/spline/silly_hermite_spline.h>      // 未实现
#include <math/spline/silly_lagrange_spline.h>     // 未实现
#include <math/spline/silly_parabolic_spline.h>    // 未实现
#include <math/spline/silly_pqs_interp_spline.h>   // 未实现

// 常用气象,地理信息树读写
#include <metadata/geotiff_utils.h>           // geotiff 
#include <metadata/grib_utils.h>              // grid 
#include <metadata/narui_radar_grid_utils.h>  // 纳睿达网格文件
#include <metadata/netcdf_utils.h>            // netcdf 
#include <metadata/silly_ascii_grid.h>        // ascii网格文件
#include <metadata/silly_egm.h>               // 地球重力场文件数据
#include <metadata/silly_geotiff.h>           // 未实现
#include <metadata/silly_grib2.h>             // grib2读取工具
#include <metadata/silly_micaps.h>            // 中国气象局Micaps系统的数据
#include <metadata/silly_netcdf.h>            // netcdf读取工具
#include <metadata/silly_nmea.h>              // GPGSA GPS数据传输标准
#include <metadata/silly_tzx_grid.h>          // tzx网格文件

// network (网络)
#include <network/ftp/silly_ftp_client.h>              // 未实现
#include <network/ftp/silly_ftp_server.h>              // 未实现
#include <network/http/silly_http_client.h>            // http(https)客户端 类声明
#include <network/http/silly_http_headers.h>           // 未实现
#include <network/http/silly_http_server.h>            // drogon请求头宏定义
#include <network/lora/silly_lora_client.h>            // 未实现
#include <network/lora/silly_lora_gateway.h>           // 未实现
#include <network/lora/silly_lora_server.h>            // 未实现
#include <network/mail/CSendMail.h>                    // 实现邮件的发送功能，支持多个用户接收，支持附件
#include <network/mail/silly_imap.h>                   // 未实现
#include <network/mail/silly_mail.h>                   // 邮件发送与接收的工具,结构定义内容未实现
#include <network/mail/silly_mail_receive.h>           // 未实现
#include <network/mail/silly_mail_send.h>              //
#include <network/mail/silly_pop3.h>                   // 未实现
#include <network/mail/silly_smtp.h>                   // SMTP客户端,与邮件服务器进行连接、登录、发送邮件和附件等操作
#include <network/mqtt/silly_mqtt_client.h>            // mqtt 客户端
#include <network/mqtt/silly_mqtt_server.h>            // 未实现
#include <network/socket/silly_socket.h>               // socket工具
#include <network/tcp/ftp_utils.h>                     // ftp 工具
#include <network/tcp/silly_ftp.h>                     // ftp客户端工具
#include <network/tools/silly_captcha.h>               // 验证码
#include <network/websocket/silly_websocket_client.h>  // websocket 客户端
#include <network/websocket/silly_websocket_data.h>    // 未实现
#include <network/websocket/silly_websocket_server.h>  // websocket 服务端 未实现

// INI配置文件
#include <parser/ini/INIReader.h>                // ini解析源码
#include <parser/ini/silly_ini_parser_base.h>    // ini文件解析 基类
#include <parser/ini/silly_boost_ini_parser.h>   // 基于boost的ini工具
#include <parser/ini/silly_simple_ini_parser.h>  // 基于SimpleIni的ini文件解析
#include <parser/ini/silly_tzx_ini_parser.h>     // tzx ini文件解析工具

// pool (线程池)
#include <pool/silly_bst_pool.h>        // 线程池源码
#include <pool/silly_bst_pool_utils.h>  // 线程池源码
#include <pool/silly_obj_pool.h>        // 对象池
#include <pool/silly_pool_manager.h>    // 连接池
#include <pool/silly_thread_pool.h>     // 线程池

// pretty
#include <pretty/wrapper/silly_decorator.h>  // 异常捕获模板函数

// radar (雷达)
#include <radar/silly_radar_grid.h>     // 雷达网格数据存储
#include <radar/silly_radar_polar.h>    // 雷达极坐标系数据存储
#include <radar/silly_radar_proj.h>     // 雷达坐标系
#include <radar/silly_radar_proj_ez.h>  // 雷达坐标系简化用法 未实现

// singleton (单例)
#include <singleton/silly_singleton.h>  // 单例类

// string (字符串)
#include <string/silly_algorithm.h>  // 字符串算法
#include <string/silly_format.h>     // 字符串格式化
#include <string/silly_regex.h>      // 正则表达式

// system (系统)
#include <system/silly_disk.h>     // 获取磁盘信息工具
#include <system/silly_dll.h>      // 动态链接库工具
#include <system/silly_process.h>  // 进程工具
#include <system/silly_uuid.h>     // guid(唯一标识码)

// test (测试)
#include <test/silly_unit_test.h>  // 单元测试模块 未实现

// 公司自定义的格式 
#include <tzx/dynamic_rule/silly_dynamic_rule.h>  // 使用本地文件实现的动态指标快速读写
#include <tzx/moisture/silly_moisture.h>          // 使用本地文件实现的土壤含水量快速读写
#include <tzx/pyramid/silly_pyramid_base.h>       // 金字塔基类
#include <tzx/pyramid/silly_pyramid_block.h>      // 金字塔数据块
#include <tzx/pyramid/silly_pyramid_data.h>       // 金字塔data
#include <tzx/pyramid/silly_pyramid_full_text.h>  // 金字塔全文检索
#include <tzx/pyramid/silly_pyramid_index.h>      // 金字塔索引类
#include <tzx/pyramid/silly_pyramid_info.h>       // 金字塔info
#include <tzx/pyramid/silly_pyramid_handle.h>     // 金字塔文件统一处理类,支持mmap读和作为普通文件写入,不限制大小


#include <tzx/rwdb/silly_rwdb_record.h>           // 数据库记录基类
#include <tzx/rwdb/silly_pptn.h>                  // pptn表对象类
#include <tzx/rwdb/silly_river.h>                 // river表对象类
#include <tzx/rwdb/silly_rsvr.h>                  // rsvr表对象类
#include <tzx/rwdb/silly_stbprp.h>                // stbprp表对象类
#include <tzx/rwdb/silly_turn_tool.h>             // 字符串与二进制数据互转
#include <tzx/silly_adcd.h>                       // adcd对象类
#include <tzx/silly_grid_index.h>                 // 网格点索引统一
#include <tzx/warn/silly_simple_warn.h>           // 一个告警必要的属性内容
#include <tzx/warn/silly_ultimate_warn.h>         // 告警内容的终极信息, 尽量包含目前已知的信息

#endif  // SILLY_UTILS_SU_H
