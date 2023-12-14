# silly_utils功能列表

[toc]



### 通用宏定义[su_marco.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/su_marco.h)

+ su/encode/silly_pinyin.h


### 3d

+ 山体阴影[silly_hill_shade.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/3d/silly_hill_shade.hpp)

### compress

+ 7z解压缩封装[silly_7z.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/compress/silly_7z.h)

+ Bz2解压缩封装[silly_bz2.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/compress/silly_bz2.h)

+ 解压缩基类[silly_compress_base.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/compress/silly_compress_base.h)

+ GZip解压缩封装[silly_gzip.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/compress/silly_gzip.h)

+ MiniZip解压缩封装[silly_minizip.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/compress/silly_minizip.h)

+ Zip解压缩封装[silly_zip.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/compress/silly_zip.h)

### database

+ 达梦数据库DPI连接方式[dm8_dpi.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/database/dm8/dm8_dpi.h)

+ 达梦数据库ODBC连接方式[dm8_odbc.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/database/dm8/dm8_odbc.h)

+ 达梦数据库PRO_C连接方式(未验证)[dm8_pro_c.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/database/dm8/dm8_pro_c.h)

+ 人大金仓连接方式(未实现)[kb_odbc.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/database/kb/kb_odbc.h)

+ OTL连接数据库头文件[otl_header.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/database/otl/otl_header.hpp)

+ OTL工具类[otl_tools.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/database/otl/otl_tools.h)

+ SQL配置管理模板[sql_studio.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/database/sql_studio.hpp)

### datastruct

+ [TFF_ThreadSafeDeque.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/datastruct/TFF_ThreadSafeDeque.h)

+ [TFF_ThreadSafeList.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/datastruct/TFF_ThreadSafeList.h)

+ [TFF_ThreadSafeMap.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/datastruct/TFF_ThreadSafeMap.h)

+ [TFF_ThreadSafeStack.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/datastruct/TFF_ThreadSafeStack.h)

### datetime

+ 时间操作[simple_time.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/datetime/simple_time.h)

### encode

+ 编码转换[convert.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/encode/convert.hpp)

+ 加密解密[encoder.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/encode/encoder.hpp)

+ 编码转换[silly_encode.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/encode/silly_encode.h)

+ 加密解密[silly_encrypt.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/encode/silly_encrypt.h)

+ 汉字转拼音[silly_pinyin.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/encode/silly_pinyin.h)

### files

+ 文件操作[silly_file.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/files/silly_file.h)

+ 内存文件映射(windows写有问题)[silly_mmap.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/files/silly_mmap.h)

+ 文件操作(有些用法旧或者效率低)[TFF_FileUtils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/files/TFF_FileUtils.h)

### geo

+ 赵的矢量栅格化[CPolygonToRaster.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/CPolygonToRaster.h)

+ 矢量基础类[silly_geo.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_geo.h)

+ deprecate[silly_geometry.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_geometry.h)

+ 矢量转换[silly_geo_convert.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_geo_convert.h)

+ 矢量基础工具[silly_geo_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_geo_utils.h)

+ 极坐标转换[silly_polar.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_polar.h)

+ 投影[silly_projection.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_projection.h)

+ 金字塔[silly_pyramid.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_pyramid.h)

+ 栅格矢量化[silly_raster_to_vector.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_raster_to_vector.h)

+ SpatiaLite库封装[silly_spatialite.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_spatialite.h)

+ 矢量栅格化[silly_vector_to_raster.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/silly_vector_to_raster.h)

+ [TFF_Geo.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/TFF_Geo.h)

+ [TFF_Pyramid.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/TFF_Pyramid.h)

+ [TFF_Screen.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/geo/TFF_Screen.h)

### gpc

+ 矢量切割[gpc.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/gpc/gpc.h)

### hydrometeorology

+ 水文雨量站[silly_station_rain.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/hydrometeorology/silly_station_rain.h)


### image

+ BMP图像处理[bmp_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/image/bmp_utils.h)

+ GIF图像处理[gif_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/image/gif_utils.h)

+ ICON图像处理[icon_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/image/icon_utils.h)

+ JPEG图像处理[jpeg_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/image/jpeg_utils.h)

+ PNG图像处理[png_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/image/png_utils.h)

+ RAW图像处理[raw_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/image/raw_utils.h)

+ TIF图像处理[tif_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/image/tif_utils.h)

### json

+ Json和对象相互转换(需要继续研究)[json_to_obj.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/json/json_to_obj.hpp)

### loguru

+ 简单的日志[loguru.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/loguru/loguru.hpp)

### mapbox

+ [dtoa_milo.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/mapbox/mbtiles/dtoa_milo.h)

