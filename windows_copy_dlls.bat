@echo off
setlocal

:: Input args
set "WKS=%~1"
set "CONFIG=%~2"

:: Default to empty path suffix
set "PATH_SUFFIX="

:: Check if CONFIG is Debug (case-insensitive)
if /I "%CONFIG%"=="Debug" (
    set "PATH_SUFFIX=\\debug"
)

:: Define paths using the suffix
set "SRC_BASE=%WKS%Engine\\third_party\\vcpkg\\installed\\x64-windows%PATH_SUFFIX%\\bin"
set "DST_BASE=%WKS%bin%PATH_SUFFIX%"

:: List of DLLs to copy
set DLL_LIST=^
PhysXFoundation_64.dll ^
PhysXCommon_64.dll ^
PhysX_64.dll ^
PhysXCooking_64.dll ^
OpenAL32.dll ^
sndfile.dll ^
ogg.dll ^
vorbis.dll ^
vorbisenc.dll ^
FLAC.dll ^
opus.dll ^
mpg123.dll ^
libmp3lame.dll

if not exist "%DST_BASE%" (
    echo Creating debug folder...
    mkdir "%DST_BASE%"
)

:: Loop through DLLs
for %%D in (%DLL_LIST%) do (
    if not exist "%DST_BASE%\\%%D" (
        if exist "%SRC_BASE%\\%%D" (
            echo Copying %%D...
            copy /Y "%SRC_BASE%\\%%D" "%DST_BASE%\\"
        )
    )
)

set "SRC_BASE=%WKS%Engine\\third_party\\vcpkg\\installed\\x64-windows\\tools"

if not exist "%DST_BASE%\\PhysXGpu_64.dll" (
    if exist "%SRC_BASE%\\PhysXGpu_64.dll" (
        echo Copying PhysXGpu_64.dll...
        copy /Y "%SRC_BASE%\\PhysXGpu_64.dll" "%DST_BASE%\\"
    )
)

endlocal
