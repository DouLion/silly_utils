if(IS_WINDOWS)
    add_executable(WinMouseClick windows/mouse_click.cpp)
    target_link_libraries(WinMouseClick sutils)
endif()

add_executable(GridToRect gird_rect/grid_to_rect.cpp)
target_link_libraries(GridToRect sutils)

add_executable(TritonCvtDeep triton/convert_deepth.cpp)
target_link_libraries(TritonCvtDeep sutils)

set(RWDB_SOURCES
    RWDB/rwdb_base.h
    RWDB/rwdb_base.cpp
    RWDB/rwdb_stbprp.h
    RWDB/rwdb_stbprp.cpp
    RWDB/rwdb_pptn.h
    RWDB/rwdb_pptn.cpp
    RWDB/rwdb_river.h
    RWDB/rwdb_river.cpp
    RWDB/rwdb_rsvr.h
    RWDB/rwdb_rsvr.cpp
)
add_executable(TzxODBCExport RWDB/TzxODBCExport.cpp ${RWDB_SOURCES})
target_link_libraries(TzxODBCExport sutils)

add_executable(TzxODBCImport RWDB/TzxODBCImport.cpp ${RWDB_SOURCES})
target_link_libraries(TzxODBCImport sutils)

add_executable(TzxODBCCompare RWDB/compare_db.cpp)
target_link_libraries(TzxODBCCompare sutils)

add_executable(2DTask 2DTask/2DTask.cpp)
target_link_libraries(2DTask sutils)

add_executable(ListHeader header/ListHeader.cpp)
target_link_libraries(ListHeader sutils)

add_executable(TzxNTP NTP/TzxNTP.cpp)
target_link_libraries(TzxNTP sutils)

add_subdirectory(ODBCExchange)



