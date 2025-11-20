message("\n检查库 SPATIALITE")

find_package(hiredis CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC hiredis::hiredis)
