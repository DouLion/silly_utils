
# silly_utils库的使用

`silly_utils` 是一个通用工具模块，旨在简化项目中的工具函数使用，并通过CMake集成到其他项目中。以下是如何在您的项目中使用 

## 环境要求

- 确保您的项目使用CMake作为构建系统。
- 使用vcpkg作为依赖管理器。确保已经安装并设置好了vcpkg。

## 获取silly_utils库

1. **拷贝CMake配置文件**：将 `silly_utils` 项目中的 `cmake/silly_utils.cmake文件` 文件夹拷贝到您的项目中与项目根目录的 `CMakeLists.txt` 同级的目录的`cmake`目录下。

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

2. **添加子目录**：如果您的项目有多个子目录，确保它们都被添加到CMake中。

    ```cmake
    add_subdirectory(core)  # core代码
    add_subdirectory(web)   # web项目
    add_subdirectory(svr)   # svr项目
    # ... 其他子目录
    ```

3. **链接silly_utils库**：在需要使用 `silly_utils` 功能的可执行文件或库的CMake配置中链接 `sutils` 库。

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


# example 示例

`silly_utils` 库的 `examples/project` 目录下提供了一个示例项目，展示了如何在实际项目中使用 `silly_utils` 的一个实例项目,该实例同时包含web程序和svr程序,：

## 项目目录结构

D:\1_wangyingjie\code\4_silly_utils\silly_utils\examples\project>tree /f
Folder PATH listing
Volume serial number is 06F2-EA39
D:.
│   .clang-format
│   CMakeLists.txt
│
├───cmake
│       silly_utils.cmake
│
├───config
│       config.json
│       core_config.json
│       web_config.json
├───core
│   │   CMakeLists.txt
│   │   project.cpp
│   │   project.h
│   │
│   └───config_tool
│           config_tool.h
├───svr
│       CMakeLists.txt
│       main.cpp
└───web
    │   CMakeLists.txt
    │   main.cc
    │
    ├───controllers
    │       check_ctrl.cc
    │       check_ctrl.h
    │
    ├───filters
    ├───models
    │       model.json
    │
    ├───plugins
    ├───test
    │       CMakeLists.txt
    │       test_main.cc
    │
    └───views


- **根目录**：包含 `CMakeLists.txt` 和其他基础配置文件。
  - 若需要添加其它项目的目录直接创建目录然后再根目录下的`CMakeLists.txt`文件中中直接添加`add_subdirectory(add)`
  
- **config 目录**：包含项目配置文件，如web项目配置文件`core_config.json`
- **core 目录**：核心函数和工具目录
  - 该目录下的工具生成动态链接库`core_lib`,`core_lib`以public的方式链接 `silly_utils` 库
  - 后续添加其它的函数直接在该目录下添加新cpp文件即可。
- **svr 目录**：该目录下为生成svr程序
  - `svr/main.cpp`链接`core_lib`库,由于core_lib库已经链接`silly_utils`库,所以可以直接使用`silly_utils`库的函数,但需添加头文件
  - 生成可执行程序名为`exampleSvr`,根据需求自行修改
- **web 目录**：该目录下为生成web程序
  - 链接`drogon`库和`core_lib`库
  - 生成可执行的web程序 `exampleWeb`,根据需求自行修改


通过这个示例项目，您可以直接复制该项目就可以实现svr程序和web程序的的整体框架。











