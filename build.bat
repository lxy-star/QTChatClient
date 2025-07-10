@echo off
REM --------- 清理旧的 build 文件夹 ---------
if exist build (
    echo Deleting old build directory...
    rmdir /s /q build
)

REM --------- 创建新的 build 文件夹 ---------
mkdir build
cd build

REM --------- 运行 cmake 生成 MinGW Makefiles ---------
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH="D:/msys64/mingw64" ^
    -DCMAKE_BUILD_TYPE=Debug
REM --------- 编译 ---------
mingw32-make

@REM REM --------- 编译成功后运行 ---------
@REM if exist QtChatClient.exe (
@REM     echo Running QtChatClient.exe...
@REM     QtChatClient.exe
@REM ) else (
@REM     echo Build failed: QtChatClient.exe not found.
@REM )

REM --------- 返回项目根目录 ---------
cd ..
