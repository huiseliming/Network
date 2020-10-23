@echo off
cd /d %~dp0
rem if not exist build ( mkdir build )
rem cd build
echo off
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
rem Generate VS2019 project.
cmake -G "Visual Studio 16 2019" -A "x64" -S . -B out/build "-DCMAKE_TOOLCHAIN_FILE=F:/vcpkg/scripts/buildsystems/vcpkg.cmake" 
