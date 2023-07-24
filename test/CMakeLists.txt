cmake_minimum_required(VERSION 3.15)
project(su_test CXX)

set(Boost_LIB_PREFIX "")
set(CMAKE_VERBOSE_MAKEFILE ON)
# set(Boost_DEBUG TRUE)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_NO_WARN_NEW_VERSIONS 1)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)		
add_executable(${PROJECT_NAME} "test_main.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC sutils)
# TODO: 准备添加boost 或者 google test模块以测试所有功能
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::unit_test_framework)
