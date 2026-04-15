# SUConfig.cmake
# 配置 silly_utils 库的构建选项和宏定义

message("\n===== 配置 silly_utils =====")

# 生成 su_config.h
configure_file(
    ${SU_FILE_ROOT}/su/su_config.h.in
    ${SU_FILE_ROOT}/su/su_config.h
    @ONLY
)

# 设置输出目录
set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
if(DEBUG_MODE)
    add_definitions(-DPROJECT_ROOT="${PROJECT_ROOT}")
else()
    add_definitions(-DPROJECT_ROOT="./")
endif()

# 输出目录设置
set(EXECUTABLE_OUTPUT_PATH
    "${PROJECT_ROOT}/Bin/${PLATFORM_NAME}/${ARCH_NAME}/${CMAKE_BUILD_TYPE}")
set(LIBRARY_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}")

# 包含目录
if(BUILD_AS_PROJ)
    include_directories(${SU_PROJ_ROOT}/su)
    add_definitions(-DDEFAULT_SU_ROOT_DIR="${SU_PROJ_ROOT}")
    add_definitions(-DDEFAULT_SU_DATA_DIR="${SU_PROJ_ROOT}/data")
else()
    include_directories(${SU_FILE_ROOT}/su)
endif()

message("===== 配置完成 =====\n")
