cmake_minimum_required (VERSION 3.15)

# 添加任务应用
add_executable(TaskA  "task_a_app.cpp" )
target_link_libraries(TaskA PUBLIC core_lib)

add_executable(TaskB  "task_b_app.cpp" )
target_link_libraries(TaskB PUBLIC core_lib)




#target_link_libraries(${PROJECT_NAME} PUBLIC sutils)






