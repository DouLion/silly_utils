project(write_color_box)

add_executable(${PROJECT_NAME} write_color_box.cpp)

# -----------------查找CAIRO开始------------------
find_path(CAIRO_INCLUDE_DIR cairo/cairo.h)
set(CAIRO_NAMES ${JRTPLIB_NAMES} cairo)
find_library(CAIRO_LIBRARY NAMES ${CAIRO_NAMES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CAIRO_LIBRARY})

# linux 上可能链接有点问题需要主动设置下面的项
if(IS_LINUX)
	find_package(Fontconfig REQUIRED) # since CMake 3.14
	target_link_libraries(${PROJECT_NAME} PUBLIC Fontconfig::Fontconfig)
	find_package(Freetype REQUIRED)
	target_link_libraries(${PROJECT_NAME} PUBLIC Freetype::Freetype) # since CMake 3.10
	find_package(PNG REQUIRED)
	target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)
	find_library(PIXMAN_LIBRARY NAMES pixman-1)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${PIXMAN_LIBRARY})
endif()
# -----------------查找CAIRO结束------------------

target_link_libraries(${PROJECT_NAME} PUBLIC sutils)