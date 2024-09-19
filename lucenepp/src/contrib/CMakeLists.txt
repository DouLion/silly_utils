project(contrib)

# ##############################################################################
# create library target
# ##############################################################################
add_subdirectory(include)

set(lucenepp-contrib_SOVERSION "0")

set(lucenepp-contrib_VERSION
    "${LucenePP_VERSION_MAJOR}.${LucenePP_VERSION_MINOR}.${LucenePP_VERSION_PATCH}"
)

# ##############################################################################
# src
# ##############################################################################
file(
  GLOB_RECURSE
  contrib_sources
  "analyzers/*.cpp"
  "highlighter/*.cpp"
  "memory/*.cpp"
  "index/*.cpp"
  "mevc/*.cpp"
  "snowball/*.cpp"
  "snowball/libstemmer_c/libstemmer/libstemmer_utf8.c"
  "snowball/libstemmer_c/src_c/*.c"
  "snowball/libstemmer_c/runtime/*.c")

file(GLOB_RECURSE contrib_headers "${LucenePP_SOURCE_ROOT}/include/*.h")

add_library(lucenepp-contrib STATIC ${contrib_headers} ${contrib_sources})

# ##############################################################################
# include directories
# ##############################################################################
target_include_directories(
  lucenepp-contrib
  PUBLIC ${LucenePP_SOURCE_ROOT}/include
         ${LucenePP_SOURCE_ROOT}/include/lucenepp
         ${LucenePP_SOURCE_ROOT}/src/contrib/include
         ${LucenePP_SOURCE_ROOT}/src/contrib/snowball/libstemmer_c/include)

# ##############################################################################
# dependencies
# ##############################################################################
target_link_libraries(lucenepp-contrib PUBLIC lucenepp)
target_precompile_headers(lucenepp-contrib PRIVATE ${LPP_PRE_COMPILE_HEADERS})

# ##############################################################################
# link args
# ##############################################################################
target_compile_options(lucenepp-contrib PRIVATE -DLPP_BUILDING_LIB)