@echo off
setlocal enabledelayedexpansion

rem 定义 clang-format 的路径
set CLANG_FORMAT_PATH=clang-format.exe

rem 查找当前目录下的 su 子目录中的所有 .cpp, .h, .hpp, .c 文件
for /r su %%f in (*.cpp *.h *.hpp *.c) do (
  if not "%%~nxf" == "otlv4.h" (
    echo Formatting %%f...
    "!CLANG_FORMAT_PATH!" -i "%%f"
  )
)

echo Done!
pause