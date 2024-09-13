if(IS_WINDOWS)
    add_executable(WinMouseClick windows/mouse_click.cpp)
    target_link_libraries(WinMouseClick sutils)
endif()

add_executable(GridToRect gird_rect/grid_to_rect.cpp)
target_link_libraries(GridToRect sutils)

add_executable(TritonCvtDeep triton/convert_deepth.cpp)
target_link_libraries(TritonCvtDeep sutils)