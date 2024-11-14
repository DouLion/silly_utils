if(IS_WINDOWS)
    add_executable(WinMouseClick windows/mouse_click.cpp)
    target_link_libraries(WinMouseClick sutils)
endif()

add_executable(GridToRect gird_rect/grid_to_rect.cpp)
target_link_libraries(GridToRect sutils)

add_executable(TritonCvtDeep triton/convert_deepth.cpp)
target_link_libraries(TritonCvtDeep sutils)

add_executable(TzxODBCExport RWDB/TzxODBCExport.cpp RWDB/silly_rwdb_export.h RWDB/silly_rwdb_export.cpp)
target_link_libraries(TzxODBCExport sutils)

add_executable(TzxODBCImport RWDB/TzxODBCImport.cpp RWDB/silly_rwdb_import.h RWDB/silly_rwdb_import.cpp)
target_link_libraries(TzxODBCImport sutils)

add_executable(TzxODBCCompare RWDB/compare_db.cpp)
target_link_libraries(TzxODBCCompare sutils)

add_executable(2DTask 2DTask/2DTask.cpp)
target_link_libraries(2DTask sutils)
