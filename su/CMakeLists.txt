cmake_minimum_required(VERSION 3.15)

foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE SU_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            "${dirname}/*.h"
            "${dirname}/*.hpp"
            "${dirname}/*.c"
            "${dirname}/*.cpp"
            )
   
endforeach()


find_path(OTL_INCLUDE_DIRS "otl/otlv4.h")
include_directories(${OTL_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC ${SU_SRC_FILES})

### Packages 
find_package(Boost REQUIRED COMPONENTS system thread date_time chrono regex filesystem unit_test_framework program_options iostreams locale serialization )
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost Boost::system Boost::thread Boost::date_time Boost::chrono Boost::regex Boost::filesystem Boost::unit_test_framework Boost::program_options Boost::iostreams Boost::locale Boost::serialization) 

find_package(cryptopp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC cryptopp::cryptopp)

find_package(Iconv)
target_link_libraries(${PROJECT_NAME} PUBLIC Iconv::Iconv)

find_package(polyclipping CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC polyclipping::polyclipping)