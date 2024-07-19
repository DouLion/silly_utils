project(AsciiGridImg CXX)

foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE ASCII_GRID_IMG_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            "${dirname}/*.h"
            "${dirname}/*.hpp"
            "${dirname}/*.c"
            "${dirname}/*.cpp"
    )

endforeach()


add_library(AsciiGridImg SHARED ${ASCII_GRID_IMG_SRC_FILES})

target_link_libraries(AsciiGridImg sutils)