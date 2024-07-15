cmake_minimum_required (VERSION 3.15)

project(su_example CXX)

add_subdirectory(example_tool)
link_libraries(tool_lib)

add_executable(${PROJECT_NAME}  "main.cpp" )

#target_link_libraries(${PROJECT_NAME} PUBLIC sutils)






