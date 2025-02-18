-- Adapted from COMP5892M (Advanced Rendering)

workspace "Heatstroke-Server"
    language "C++"
    cppdialect "C++20"

    platforms { "x64" }
    configurations { "Debug", "Release" }

    flags "NoPCH"
    flags "MultiProcessorCompile"

    startproject "Game"

    debugdir "%{wks.location}"
    objdir "build/%{cfg.buildcfg}-%{cfg.platform}-%{cfg.toolset}"
    targetsuffix "-%{cfg.buildcfg}-%{cfg.platform}-%{cfg.toolset}"

    filter "toolset:gcc or toolset:clang"
        linkoptions { "-pthread" }
        buildoptions { "-march=native", "-Wall", "-pthread" }

    filter "toolset:msc-*"
        defines { "_CRT_SECURE_NO_WARNINGS=1" }
        defines { "_SCL_SECURE_NO_WARNINGS=1" }
        buildoptions { "/utf-8" }

    filter "configurations:Debug"
        symbols "On"
        defines {"DEBUG", "YOJIMBO_DEBUG", "NETCODE_DEBUG", "RELIABLE_DEBUG"}
    filter "configurations:Release"
        symbols "Off"
        optimize "On"
        defines {"YOJIMBO_RELEASE", "NETCODE_RELEASE", "RELIABLE_RELEASE"}
    filter "system:windows"
        defines {"OS_WINDOWS"}
    filter "system:linux"
        defines {"OS_LINUX"}
        links "dl"

    filter "kind:StaticLib"
        targetdir "lib/"

    filter "kind:ConsoleApp"
        targetdir "bin/"
        targetextension ".exe"

    filter "*"

include "third_party"

-- Projects
project "Engine"
    local sources = {
        "Engine/**"
    }

    kind "StaticLib"
    location "Engine"

    files(sources)
    removefiles("**.vcxproj*")

    dependson "Yojimbo"

project "Game"
    local sources = {
        "Game/**"
    }

    includedirs {
        ".",
        "../"
    }

    kind "ConsoleApp"
    location "Game"

    files(sources)
    removefiles("**.vcxproj*")
    
    links "Engine"
    links "Yojimbo"

    dependson "Engine"

project()

-- Custom Clean Action
newaction {
    trigger = "clean",
    description = "Remove all generated binaries and intermediate files",
    execute = function()
        print("Cleaning project...")
        os.rmdir("bin")
        os.rmdir("build")
        os.rmdir("lib")
        os.remove("**.make")  -- Remove Makefiles
        os.remove("**.workspace")  -- Remove workspace files
        os.remove("**.sln")  -- Remove Visual Studio solution
        os.remove("**.vcxproj*")  -- Remove Visual Studio project files
        print("Clean complete!")
    end
}