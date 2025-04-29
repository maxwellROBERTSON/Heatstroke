@echo off
setlocal enabledelayedexpansion

cd Engine/third_party/vcpkg

if not exist "vcpkg.exe" (
    echo Running bootstrap-vcpkg.bat...
    call bootstrap-vcpkg.bat
)

set "VCPKG_ROOT=%CD%"
set PATH=%VCPKG_ROOT%;%PATH%

REM Run vcpkg list and find physx
for /f "delims=" %%i in ('vcpkg list ^| findstr "physx"') do set INSTALLED_PHYSX=%%i

REM Check if physx is installed
if not defined INSTALLED_PHYSX (
    echo physx is not installed. Installing...
    .\vcpkg install physx
) else (
    echo physx is already installed:
    echo %INSTALLED_PHYSX%
)

REM Run vcpkg list and find openal
for /f "delims=" %%i in ('vcpkg list ^| findstr "openal-soft"') do set INSTALLED_OPENAL=%%i

REM Check if openal is installed
if not defined INSTALLED_OPENAL (
    echo openal is not installed. Installing...
    .\vcpkg install openal-soft
) else (
    echo openal is already installed:
    echo %INSTALLED_OPENAL%
)

REM Run vcpkg list and find libsndfile
for /f "delims=" %%i in ('vcpkg list ^| findstr "libsndfile"') do set INSTALLED_LIBSNDFILE=%%i

REM Check if libsndfile is installed
if not defined INSTALLED_LIBSNDFILE (
    echo libsndfile is not installed. Installing...
    .\vcpkg install libsndfile
) else (
    echo libsndfile is already installed:
    echo %INSTALLED_LIBSNDFILE%
)

REM Check if Vulkan SDK is installed via system environment variables
set VULKAN_SDK=%VULKAN_SDK%
if not defined VULKAN_SDK (
    REM Check if Vulkan is installed via vcpkg
    echo here
    for /f "delims=" %%i in ('vcpkg list ^| findstr "vulkan"') do set INSTALLED_VULKAN=%%i
    if not defined INSTALLED_VULKAN (
        echo Vulkan SDK is not installed.
        echo "Do you want to install Vulkan SDK using vcpkg? (Y/N):"
        set /p INSTALL_VULKAN="Enter Y or N: "
        REM Check user's input for installation
        if /i "!INSTALL_VULKAN!"=="Y" (
            echo Installing Vulkan SDK...
            .\vcpkg install vulkan
        ) else (
            echo !INSTALL_VULKAN!
            echo Skipping Vulkan SDK installation.
        )
    )
) else (
    echo Vulkan SDK is already set in environment variable: %VULKAN_SDK%
)

echo Setting up VCPKG_TOOLCHAIN for use with Premake and CMake...
set VCPKG_TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

cd ../../..
