# silly_utils

通用工具模块,目的是能够在其他CMake构建的项目中能够直接使用, 以C++ 17为标准.

本项目中只做基本功能实现,在设计参数时不要引入业务逻辑,始终希望任意一个类都可以独立工作.

请在Linux和Windows平台上都做测试

### [Unix 17条基本哲学](Unix Philosophy,md)

**原则,约束只对实践起指导作用,而不是决定作用**

**否定一项规则最好的方式就是100%遵循它**

[Basics of the Unix Philosophy](http://www.catb.org/~esr/writings/taoup/html/ch01s06.html)

- 模块原则，Modularity：写简单的程序，并用好的接口连接它们
- 清晰原则，Clarity：清楚透明的算法比“高明”的算法更好
- 组装原则，Composition：写能够跟其他程序一起工作的程序
- 隔离原则，Separation：分离接口（使用引擎的方法）和引擎
- 简单原则，Simplicity：尽量简化算法，不到必要的时候不要增加复杂度
- 简约原则，Parsimony：只要在必要的时候才写大型程序，通常小程序已经足够了
- [透明原则](https://zhida.zhihu.com/search?content_id=109695570&content_type=Article&match_order=1&q=透明原则&zhida_source=entity)，Transparency：写容易测试和纠错的代码
- 健壮原则，Robustness：这是简单和简约的副产物
- 表达原则，Representation：用数据结构表达逻辑，而不是用过程表达逻辑
- 传统原则，Least Surprise：用最常识的方法设计借口
- 安静原则，Silence：如果程序没什么特别事情要表达，应该保持安静！
- 经济原则，Economy：程序员的时间比机器的时间更加宝贵
- 生成原则，Generation：尽量写代码来生成代码，而不是手工输入代码
- 修复原则，Repair：当程序出现异常，应该明确的抛出异常，而且约早越好！
- 优化原则，Optimization：先让程序工作，在考虑优化的事情
- 多样性原则，Diversity：一个问题有很多好的解决方案，没有最好的解决方案！
- 拓展性原则，Extensible： 设计程序时应该考虑到未来的拓展，因为未来比你想象来的早

# 依赖环境

### 1 Windows

+ Visual studio 2015及以上,必须包含的组件 `MSVC v140 -VS 2015 c++ 生成工具` ,语言包必须额外安装`英语`
+ 也可以借助cygwin 或者 mysm2等跨平台编译工具,需要自行探索
+ CMake 3.20+
+ Git 2.30+
+ vcpkg(跨平台C++包管理器)

### 2 Linux

+ GCC 6.0+ ,建议GCC 9.10 不要超过 GCC 9.11(曾经有一个库的在高版本编译有问题) 
+ CMake 3.20+
+ Git 2.30+
+ vcpkg(跨平台C++包管理器)

## 使用方式

**下面会使用`UPROJ`表示新项目的根目录**

### 1. 自动构建(依赖Python)

+ `mkdir UPROJ` 或者 新建文件目录 -> `UPROJ`
+ 进入`UPROJ`

+ 将本项目`tools/python/auto_gen.py`拷贝到`UPROJ`
+ 运行命令`python auto_gen.py`
+ 按照提示输入,不要与`su`,`su_marco`重名,如果端口号指定为0或者未输入端口号,那么不会自动生成web模块

### 2. 手动拷贝

+ 拷贝`cmake/FetchSU.cmake`文件,到`UPROJ/cmake/FetchSU.cmake`;

+ 本项目依赖很多第三方库,你可能并不需要,拷贝`cmake/ThirdSupport.cmake`到`UPROJ/cmake/ThirdSupport.cmake`,将不需要的第三方库的`ENABLE_XXX`修改为`OFF`,

+ 在`UPROJ/CMakeList.txt`中添加如下内容

  ```cmake
  cmake_minimum_required(VERSION 3.15)
  project(项目名称)
  
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
  include(ThirdSupport)
  include(FetchSU)
  
  ##########################################
  # global define.
  ##########################################
  if(DEBUG_MODE)
      add_definitions(-DPROJ_ROOT="${CMAKE_SOURCE_DIR}")
  else()
      add_definitions(-DPROJ_ROOT="./")
  endif()
  # [[个人习惯, 在服务中使用std::filesystem::current_path(PROJ_ROOT) 切换当前工作目录]]
  
  ##########################################.
  # global include.
  ##########################################
  include_directories("${CMAKE_SOURCE_DIR}/core")
  
  ##########################################
  # Add sub dirs.
  ######################+####################
  add_subdirectory(core)  # 自己的业务逻辑
  add_subdirectory(web) # http请求处理, 依赖drogon, 使用 vcpkg install drogon 安装
  add_subdirectory(app) # 普通的程序,一般是定时任务等
  add_subdirectory(test) # 功能测试
  add_subdirectory(tools) # 
  
  ```
  
+ 记得要链接本项目

  ```cmake
  target_link_libraries(${PROJECT_NAME} PUBLIC sutils)
  ```


## OPENMP
    https://blog.csdn.net/ab0902cd/article/details/108770396
    https://blog.csdn.net/jjjstephen/article/details/132201248
    https://blog.csdn.net/yongbaofeng1234/article/details/88651113
    https://zhuanlan.zhihu.com/p/61857547

## vcpkg依赖库:

+ windows 下执行:

  ```
  vcpkg install msinttypes boost gdal zlib bzip2 libzip jsoncpp polyclipping cryptopp libiconv otl curl netcdf-cxx4 rapidjson sqlite3 libspatialite libgeotiff protozero libpng libjpeg-turbo simpleini 7zip drogon[*] fmt spdlog catch2 gzip-hpp cairo
  
  ```


+ linux下额外安装

  ```
  vcpkg install geos freexl curl librttopo proj libxml2 tiff libuuid
  ```
  
  

## 功能列表

+ 见`FUNCTIONS.md`文件, 或者 `su/su.h`

