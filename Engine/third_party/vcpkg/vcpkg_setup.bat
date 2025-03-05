@echo off

cd Engine/third_party/vcpkg

if not exist "vcpkg.exe" (
    echo Running bootstrap-vcpkg.bat...
    call bootstrap-vcpkg.bat
)

set VCPKG_ROOT=%cd%\vcpkg
set PATH=%VCPKG_ROOT%;%PATH%

echo Installing PhysX using vcpkg...
vcpkg install physx

echo Setting up VCPKG_TOOLCHAIN for use with Premake and CMake...
set VCPKG_TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

cd ../../..
