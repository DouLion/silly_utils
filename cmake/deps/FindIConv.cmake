# FindIConv.cmake
# 查找 Iconv 编码转换库
# 兼容新旧两种使用方式：
#   旧式：include_directories(${Iconv_INCLUDE_DIR}), list(APPEND ... ${Iconv_LIBRARIES})
#   新式：target_link_libraries(... Iconv::Iconv)
# DISABLE_ICONV=ON 时跳过查找

message("\n[deps] 检查库 Iconv")

if(DISABLE_ICONV)
    message(STATUS "[deps] 禁用 Iconv")
    set(SU_THIRD_SUPPORT_ICONV 0)
    set(Iconv_FOUND FALSE)
else()
    # 尝试新式查找 (CONFIG)
    find_package(Iconv CONFIG QUIET)
    
    if(Iconv_FOUND)
        # 获取属性（兼容旧式代码）
        get_target_property(Iconv_INCLUDE_DIR Iconv::Iconv INTERFACE_INCLUDE_DIRECTORIES)
        get_target_property(Iconv_LIBRARIES Iconv::Iconv IMPORTED_LOCATION_RELEASE)
        
        # 如果没找到 Release 版本，尝试 Debug
        if(NOT Iconv_LIBRARIES)
            get_target_property(Iconv_LIBRARIES Iconv::Iconv IMPORTED_LOCATION_DEBUG)
        endif()
        
        # 设置旧式变量（向后兼容）
        if(NOT Iconv_INCLUDE_DIR)
            # 如果 CONFIG 模式没提供，尝试旧式查找 fallback
            find_package(Iconv QUIET)
        endif()
        
        if(Iconv_INCLUDE_DIR)
            message(STATUS "[deps] Iconv found: TRUE")
            message(STATUS "[deps] Iconv include: ${Iconv_INCLUDE_DIR}")
            if(Iconv_LIBRARIES)
                message(STATUS "[deps] Iconv libs: ${Iconv_LIBRARIES}")
            endif()
            set(SU_THIRD_SUPPORT_ICONV 1)
            
            # 如果旧式查找也成功，覆盖变量
            if(NOT Iconv_LIBRARIES)
                set(Iconv_LIBRARIES "Iconv::Iconv")
            endif()
        else()
            message(WARNING "[deps] 未发现 Iconv (CONFIG 模式未找到 include dir)")
            set(SU_THIRD_SUPPORT_ICONV 0)
            set(Iconv_FOUND FALSE)
        endif()
    else()
        # Fallback: 旧式查找
        find_package(Iconv QUIET)
        if(Iconv_FOUND)
            message(STATUS "[deps] Iconv found: TRUE (旧式查找)")
            message(STATUS "[deps] Iconv include: ${Iconv_INCLUDE_DIR}")
            message(STATUS "[deps] Iconv libs: ${Iconv_LIBRARIES}")
            set(SU_THIRD_SUPPORT_ICONV 1)
        else()
            message(WARNING "[deps] 未发现 Iconv")
            set(SU_THIRD_SUPPORT_ICONV 0)
            set(Iconv_FOUND FALSE)
        endif()
    endif()
endif()
