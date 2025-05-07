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
        linkoptions { "/ignore:4099" }
        buildoptions { "/utf-8" }

    filter "*"

    -- default options for GLSLC
    glslcOptions = "-O --target-env=vulkan1.2"

    local cwd = os.getcwd()
    local shellScript = cwd .. "/Engine/third_party/vcpkg/vcpkg_setup.sh"
    local bootstrapScript = cwd .. "/Engine/third_party/vcpkg/bootstrap-vcpkg.sh"
    local batchFile = cwd .. "/Engine/third_party/vcpkg/vcpkg_setup.bat"

    local vcpkglibstr
    local vcpkgincludeDirs

    if os.istarget("linux") then
        os.execute('chmod 777 "' .. shellScript .. '"')
        os.execute('chmod 777 "' .. bootstrapScript .. '"')
        os.execute('sh "' .. shellScript .. '"')
        vcpkglibstr = "x64-linux"
        vcpkgincludeDirs = {
            "Engine/third_party/vcpkg/installed/x64-linux/include/physx",
            "Engine/third_party/vcpkg/installed/x64-linux/include/AL",
            "Engine/third_party/vcpkg/installed/x64-linux/include/ogg",
            "Engine/third_party/vcpkg/installed/x64-linux/include/vorbis",
            "Engine/third_party/vcpkg/installed/x64-linux/include/flac",
            "Engine/third_party/vcpkg/installed/x64-linux/include/opus",
            "Engine/third_party/vcpkg/installed/x64-linux/include/lame",
            "Engine/third_party/vcpkg/installed/x64-linux/include"
        }
    end
    if os.istarget("windows") then
        os.execute('"' .. batchFile .. '"')
        vcpkglibstr = "x64-windows"
        vcpkgincludeDirs = {
            "Engine/third_party/vcpkg/installed/x64-windows/include/physx",
            "Engine/third_party/vcpkg/installed/x64-windows/include/AL",
            "Engine/third_party/vcpkg/installed/x64-windows/include"
        }
    end

    if os.isdir("bin") == false then
        os.mkdir("bin")
    end

    filter "system:linux"
        links "dl"
        defines {"OS_LINUX", "DISABLE_CUDA_PHYSX"}

    filter "system:windows"
        defines {"OS_WINDOWS"}
    
    filter "*"

    filter "kind:StaticLib"
        targetdir "lib/"

    filter { "configurations:Debug", "kind:ConsoleApp" }
        targetdir "bin/debug/"
        targetextension ".exe"
    
    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"
        defines { "_DEBUG=1", "YOJIMBO_DEBUG", "NETCODE_DEBUG", "RELIABLE_DEBUG" }
        buildoptions { "/D_ITERATOR_DEBUG_LEVEL=2" }
    
    filter { "configurations:Release", "kind:ConsoleApp" }
        targetdir "bin/"
        targetextension ".exe"
    
    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines { "NDEBUG=1", "YOJIMBO_RELEASE", "NETCODE_RELEASE", "RELIABLE_RELEASE" }
        buildoptions { "/D_ITERATOR_DEBUG_LEVEL=0" }
    
    filter "*"

include "Engine/third_party"

-- GLSLC helpers
dofile("Engine/Utils/glslc.lua")

-- Projects
project "Engine"
    local sources = {
        "Engine/Core/**",
	    "Engine/Audio/**",
	    "Engine/ThreadPool/**",
        "Engine/Input/**", 
        "Engine/Events/**", 
        "Engine/gltf/**",
        "Engine/vulkan/**",
        "Engine/Shaders/**",
        "Engine/Network/**",
        "Engine/ECS/**",
        "Engine/GUI/**",
        "Engine/Physics/**",
        "Engine/Rendering/**"
    }

    includedirs {
        "Engine/Utils/",
        vcpkgincludeDirs
    }

    kind "StaticLib"
    location "Engine"

    filter "*"

    filter "configurations:Debug"
        libdirs { "Engine/third_party/vcpkg/installed/" .. vcpkglibstr .. "/debug/lib" }

    filter "configurations:Release"
        libdirs { "Engine/third_party/vcpkg/installed/" .. vcpkglibstr .. "/lib" }
    
    filter "*"
    
    files(sources)
    removefiles("**.vcxproj*")

    filter "*"
    
    filter { "system:linux" }
        links {
            "PhysXCharacterKinematic_static_64",
            "PhysXExtensions_static_64",
            "PhysX_static_64",
            "PhysXPvdSDK_static_64",
            "PhysXVehicle_static_64",
            "PhysXCooking_static_64",
            "PhysXCommon_static_64",
            "PhysXFoundation_static_64",
            "openal",
            "sndfile",
            "vorbis",
            "vorbisenc",
            "ogg",
            "FLAC",
            "opus",
            "mpg123",
            "mp3lame"
        }

    filter "*"

    filter { "system:windows" }
        links {
            "PhysX_64",
            "OpenAL32",
            "sndfile"
        }
        postbuildcommands {
            "call \"%{wks.location}windows_copy_dlls.bat\" \"%{wks.location}\" \"%{cfg.buildcfg}\""        
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
        "Engine/Utils",
        "Engine",
        vcpkgincludeDirs
    }

    kind "ConsoleApp"
    location "Game"

    filter "*"

    filter "configurations:Debug"
        libdirs { "Engine/third_party/vcpkg/installed/" .. vcpkglibstr .. "/debug/lib" }

    filter "configurations:Release"
        libdirs { "Engine/third_party/vcpkg/installed/" .. vcpkglibstr .. "/lib" }
    
    filter "*"

    files(sources)
    removefiles("**.vcxproj*")

    filter "*"
        
    filter { "system:linux" }
        links {
            "PhysXCharacterKinematic_static_64",
            "PhysXExtensions_static_64",
            "PhysX_static_64",
            "PhysXPvdSDK_static_64",
            "PhysXVehicle_static_64",
            "PhysXCooking_static_64",
            "PhysXCommon_static_64",
            "PhysXFoundation_static_64",
            "openal",
            "sndfile",
            "vorbis",
            "vorbisenc",
            "ogg",
            "FLAC",
            "opus",
            "mpg123",
            "mp3lame"
        }

    filter "*"

    filter { "system:windows" }
        links {
            "PhysXCharacterKinematic_static_64",
            "PhysXExtensions_static_64",
            "PhysX_64",
            "PhysXPvdSDK_static_64",
            "PhysXVehicle_static_64",
            "PhysXCooking_64",
            "PhysXCommon_64",
            "PhysXFoundation_64",
            "OpenAL32",
            "sndfile"
        }

    filter "*"
    
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