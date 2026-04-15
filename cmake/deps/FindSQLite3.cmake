# FindSQLite3.cmake
# 查找 SQLite3 数据库
# DISABLE_SQLITE3=ON 时跳过查找

message("\n[deps] 检查库 unofficial-sqlite3")

if(DISABLE_SQLITE3)
    message(STATUS "[deps] 禁用 SQLITE3")
    set(SU_THIRD_SUPPORT_SQLITE3 0)
else()
    # vcpkg 提供的是 unofficial-sqlite3
    find_package(unofficial-sqlite3 CONFIG)
    if(unofficial-sqlite3_FOUND)
        message(STATUS "[deps] SQLite3 found: ${unofficial-sqlite3_FOUND}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES unofficial::sqlite3::sqlite3)
        set(SU_THIRD_SUPPORT_SQLITE3 1)
    else()
        message(WARNING "[deps] 未发现 SQLite3")
        set(SU_THIRD_SUPPORT_SQLITE3 0)
    endif()
endif()
