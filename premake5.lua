-- Adapted from COMP5892M (Advanced Rendering)

workspace "Heatstroke"
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

    filter "*"

    -- default options for GLSLC
    glslcOptions = "-O --target-env=vulkan1.2"

    filter "system:linux"
        links "dl"

    filter "system:windows"

    filter "*"

    filter "kind:StaticLib"
        targetdir "lib/"

    filter "kind:ConsoleApp"
        targetdir "bin/"
        targetextension ".exe"

    filter "*"

    filter "Debug"
		symbols "On"
		defines { "_DEBUG=1" }

	filter "Release"
		optimize "On"
		defines { "NDEBUG=1" }

    filter "*"

include "third_party"

-- GLSLC helpers
dofile("Utils/glslc.lua")

-- Projects
project "Engine"
    local sources = {
        "Engine/**"
    }

    includedirs {
        "Utils/"
    }

    kind "StaticLib"
    location "Engine"

    files(sources)
    removefiles("**.vcxproj*")

    dependson "Shaders"

    links "Utils"
    links "x-volk"
    links "x-stb"
    links "x-glfw"
    links "x-vma"

    dependson "x-glm"

project "Game"
    local sources = {
        "Game/**"
    }

    includedirs {
        ".",
        "../",
        "Utils/"

    }

    kind "ConsoleApp"
    location "Game"

    files(sources)
    removefiles("**.vcxproj*")

    links "Engine"
    links "Utils"
    links "x-volk"
    links "x-glfw"
    links "x-vma"

    dependson "Engine"

project "Shaders"
    local sources = {
        "Shaders/**.vert",
        "Shaders/**.frag",
        "Shaders/**.geom",
        "Shaders/**.tesc",
        "Shaders/**.tese",
        "Shaders/**.comp"
    }

    kind "Utility"
    location "Shaders"

    files(sources)

    handle_glsl_files(glslcOptions, "Shaders", {})

project "Utils"
    local sources = {
        "Utils/**.cpp",
        "Utils/**.h*"
    }

    kind "StaticLib"
    location "Utils"

    files(sources)

project()