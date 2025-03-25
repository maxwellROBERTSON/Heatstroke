@echo off
setlocal enabledelayedexpansion

cd Engine/third_party/vcpkg

if not exist "vcpkg.exe" (
    echo Running bootstrap-vcpkg.bat...
    call bootstrap-vcpkg.bat
)

set "VCPKG_ROOT=%CD%"
set PATH=%VCPKG_ROOT%;%PATH%

REM Run vcpkg list and find PhysX
for /f "delims=" %%i in ('vcpkg list ^| findstr "physx"') do set INSTALLED=%%i

REM Check if PhysX is installed
if not defined INSTALLED (
    echo PhysX is not installed. Installing...
    .\vcpkg install physx
) else (
    echo PhysX is already installed:
    echo %INSTALLED%
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

REM Check if Vulkan SDK is installed via system environment variables
set VOLK_SDK=%VOLK_SDK%
if not defined VOLK_SDK (
    REM Check if volk is installed via vcpkg
    echo here
    for /f "delims=" %%i in ('vcpkg list ^| findstr "volk"') do set INSTALLED_VOLK=%%i
    if not defined INSTALLED_VOLK (
        echo Volk SDK is not installed.
        echo "Do you want to install Volk SDK using vcpkg? (Y/N):"
        set /p INSTALL_VOLK="Enter Y or N: "
        REM Check user's input for installation
        if /i "!INSTALL_VOLK!"=="Y" (
            echo Installing volk SDK...
            .\vcpkg install volk
        ) else (
            echo !INSTALL_VOLK!
            echo Skipping volk SDK installation.
        )
    )
) else (
    echo Volk SDK is already set in environment variable: %VOLK_SDK%
)

echo Setting up VCPKG_TOOLCHAIN for use with Premake and CMake...
set VCPKG_TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

cd ../../..
