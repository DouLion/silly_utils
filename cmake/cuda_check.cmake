if(ENABLE_CUDA)
set(CMAKE_CUDA_ARCHITECTURES "70")
set(CMAKE_CXX_STANDARD 17)
# 设置CUDA Toolkit路径（如果自动检测不工作）

if("" STREQUAL "$ENV{CUDA_PATH}")
    MESSAGE(FAULT "需要添加环境变量 CUDA_PATH  如 C:/Program Files\NVIDIA GPU Computing Toolkit/CUDA/v12.5 ")
    add_definitions("-DSILLY_ENABLE_CUDA=0")
else()
    # 从环境变量中取 CUDA的根目录
    set(TMP_CUDA_ROOT "$ENV{CUDA_PATH}")
    # replace_backslashes(${TMP_CUDA_ROOT} MY_CUDA_ROOT)
    string(REPLACE "\\" "/" MY_CUDA_ROOT "${TMP_CUDA_ROOT}")
    MESSAGE("${MY_CUDA_ROOT}")
    set(CMAKE_CUDA_COMPILER "${CUDAToolkit_NVCC_EXECUTABLE}")

    # set(CMAKE_CUDA_COMPILER_WORKS TRUE) # 跳过检查
    # 查找CUDA Toolkit
    find_package(CUDAToolkit REQUIRED)
    # 如果找到了, 就定义ACTIVE_GPU=1
    if(CUDAToolkit_FOUND)
        message(STATUS ">>>>>>>>>>>>>>>> CUDA <<<<<<<<<<<<<<<<")
        message(STATUS "CUDA ROOT ${MY_CUDA_ROOT}.")
        message(STATUS "CUDAToolkit Include ${CUDAToolkit_INCLUDE_DIRS}.")
        message(STATUS "CUDAToolkit libs ${CUDAToolkit_LIBRARY_DIR}.")
        message(STATUS "CUDAToolkit_NVCC_EXECUTABLE ${CUDAToolkit_NVCC_EXECUTABLE}.")
        message(STATUS ">>>>>>>>>>>>>>>> CUDA <<<<<<<<<<<<<<<<\n")
        add_definitions("-DSILLY_ENABLE_CUDA=1")
        include_directories(${CUDAToolkit_INCLUDE_DIRS})
    endif()

endif()
endif()