+ 读取mbtiles块[silly_mbtiles_reader.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/mapbox/mbtiles/silly_mbtiles_reader.
h)

+ [silly_mvt.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/mapbox/mbtiles/silly_mvt.h)

+ [silly_vector_tile.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/mapbox/mbtiles/silly_vector_tile.h)

+ [vector_tile_21_pb.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/mapbox/mbtiles/vector_tile_21_pb.h)

+ [silly_mapbox.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/mapbox/silly_mapbox.h)

### math

+ 二维矩阵[silly_matrix.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/silly_matrix.h)

+ [TFF_Double2.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_Double2.h)

+ [TFF_Float2.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_Float2.h)

+ [TFF_Float4.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_Float4.h)

+ [TFF_FourCC.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_FourCC.h)

+ [TFF_GeometryCalc.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_GeometryCalc.h)

+ [TFF_GeometryUtils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_GeometryUtils.h)

+ [TFF_MathUtils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_MathUtils.h)

+ [TFF_Point.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_Point.h)

+ [TFF_Rectangle.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_Rectangle.h)

+ [TFF_Scaler.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_Scaler.hpp)

+ [TFF_Vector.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/math/TFF_Vector.h)

### proj

+ [CoordinateConvert.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/proj/CoordinateConvert.hpp)

+ [EllipsoidProj.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/proj/EllipsoidProj.h)

### singleton

+ 目前用的最简单的单例模板类[silly_singleton.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/singleton/silly_singleton.h)

+ [TFF_BoostSingleton.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/singleton/TFF_BoostSingleton.h)

+ [TFF_ThreadSafeSingleton.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/singleton/TFF_ThreadSafeSingleton
.h)

### system

+ UUID[silly_uuid.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/system/silly_uuid.hpp)

+ 字符串拼接[string_format.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/system/string_format.h)

### tcp

+ curl实现的FTP工具[ftp_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/tcp/ftp_utils.h)

+ FTP原生功能封装,有些还没测试[silly_ftp.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/tcp/silly_ftp.h)

+ HTTP(s)请求工具[silly_http.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/tcp/silly_http.h)

### thiessen

+ voronoi图(泰森多边形)[jc_voronoi.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/thiessen/jc_voronoi.h)

+ [jvc_struct.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/thiessen/jvc_struct.h)

+ 泰森多边形封装[thiessen_creator.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/thiessen/thiessen_creator.h)

### triangular

+ 德劳内三角化[silly_delaunay.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/triangular/silly_delaunay.hpp)

+ [TFF_CovexHull.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/triangular/TFF_CovexHull.h)

+ [TFF_Delaunay.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/triangular/TFF_Delaunay.h)

+ [TFF_ThiessenPolygon.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/triangular/TFF_ThiessenPolygon.h)

+ [TFF_Voronoi.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/triangular/TFF_Voronoi.h)

### tzx

+ TZX金字塔基类[silly_pyramid_base.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/tzx/pyramid/silly_pyramid_base.h)

+ TZX金字塔数据文件[silly_pyramid_data.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/tzx/pyramid/silly_pyramid_data.h)

+ TZX金字塔全文检索文件[silly_pyramid_full_text.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/tzx/pyramid/silly_pyramid_full_te
xt.h)

+ TZX金字塔索引文件[silly_pyramid_index.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/tzx/pyramid/silly_pyramid_index.h)

+ TZX金字塔信息文件[silly_pyramid_info.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/tzx/pyramid/silly_pyramid_info.h)

+ TZX金字塔简单工具[silly_pyramid_tools.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/tzx/pyramid/silly_pyramid_tools.h)

### vacuate

+ [psimpl.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/vacuate/psimpl.h)

+ 抽稀算法封装[vacuate_wrapper.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/vacuate/vacuate_wrapper.h)

### vectorize

+ 风险预警成果矢量化[silly_vectorizer.hpp](http://192.168.0.60/douliyang/silly_utils/blob/master/su/vectorize/silly_vectorizer.hpp)

### weather

+ [geotiff_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/weather/geotiff_utils.h)

+ [grib_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/weather/grib_utils.h)

+ [netcdf_utils.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/weather/netcdf_utils.h)

+ GeoTIFF读写工具[silly_geotiff.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/weather/silly_geotiff.h)

+ GRB2读写工具[silly_grib2.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/weather/silly_grib2.h)

+ NetCDF读写工具(好像有内存泄漏,并发读写也有问题)[silly_netcdf_c.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/weather/silly_netcdf_c.h)

+ [silly_netcdf_cf.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/weather/silly_netcdf_cf.h)

+ [silly_netcdf_cxx.h](http://192.168.0.60/douliyang/silly_utils/blob/master/su/weather/silly_netcdf_cxx.h)



