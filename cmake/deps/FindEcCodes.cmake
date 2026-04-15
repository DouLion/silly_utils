message("\n检查库 ECCODES")
if(DISABLE_ECCODES)
    message(STATUS "禁用EcCodes(Grib2)")
else()
    if(DEFINED ENV{ECCODES_ROOT})
        message(STATUS "检测到环境变量ECCODES_ROOT: $ENV{ECCODES_ROOT}")
        set(ECCODES_PREFIX $ENV{ECCODES_ROOT})
        list(APPEND CMAKE_PREFIX_PATH  "${ECCODES_PREFIX}")
    endif()
    find_package(eccodes CONFIG REQUIRED)

    if (NOT "" STREQUAL "${eccodes_BASE_DIR}")
        message(STATUS "eccodes BASE_DIR ${eccodes_BASE_DIR}.")
        message(STATUS "eccodes VERSION ${eccodes_VERSION}.")
        message(STATUS "DEFINITION_PATH ${ECCODES_DEFINITION_PATH}.")
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
            endif ()
            # 拷贝动态库
            message(STATUS "ECCODES_LIBRARY源地址: ${ECCODES_LIBRARY}")
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

            list(APPEND SU_ALL_REQUIRED_LIBRARIES eccodes)
            set(SU_THIRD_SUPPORT_ECCODES 1)
        else (IS_WINDOWS)
            # linux 通过yum 或者 apt-get 安装的, 应该不需要额外配置
            list(APPEND SU_ALL_REQUIRED_LIBRARIES eccodes)
            set(SU_THIRD_SUPPORT_ECCODES 1)
        endif ()
    endif()
endif()