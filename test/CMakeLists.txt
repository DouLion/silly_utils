cmake_minimum_required(VERSION 3.15)
project(su_test CXX)

foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
  file(
    GLOB_RECURSE SU_TEST_SRC_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "units/${dirname}/*.h" "units/${dirname}/*.hpp" "units/${dirname}/*.c" "units/${dirname}/*.cpp")
endforeach()

if(CATCH2_UNITS_DISABLED)
  add_executable(${PROJECT_NAME} test.cpp)
   find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
else()
  add_executable(${PROJECT_NAME} ${SU_TEST_SRC_FILES} test.cpp)
  add_definitions("-DCATCH2_UNIT_TEST")

endif()

target_link_libraries(${PROJECT_NAME} PUBLIC sutils)
find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Catch2::Catch2
                                             Catch2::Catch2WithMain)
