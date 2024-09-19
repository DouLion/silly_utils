####################################
# include directories
####################################

add_subdirectory(config_h)
add_subdirectory(lucenepp)

file(GLOB_RECURSE LPP_PRE_COMPILE_HEADERS "${LucenePP_SOURCE_ROOT}/include/lucenepp/*.h")


message(STATUS "PRE _HEADER   ${LPP_PRE_COMPILE_HEADERS}")