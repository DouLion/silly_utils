# silly_utils

通用工具模块,目的是能够再cmake中直接引用该项目代码



## 使用方式

+ 将cmake 目录拷贝到项目AnyProj根目录

+ 在CMakeList.txt添加AnyProj源代码之前, 添加如下几行代码

  ```cmake
  # 
  set(SU_AS_SUB_PROJECT TRUE)
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
  include(env_check)
  include(dm8_check)
  include(silly_utils)
  include_directories(silly_utils/su)
  ```

  这几行代码结合cmake/silly_utils.cmake 会将gitlab上的silly_utils项目源码下载到项目根目录下,并且以源码的形式嵌入当前项目

+ 记得要链接这个库

  ```cmake
  target_link_libraries(${PROJECT_NAME} PUBLIC sutils)
  ```

  

## VCPKG依赖库:

```
vcpkg install boost zlib bzip2 libzip jsoncpp polyclipping cryptopp libiconv otl curl
```



## 功能列表

TODO:

