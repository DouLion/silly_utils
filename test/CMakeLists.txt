cmake_minimum_required(VERSION 3.15)
project(su_test CXX)

foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE SU_TEST_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            "${dirname}/*.h"
            "${dirname}/*.hpp"
            "${dirname}/*.c"
            "${dirname}/*.cpp"
            )
   
endforeach()


find_package(Boost REQUIRED COMPONENTS unit_test_framework )
add_executable(${PROJECT_NAME} ${SU_TEST_SRC_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC sutils)
# TODO: 准备添加boost 或者 google test模块以测试所有功能
if(Boost_FOUND)
    message(STATUS "Found Boost version ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
endif()

find_package(httplib CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC httplib::httplib)