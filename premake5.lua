-- Adapted from COMP5892M (Advanced Rendering)

workspace "Heatstroke"
    language "C++"
    cppdialect "C++20"
    platforms { "x64" }
    configurations { "Debug", "Release" }

    flags "NoPCH"
    flags "MultiProcessorCompile"

    startproject "Game"

    defines { "CLIENT" }

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
        defines {"OS_LINUX"}

    filter "system:windows"
        defines {"OS_WINDOWS"}
    
    filter "*"

    filter "kind:StaticLib"
        targetdir "lib/"

    filter "kind:ConsoleApp"
        targetdir "bin/"
        targetextension ".exe"

    filter "*"
    
    filter "configurations:Debug"
        symbols "On"
        defines {"_DEBUG=1", "YOJIMBO_DEBUG", "NETCODE_DEBUG", "RELIABLE_DEBUG"}
    
    filter "configurations:Release"
        optimize "On"
        defines {"NDEBUG=1", "YOJIMBO_RELEASE", "NETCODE_RELEASE", "RELIABLE_RELEASE"}

    filter "*"

include "Engine/third_party"

-- GLSLC helpers
dofile("Engine/Utils/glslc.lua")

local cwd = os.getcwd()
local batchFile = cwd .. "/Engine/third_party/vcpkg/vcpkg_setup.bat"
os.execute('"' .. batchFile .. '"')

-- Projects
project "Engine"
    local sources = {
        "Engine/Core/**",
        "Engine/glfw/**",  -- TO BE REMOVED
        "Engine/Input/**", 
        "Engine/Events/**", 
        "Engine/gltf/**",
        "Engine/vulkan/**",
        "Engine/Shaders/**",
        "Engine/Network/**",
        "Engine/ECS/**",
        "Engine/GUI/**",
        "Engine/Physics/**"
    }

    includedirs {
        "Engine/Utils/",
        "Engine/third_party/physx/include",
    }

    libdirs {
        "Engine/third_party/physx/debug/lib",
    }

    kind "StaticLib"
    location "Engine"

    files(sources)
    removefiles("**.vcxproj*")

    links {
        "Utils",
        "yojimbo",
        "sodium-builtin",
        "netcode",
        "reliable",
        "tlsf",
        "x-glm",
        "x-volk",
        "x-glfw",
        "x-vma",
        "imgui",
        "x-tgen"
    }

    filter { "configurations:Debug" }
        links(os.matchfiles("Engine/third_party/physx/debug/lib/*.lib"))
        
        postbuildcommands {
            '{COPY} "%{wks.location}/Engine/third_party/physx/debug/bin/PhysXFoundation_64.dll" "%{wks.location}/bin"',
            '{COPY} "%{wks.location}/Engine/third_party/physx/debug/bin/PhysXCommon_64.dll" "%{wks.location}/bin"',
            '{COPY} "%{wks.location}/Engine/third_party/physx/debug/bin/PhysX_64.dll" "%{wks.location}/bin"'
        }
        
    dependson "Shaders"

project "Game"
    local sources = {
        "Game/**"
    }

    includedirs {
        ".",
        "../",
        "Engine/Utils",
        "Engine/third_party/physx/include"
    }

    kind "ConsoleApp"
    location "Game"

    files(sources)
    removefiles("**.vcxproj*")
    
    links {
        "Engine",
        "Utils",
        "yojimbo",
        "sodium-builtin",
        "netcode",
        "reliable",
        "tlsf",
        "x-glm",
        "x-volk",
        "x-glfw",
        "x-vma",
        "imgui"
    }

    dependson "Engine"

project "Shaders"
    local sources = {
        "Engine/Shaders/**.vert",
        "Engine/Shaders/**.frag",
        "Engine/Shaders/**.geom",
        "Engine/Shaders/**.tesc",
        "Engine/Shaders/**.tese",
        "Engine/Shaders/**.comp"
    }

    kind "Utility"
    location "Engine/Shaders"

    files(sources)

    handle_glsl_files(glslcOptions, "Engine/Shaders/spv", {})

project "Utils"
    local sources = {
        "Engine/Utils/**.cpp",
        "Engine/Utils/**.h*"
    }

    kind "StaticLib"
    location "Engine/Utils"

    files(sources)

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