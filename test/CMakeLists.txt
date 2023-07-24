cmake_minimum_required(VERSION 3.15)
project(su_test CXX)


find_package(Boost COMPONENTS unit_test_framework REQUIRED)		
add_executable(${PROJECT_NAME} "test_main.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC sutils)
# TODO: 准备添加boost 或者 google test模块以测试所有功能
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::unit_test_framework)
