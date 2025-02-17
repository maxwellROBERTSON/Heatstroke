-- Third party libraries
-- Adapted from COMP5892M (Advanced Rendering)

includedirs("volk/include")
includedirs("vulkan/include")
includedirs("glfw/include")
includedirs("VulkanMemoryAllocator/include")
includedirs("glm/include")
includedirs("tgen/include")
includedirs("tinygltf/")
includedirs( "imgui");
includedirs( "imgui/backends");

defines("GLM_FORCE_RADIANS=1")
defines("GLM_FORCE_SIZE_T_LENGTH=1")
defines("GLM_ENABLE_EXPERIMENTAL=1")

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

project ( "x-imgui")
	kind "StaticLib"
	location "."

	files {
		"imgui/*.cpp",
		"imgui/*.h",
		"imgui/backends/imgui_impl_vulkan.h",
		"imgui/backends/imgui_impl_vulkan.cpp",
		"imgui/backends/imgui_impl_glfw.h",
		"imgui/backends/imgui_impl_glfw.cpp",
		"imgui/misc/debuggers/imgui.natvis",
		"imgui/misc/debuggers/imgui.nastepfilter",
		"imgui/misc/cpp/imgui_stdlib.cpp",
		"imgui/misc/cpp/imgui_stdlib.h"
	}

	includedirs( "imgui");
	includedirs( "imgui/backends");

project()