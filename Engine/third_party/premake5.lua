-- Third party libraries
-- Adapted from COMP5892M (Advanced Rendering)

includedirs("yojimbo/.")
includedirs("yojimbo/include")
includedirs("yojimbo/sodium")
includedirs("yojimbo/tlsf")
includedirs("yojimbo/netcode")
includedirs("yojimbo/reliable")
includedirs("yojimbo/serialize")
includedirs("volk/include")
includedirs("vulkan/include")
includedirs("glfw/include")
includedirs("VulkanMemoryAllocator/include")
includedirs("glm/include")
includedirs("tgen/include")
includedirs("tinygltf/")
includedirs("imgui/")

defines("GLM_FORCE_RADIANS=1")
defines("GLM_FORCE_SIZE_T_LENGTH=1")
defines("GLM_ENABLE_EXPERIMENTAL=1")

filter "system:not windows"
    includedirs( "libsodium-1.0.20/libsodium-build/include")
    libdirs( "libsodium-1.0.20/libsodium-build/lib")
    links { "sodium" }

project "yojimbo"
    kind "StaticLib"
    location "."

    files {
        "yojimbo/include/*.h",
        "yojimbo/source/*.cpp"
    }

project "sodium-builtin"
    kind "StaticLib"
    location "."
    language "C"

    files {
        "yojimbo/sodium/**.c",
        "yojimbo/sodium/**.h"
    }

    filter "system:not windows"
        files { "yojimbo/sodium.s/**.S" }  -- Include assembly files on Linux/macOS
        buildoptions { "-Wno-unused-parameter", "-Wno-unused-function", "-Wno-unknown-pragmas", "-Wno-unused-variable", "-Wno-type-limits" }

    filter "action:gmake*"
        buildoptions { "-Wno-unused-parameter", "-Wno-unused-function", "-Wno-unknown-pragmas", "-Wno-unused-variable", "-Wno-type-limits" }

project "netcode"
    kind "StaticLib"
    location "."
    language "C"

    defines { "NETCODE_ENABLE_TESTS=1" }
    files { "yojimbo/netcode/netcode.c", "yojimbo/netcode/netcode.h" }

project "reliable"
    kind "StaticLib"
    location "."
    language "C"

    defines { "RELIABLE_ENABLE_TESTS=1" }
    files { "yojimbo/reliable/reliable.c", "yojimbo/reliable/reliable.h" }

project "tlsf"
    kind "StaticLib"
    location "."
    language "C"

    files { "yojimbo/tlsf/tlsf.c", "yojimbo/tlsf/tlsf.h" }

project("x-volk")
    kind "StaticLib"
    location "."

    files("volk/src/*.c")
    files("volk/include/volk/*.h")

project("x-vulkan-headers")
    kind "Utility"
    location "."

    files("vulkan/include/**.h*")

project("x-glfw")
    kind "StaticLib"
    location "."

    filter "system:linux"
        defines { "_GLFW_X11=1" }

    filter "system:windows"
        defines { "_GLFW_WIN32=1" }

    filter "*"

    files {
        "glfw/src/context.c",
		"glfw/src/egl_context.c",
		"glfw/src/init.c",
		"glfw/src/input.c",
		"glfw/src/internal.h",
		"glfw/src/mappings.h",
		"glfw/src/monitor.c",
		"glfw/src/null_init.c",
		"glfw/src/null_joystick.c",
		"glfw/src/null_joystick.h",
		"glfw/src/null_monitor.c",
		"glfw/src/null_platform.h",
		"glfw/src/null_window.c",
		"glfw/src/platform.c",
		"glfw/src/platform.h",
		"glfw/src/vulkan.c",
		"glfw/src/window.c",
		"glfw/src/osmesa_context.c"
    };

    filter "system:linux"
        files {
            "glfw/src/posix_*",
			"glfw/src/x11_*", 
			"glfw/src/xkb_*",
			"glfw/src/glx_*",
			"glfw/src/linux_*"
        };

    filter "system:windows"
        files {
            "glfw/src/win32_*",
            "glfw/src/wgl_*"
        };

    filter "*"

project("x-vma")
    kind "StaticLib"
    location "."

    filter "toolset:gcc or toolset:clang"
        buildoptions {
            "-Wno-unused-variable",
            "-Wno-unused-function"
        }
    filter "toolset:clang"
        buildoptions {
            "-Wno-nullability-completeness"
        }
    filter {}

    files("VulkanMemoryAllocator/src/*.cpp")

project("x-glm")
    kind "Utility"
    location "."

    files("glm/include/**.h*")
    files("glm/include/**.inl")

project("x-tgen")
    kind "StaticLib"
    location "."

    files("tgen/src/*.cpp")

project("x-tinygltf")
    kind "StaticLib"
    location "."

    files("tinygltf/**.h*")

project("imgui")
    kind "StaticLib"
    location "."

    files("imgui/**.h**")
    files("imgui/**.cpp")
    files("imgui/backend/**.h**")
    files("imgui/backend/**.cpp")
    files("imgui/misc/**/**.h**")
    files("imgui/misc/**/**.cpp")


-- project("stb")
--     kind "StaticLib"
--     location "."

--     files("stb/src/*.c")

project()
