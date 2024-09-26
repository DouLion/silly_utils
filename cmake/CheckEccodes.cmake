find_package(eccodes CONFIG REQUIRED)
message("\n检查ECCODES")
if(DEFINED ENV{ECCODES_ROOT})
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};$ENV{ECCODES_ROOT}")
    message(STATUS "检测到环境变量ECCODES_ROOT: $ENV{ECCODES_ROOT}")
endif()
if (NOT "" STREQUAL "${eccodes_BASE_DIR}")

  message(STATUS ">>>>>>>>>>>>>>>> ECCODES <<<<<<<<<<<<<<<<")
  message(STATUS "BASE_DIR ${eccodes_BASE_DIR}.")
  message(STATUS "VERSION ${eccodes_VERSION}.")
  message(STATUS "DEFINITION_PATH ${ECCODES_DEFINITION_PATH}.")
  message(STATUS ">>>>>>>>>>>>>>>> ECCODES <<<<<<<<<<<<<<<<")
  if(IS_WINDOWS)

    if(EXISTS ${ECCODES_DEBUG_LIBRARY} AND DEBUG_MODE)
      set(ECCODES_LIBRARY "${ECCODES_DEBUG_LIBRARY}")
    endif()
     if(EXISTS ${ECCODES_RELEASE_LIBRARY} AND NOT DEBUG_MODE)
        set(ECCODES_LIBRARY "${ECCODES_RELEASE_LIBRARY}")
    endif()
    if(NOT EXISTS "${ECCODES_LIBRARY}" OR "" STREQUAL "${ECCODES_LIBRARY}")
      message(FATAL_ERROR "Windows平台检测不到 ECCODES_LIBRARY")
    endif()

    # 有VCPKG的情况
    if("" STREQUAL "${VCPKG_INSTALLED_DIR}")
      message(WARNING "没有检测到VCPKG, 需要手动拷贝依赖库 jasper 和 openjp2")
    else ("" STREQUAL "${VCPKG_INSTALLED_DIR}")
      # 检查依赖库
      find_package(OpenJPEG CONFIG REQUIRED)
      target_link_libraries(openjp2)
    endif ()
    # 拷贝动态库
    message(STATUS "ECCODES_LIBRARY源地址: ${ECCODES_LIBRARY}\n")
    get_filename_component(ECCODES_LIB_FILENAME ${ECCODES_LIBRARY} NAME)
    get_filename_component(ECCODES_LIB_DIR ${ECCODES_LIBRARY} DIRECTORY)
    execute_process(
            COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            "${ECCODES_LIBRARY}"
            "${EXECUTABLE_OUTPUT_PATH}/${ECCODES_LIB_FILENAME}")
    execute_process(
            COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            "${ECCODES_LIB_DIR}/jasper.dll"
            "${EXECUTABLE_OUTPUT_PATH}/jasper.dll")

    add_compile_options(-DSU_ECCODES_ENABLED)
  else (IS_WINDOWS)
    # linux 通过yum 或者 apt-get 安装的, 应该不需要额外配置
  endif ()

endif()

