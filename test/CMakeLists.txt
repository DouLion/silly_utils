cmake_minimum_required(VERSION 3.15)
project(su_test CXX)

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC silly_utils)

