# FindNetCDF.cmake
# 查找 netCDF C++ 库
# DISABLE_NETCDF_CXX=ON 时跳过查找

message("\n[deps] 检查库 netCDFCxx")

if(DISABLE_NETCDF_CXX)
    message(STATUS "[deps] 禁用 netCDFCxx")
    set(SU_THIRD_SUPPORT_NETCDF_CXX 0)
else()
    find_package(netCDFCxx CONFIG)
    if(netCDFCxx_FOUND)
        message(STATUS "[deps] netCDFCxx found: ${netCDFCxx_FOUND}")
        message(STATUS "[deps] netCDFCxx version: ${netCDFCxx_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES netCDF::netcdf netCDF::netcdf-cxx4)
        set(SU_THIRD_SUPPORT_NETCDF_CXX 1)
    else()
        message(WARNING "[deps] 未发现 netCDFCxx")
        set(SU_THIRD_SUPPORT_NETCDF_CXX 0)
    endif()
endif()
