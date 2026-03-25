@echo off
setlocal enabledelayedexpansion

rem 定义 clang-format 的路径
set CLANG_FORMAT_PATH=clang-format.exe

echo 说明：输入目录路径（相对或绝对）以格式化其中的 C/C++ 文件，输入 exit 退出。
echo.

:main_loop
set "TARGET_DIR="
set /p "TARGET_DIR=请输入要遍历的目录 (输入 exit 退出): "

rem 检查是否要退出
if /i "!TARGET_DIR!"=="exit" (
    echo 退出程序。
    goto :eof
)

rem 去除输入可能存在的引号（可选，便于检查目录存在）
set "TARGET_DIR=!TARGET_DIR:"=!"

rem 检查输入是否为空
if "!TARGET_DIR!"=="" (
    echo 目录不能为空，请重新输入。
    goto main_loop
)

rem 检查目录是否存在
if not exist "!TARGET_DIR!" (
    echo 目录 "!TARGET_DIR!" 不存在，请重新输入。
    goto main_loop
)

echo 开始格式化目录 "!TARGET_DIR!" 中的文件...

rem 遍历指定目录下的所有 .cpp, .h, .hpp, .c 文件

for /r "%TARGET_DIR%" %%f in (*.cpp *.h *.hpp *.c *.cc *.hh) do (
    if not "%%~nxf" == "otlv4.h" (
        echo Formatting %%f...
        "!CLANG_FORMAT_PATH!" -i "%%f"
    )
)

echo 完成对目录 "!TARGET_DIR!" 的处理。
echo.
goto main_loop