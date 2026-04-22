# =============================================================================
# ProjectConfig.cmake - silly_utils 项目配置与第三方库开关
# =============================================================================
# 说明：
#   - 所有 DISABLE_* 变量默认为 OFF，设为 ON 时禁用对应库
#   - 代码中通过 #ifndef DISABLE_XXX 判断是否包含相关功能
#   - C++ 标准强制为 C++17
# =============================================================================

# -------------------------- 第三方库开关 --------------------------
# 格式：set(DISABLE_XXX OFF)  # 注释说明
# 禁用时设为 ON，默认 OFF 表示启用

# 基础库
set(DISABLE_FMT OFF)          # 字符串格式化 (fmt)
set(DISABLE_SPDLOG OFF)       # 日志库 (spdlog)
set(DISABLE_JSONPP OFF)       # JSON 处理 (jsoncpp)

# 地理空间库

set(DISABLE_SQLITE3 ON)       # SQLite3（默认禁用，GDAL 已集成）
set(DISABLE_GDAL OFF)         # GDAL 地理数据抽象库
set(DISABLE_POLY_CLIPPING OFF) # 矢量切割算法 (Clipper2)
set(DISABLE_GEOTIFF OFF)      # GeoTIFF 栅格数据读写
set(DISABLE_NETCDF_CXX OFF)   # netCDF C++ 接口
set(DISABLE_ECCODES ON)       # ECMWF 气象数据库 (GRIB2)，默认禁用

# 图像处理库
set(DISABLE_CAIRO OFF)        # Cairo 矢量绘图库
set(DISABLE_FREE_TYPE OFF)    # FreeType 字体支持
set(DISABLE_PNG OFF)          # PNG 图像
set(DISABLE_JPEG OFF)         # JPEG 图像

# 压缩/归档库
set(DISABLE_LIBARCHIVE OFF)   # libarchive 通用归档
set(DISABLE_7ZIP ON)          # 7-Zip 支持（默认禁用）
set(DISABLE_BZIP2 OFF)        # BZip2 压缩
set(DISABLE_LIBZIP OFF)       # libzip ZIP 文件操作
set(DISABLE_LZ4 OFF)          # LZ4 快速压缩（tzx_grid 需要）
set(DISABLE_MINI_ZIP OFF)     # MiniZIP 轻量 ZIP
set(DISABLE_GZIP OFF)         # GZIP 压缩
set(DISABLE_ZLIB OFF)         # ZLIB 基础压缩

# 网络/加密库
set(DISABLE_CRYPTOPP OFF)     # Crypto++ 加密算法
set(DISABLE_CURL OFF)         # CURL 网络请求
set(DISABLE_ICONV OFF)        # 字符串编码转换
set(DISABLE_OPENSSL OFF)      # OpenSSL 加密库
set(DISABLE_PAHO_MQTT OFF)    # Paho MQTT 客户端
set(DISABLE_WEBSOCKET_PP OFF) # WebSocket++（已废弃，保留兼容）

# 数据库库
set(DISABLE_PROTO_ZERO ON)    # ProtoZero MVT 瓦片（默认禁用）
set(DISABLE_SPATIALITE ON)    # SpatiaLite 空间数据库（默认禁用）

# 数学/几何库
set(DISABLE_EIGEN3 ON)        # Eigen3 线性代数（默认禁用）
set(DISABLE_CGAL ON)          # CGAL 计算几何（默认禁用）

# CUDA 支持（可选）
set(DISABLE_CUDA ON)          # CUDA 加速（默认禁用）

# 数据库驱动（达梦等）
set(DISABLE_DM8 ON)           # 达梦数据库（默认禁用）

# -------------------------- C++ 标准设置 --------------------------
# 强制 C++17 标准
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------- 编译器兼容性检查 --------------------------
if(CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "silly_utils 最低要求 C++17，当前版本：${CMAKE_CXX_STANDARD}")
endif()
