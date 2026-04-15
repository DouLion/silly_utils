# silly_utils CMake 构建系统说明

## 📁 目录结构

```
cmake/
├── config/           # 项目配置与开关定义
│   ├── ProjectConfig.cmake  # 所有 DISABLE_* 开关定义
│   └── SUConfig.cmake       # 项目通用配置
├── env/             # 环境检查
│   ├── CheckEnv.cmake       # 编译器、平台、C++ 标准检查
│   └── CheckOSEndian.cmake  # 系统字节序检测
├── deps/            # 第三方库查找模块 (FindXxx.cmake)
│   └── Find*.cmake          # 所有依赖的查找模块
├── tools/           # 工具函数
│   └── CheckThirdLib.cmake  # 聚合调用 deps 模块
├── env/             # 已存在，保留
│   └── SetProjectRoot.cmake # 项目根目录设置
└── README.md        # 本文档
```

## ⚙️ 构建开关

所有 `DISABLE_*` 开关定义在 `cmake/config/ProjectConfig.cmake` 中：

```cmake
# 禁用某个库：设为 ON
set(DISABLE_FMT OFF)      # 启用 FMT
set(DISABLE_GDAL OFF)     # 启用 GDAL
set(DISABLE_ECCODES ON)   # 禁用 EcCodes (GRIB2)
```

代码中通过宏判断是否包含相关功能：
```cpp
#ifndef DISABLE_GDAL
    // GDAL 相关代码
#endif
```

## 🛠️ 构建命令

### 配置（仅生成，不编译）
```powershell
# 使用 vcpkg（推荐）
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 或使用自定义 toolchain
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="E:\vcpkg\scripts\buildsystems\vcpkg.cmake"
```

### 编译
```powershell
cmake --build build --config Release
```

## 📦 依赖说明

### 必选依赖
- **CMake 3.15+**
- **C++17 或更高**（最低要求）
- **MSVC 2019+** 或 **GCC 8+** 或 **Clang 7+**

### 第三方库（通过 vcpkg 安装）
```powershell
# 基础库
vcpkg install fmt spdlog jsoncpp

# 地理空间
vcpkg install gdal geotiff netcdf-cxx clipper2

# 图像处理
vcpkg install cairo freetype libpng libjpeg-turbo

# 压缩
vcpkg install libarchive libzip lz4 zlib bzip2

# 网络/加密
vcpkg install curl openssl crypto++ paho-mqttpp3

# 其他
vcpkg install eigen3
```

## 🚫 禁用特定库

如果不需要某些库，可以在 `cmake/config/ProjectConfig.cmake` 中设置：

```cmake
set(DISABLE_EIGEN3 ON)    # 禁用 Eigen3
set(DISABLE_CGAL ON)      # 禁用 CGAL
set(DISABLE_NETCDF_CXX ON) # 禁用 netCDF
```

或者在命令行中覆盖：
```powershell
cmake -B build -DDISABLE_EIGEN3=ON -DDISABLE_CGAL=ON
```

## 📝 维护说明

- **新增依赖**：在 `deps/` 目录创建 `FindXxx.cmake`，在 `CheckThirdLib.cmake` 中 include
- **修改开关**：在 `ProjectConfig.cmake` 中添加/修改 `DISABLE_XXX` 定义
- **调整查找逻辑**：修改对应的 `FindXxx.cmake` 文件

## 🔧 常见问题

### 1. "CMake 找不到库"
确保 vcpkg 已安装该库，并正确设置 `CMAKE_TOOLCHAIN_FILE`：
```powershell
$env:CMAKE_TOOLCHAIN_FILE="E:\vcpkg\scripts\buildsystems\vcpkg.cmake"
```

### 2. "C++17 标准不满足"
检查编译器是否支持 C++17，CMake 会自动检测并报错。

### 3. "DISABLE_XXX 设置无效"
确保在 `ProjectConfig.cmake` 中设置，或在命令行中使用 `-D` 参数覆盖。
