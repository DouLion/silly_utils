
add_library(AsciiGridImg SHARED "ascii_grid_image.h" "ascii_grid_image.cpp")

target_link_libraries(AsciiGridImg sutils)