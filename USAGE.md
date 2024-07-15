
# silly_utils库的使用

`silly_utils` 是一个通用工具模块，旨在简化项目中的工具函数使用，并通过CMake集成到其他项目中。以下是如何在您的项目中使用 

## 环境要求

- 确保您的项目使用CMake作为构建系统。
- 使用vcpkg作为依赖管理器。确保已经安装并设置好了vcpkg。

## 获取silly_utils库

1. **拷贝CMake配置文件**：将 `silly_utils` 项目中的 `cmake` 文件夹拷贝到您的项目中与项目根目录的 `CMakeLists.txt` 同级的目录。

2. **配置CMakeLists.txt**：在您的项目根目录的 `CMakeLists.txt` 文件中进行如下配置：

    ```cmake
    # 设置CMake模块路径，以便找到silly_utils的配置文件
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
    
    # 包含silly_utils库的配置
    include(silly_utils)
    ```

## 配置项目以使用silly_utils

1. **设置项目名称和其他基础配置**：在 `CMakeLists.txt` 中设置您的项目名称，并进行其他基础配置。

    ```cmake
    cmake_minimum_required(VERSION 3.15)
    project(YourProjectName) # 替换为您的项目名称
    ```

2. **包含头文件目录**：确保您的源代码可以访问 `silly_utils` 的头文件。

    ```cmake
    include_directories("${SILLY_UTILS_FETCH_ROOT}/silly_utils/su")
    ```

3. **添加子目录**：如果您的项目有多个子目录，确保它们都被添加到CMake中。

    ```cmake
    add_subdirectory(src)
    # ... 其他子目录
    ```

4. **链接silly_utils库**：在需要使用 `silly_utils` 功能的可执行文件或库的CMake配置中链接 `sutils` 库。

    ```cmake
    target_link_libraries(${PROJECT_NAME} PUBLIC sutils)
    ```

## 安装依赖库

使用vcpkg安装所需的依赖库。在Windows和Linux系统上，打开终端或命令提示符并运行以下命令：

**Windows:**
```sh
boost, cryptopp, iconv, polyclipping, jsoncpp, zlib, libzip, bzip2, minizip, gzip, 7zip, rapidjson, netcdfcxx, png, jpeg, openssl, protozero, geotiff, gdal, spatialite, unofficial-sqlite3, hiredis, fmt, cairo, freetype, spdlog, lz4
```

**Linux:**
```sh
# 确保已经安装了上述Windows命令中的库,linux下需要额外安装下面这些库
unofficial-libuuid , geos , freexl , proj , curl , libtiff , libxml2 , rttopo , unixodbc or -lodbc , fontconfig , pixman 
```

## 编译项目

1. 生成配置缓存
2. 选择需要编译的可执行程序进行编译生成


# example 示例

`silly_utils` 库的 `examples/project` 目录下提供了一个示例项目，展示了如何在实际项目中使用 `silly_utils`：

## 项目目录结构

Folder PATH listing
Volume serial number is 06F2-EA39
D:.
│   .clang-format
│   .gitignore
│   CMakeLists.txt
│   CMakeSettings.json
│   README.md
│
├───cmake
│       dm8_check.cmake
│       env_check.cmake
│       grib_api_check.cmake
│       silly_utils.cmake
│
├───config
│       config.json
│
└───src
    │   CMakeLists.txt
    │   main.cpp
    │
    └───example_tool
            CMakeLists.txt
            config.hpp
            project.cpp
            project.h


- **根目录**：包含 `CMakeLists.txt` 和其他基础配置文件。
- **config 目录**：包含项目配置文件，如 `config.json`。
- **src 目录**：包含源代码和 `example_tool` 子目录，后者是一个示例模块，演示了如何在模块级别链接 `silly_utils` 库。

在示例项目的根 `CMakeLists.txt` 中，配置了项目的基本设置并引入了 `silly_utils` 库。`src` 目录下的 `main.cpp` 文件用于生成可执行程序，并可能链接到 `example_tool` 模块。如果 `example_tool` 已经链接了 `silly_utils` 库，则 `main.cpp` 在构建时无需再次链接。可根据实际需要进行更改。

通过这个示例项目，您可以快速理解如何在项目中集成 `silly_utils` 库，并开始使用它提供的工具函数。











