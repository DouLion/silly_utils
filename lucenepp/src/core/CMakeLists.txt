# ##############################################################################
# src
# ##############################################################################
file(
  GLOB_RECURSE
  lucene_sources
  "search/*.cpp"
  "analysis/*.cpp"
  "document/*.cpp"
  "index/*.cpp"
  "queryparser/*.cpp"
  "store/*.cpp"
  "util/*.c*")

file(GLOB_RECURSE lucene_internal_headers "${LucenePP_SOURCE_ROOT}/include/lucenepp/*.h")
message(STATUS "lucene_internal_headers \n ${lucene_internal_headers} \n --end")

add_library(lucenepp STATIC ${lucene_sources} ${lucene_internal_headers})
# ##############################################################################
# include directories
# ##############################################################################
target_include_directories(lucenepp
                           PUBLIC "${LucenePP_SOURCE_ROOT}/include/lucenepp")
target_include_directories(lucenepp
                           PUBLIC "${LucenePP_SOURCE_ROOT}/src/core/include")
target_include_directories(lucenepp
                           PUBLIC "${LucenePP_SOURCE_ROOT}/src/core/util/md5")
target_include_directories(
  lucenepp PUBLIC "${LucenePP_SOURCE_ROOT}/src/core/util/unicode")

target_precompile_headers(lucenepp PUBLIC ${LPP_PRE_COMPILE_HEADERS})

# ##############################################################################
# dependencies
# ##############################################################################
target_link_libraries(
  lucenepp
  PUBLIC Boost::boost
         Boost::date_time
         Boost::filesystem
         Boost::iostreams
         Boost::regex
         Boost::system
         Boost::thread
         ZLIB::ZLIB)

if(WIN32)
  target_link_libraries(lucenepp PUBLIC ws2_32)
endif()

target_compile_options(lucenepp PRIVATE -DLPP_BUILDING_LIB)
