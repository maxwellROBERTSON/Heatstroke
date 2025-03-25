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

    local cwd = os.getcwd()
    local shellScript = cwd .. "/Engine/third_party/vcpkg/vcpkg_setup.sh"
    local batchFile = cwd .. "/Engine/third_party/vcpkg/vcpkg_setup.bat"

    if os.istarget("linux") then
        os.execute('sh "' .. shellScript .. '"')
    end
    if os.istarget("windows") then
        os.execute('"' .. batchFile .. '"')
    end

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

-- Projects
project "Engine"
    local sources = {
        "Engine/Core/**",
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
        "Engine/Utils/"
    }
    
    kind "StaticLib"
    location "Engine"

    -- includedirs { "Engine/third_party/vcpkg/packages/physx_x64-linux/include/physx" }

    filter "*"

    filter "system:linux"
        includedirs { "Engine/third_party/vcpkg/packages/physx_x64-linux/include/physx" }

    filter "system:windows"
        includedirs { "Engine/third_party/vcpkg/packages/physx_x64-windows/include/physx" }

    filter "*"
    
    files(sources)
    removefiles("**.vcxproj*")

    libdirs { "Engine/third_party/vcpkg/packages/physx_x64-linux/debug/lib" }
    links(os.matchfiles("Engine/third_party/vcpkg/packages/physx_x64-linux/debug/lib/*.lib"))

    filter "*"
        
    filter { "system:linux", "configurations:Debug" }
        libdirs { "Engine/third_party/vcpkg/packages/physx_x64-linux/debug/lib" }
        links(os.matchfiles("Engine/third_party/vcpkg/packages/physx_x64-linux/debug/lib/*.lib"))
    filter { "system:linux", "configurations:Release" }
        libdirs { "Engine/third_party/vcpkg/packages/physx_x64-linux/lib" }
        links(os.matchfiles("Engine/third_party/vcpkg/packages/physx_x64-linux/lib/*.lib"))

    filter { "system:windows", "configurations:Debug" }
        libdirs { "Engine/third_party/vcpkg/packages/physx_x64-windows/debug/lib" }
        links(os.matchfiles("Engine/third_party/vcpkg/packages/physx_x64-windows/debug/lib/*.lib"))
        postbuildcommands {
            "if not exist \"%{wks.location}bin\\PhysXFoundation_64.dll\" if exist \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/debug/bin/PhysXFoundation_64.dll\" copy /Y \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/debug/bin/PhysXFoundation_64.dll\" \"%{wks.location}bin\"",
            "if not exist \"%{wks.location}bin\\PhysXCommon_64.dll\" if exist \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/debug/bin/PhysXCommon_64.dll\" copy /Y \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/debug/bin/PhysXCommon_64.dll\" \"%{wks.location}bin\"",
            "if not exist \"%{wks.location}bin\\PhysX_64.dll\" if exist \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/debug/bin/PhysX_64.dll\" copy /Y \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/debug/bin/PhysX_64.dll\" \"%{wks.location}bin\""
        }
    filter { "system:windows", "configurations:Release" }
        libdirs { "Engine/third_party/vcpkg/packages/physx_x64-windows/lib" }
        links(os.matchfiles("Engine/third_party/vcpkg/packages/physx_x64-windows/lib/*.lib"))
        postbuildcommands {
            "if not exist \"%{wks.location}bin\\PhysXFoundation_64.dll\" if exist \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/bin/PhysXFoundation_64.dll\" copy /Y \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/bin/PhysXFoundation_64.dll\" \"%{wks.location}bin\"",
            "if not exist \"%{wks.location}bin\\PhysXCommon_64.dll\" if exist \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/bin/PhysXCommon_64.dll\" copy /Y \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/bin/PhysXCommon_64.dll\" \"%{wks.location}bin\"",
            "if not exist \"%{wks.location}bin\\PhysX_64.dll\" if exist \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/bin/PhysX_64.dll\" copy /Y \"%{wks.location}Engine/third_party/vcpkg/packages/physx_x64-windows/bin/PhysX_64.dll\" \"%{wks.location}bin\""
        }

    filter "*"

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
        
    dependson "Shaders"

project "Game"
    local sources = {
        "Game/**"
    }

    includedirs {
        ".",
        "../",
        "Engine/Utils"
    }

    kind "ConsoleApp"
    location "Game"

    filter "*"

    filter "system:linux"
        includedirs { "Engine/third_party/vcpkg/packages/physx_x64-linux/include/physx" }

    filter "system:windows"
        includedirs { "Engine/third_party/vcpkg/packages/physx_x64-windows/include/physx" }

    filter "*"

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
        "imgui",
        "x-tgen"
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