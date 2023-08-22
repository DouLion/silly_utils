project(tzx_image_tools)

add_executable(${PROJECT_NAME} tzx_image_tool.cpp)

target_link_libraries(${PROJECT_NAME} sutils)