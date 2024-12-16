
project(ODBCExchange CXX)
foreach(dir ${CMAKE_CURRENT_SOURCE_DIR})
    file(
            GLOB_RECURSE ODBC_EXCHANGE_SRC_FILES
            RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}}
            "${dir}/*.h" "${dir}/*.hpp" "${dir}/*.c" "${dir}/*.cpp")
endforeach()

message(STATUS ${ODBC_EXCHANGE_SRC_FILES})

add_executable(${PROJECT_NAME} ${ODBC_EXCHANGE_SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} sutils)