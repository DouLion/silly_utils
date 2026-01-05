cmake_minimum_required(VERSION 3.15)

project(sutils CXX)

foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
  file(
    GLOB_RECURSE SU_SRC_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "${dirname}/*.h" "${dirname}/*.hpp" "${dirname}/*.c" "${dirname}/*.cpp")

endforeach()

add_library(${PROJECT_NAME} STATIC ${SU_SRC_FILES})

#[[target_precompile_headers(
        ${PROJECT_NAME} PUBLIC
        ${SU_FILE_ROOT}/su/su_marco.h
        ${SU_FILE_ROOT}/su/geo/silly_geo.h
        ${SU_FILE_ROOT}/su/log/silly_log.h
        ${SU_FILE_ROOT}/su/string/silly_format.h
        ${SU_FILE_ROOT}/su/database/otl/silly_otl.h
        ${SU_FILE_ROOT}/su/datetime/silly_posix_time.h
        ${SU_FILE_ROOT}/su/json/silly_jsonpp.h
)]]

target_link_libraries(${PROJECT_NAME} PUBLIC ${SU_ALL_REQUIRED_LIBRARIES})


