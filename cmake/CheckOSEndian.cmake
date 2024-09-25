
# 检查字节序
include(CheckCXXSourceCompiles)
message("\n检查字节序")
# 测试小端序
set(ENDIAN_TEST_CODE "
#include <cstdint>
#include <iostream>

int main() {
    uint32_t num = 1;
    return (*(reinterpret_cast<uint8_t*>(&num)) == 1) ? 0 : 1;
}
")

check_cxx_source_compiles("${ENDIAN_TEST_CODE}" IS_LITTLE_ENDIAN)
message(STATUS ">>>>>>>>>>>>>>>> ENDIAN <<<<<<<<<<<<<<<<")
if(IS_LITTLE_ENDIAN)
    message(STATUS "系统是小端序(Little endian).")
else()
    message(STATUS "系统是大端序(Big endian).")
endif()

message(STATUS ">>>>>>>>>>>>>>>> ENDIAN <<<<<<<<<<<<<<<<")