# CheckOSEndian.cmake
# 检查系统字节序（小端/大端）
# 设置：SU_LITTLE_ENDIAN (1) 或 SU_BIG_ENDIAN (1)
# 同时设置宏：IS_LITTLE_ENDIAN 或 IS_BIG_ENDIAN

message("\n===== 检查字节序 =====")

# 使用 CMake 内置检查，编译并运行测试代码
include(CheckCXXSourceCompiles)

set(ENDIAN_TEST_CODE "
#include <cstdint>

int main() {
    uint32_t num = 1;
    // 如果最低字节是 1，则是小端序
    return (*(reinterpret_cast<uint8_t*>(&num)) == 1) ? 0 : 1;
}
")

check_cxx_source_compiles("${ENDIAN_TEST_CODE}" ENDIAN_TEST_RESULT)

if(ENDIAN_TEST_RESULT)
    message(STATUS "系统是小端序 (Little endian).")
    set(SU_LITTLE_ENDIAN 1)
    set(SU_BIG_ENDIAN 0)
    add_definitions(-DIS_LITTLE_ENDIAN)
else()
    message(STATUS "系统是大端序 (Big endian).")
    set(SU_LITTLE_ENDIAN 0)
    set(SU_BIG_ENDIAN 1)
    add_definitions(-DIS_BIG_ENDIAN)
endif()

message("===== 字节序检查完成 =====\n")
