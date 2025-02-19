-- Third party libraries
-- Adapted from COMP5892M (Advanced Rendering)

includedirs("yojimbo/")
includedirs("tinygltf/")
includedirs("glm/include")

defines("GLM_FORCE_RADIANS=1")
defines("GLM_FORCE_SIZE_T_LENGTH=1")
defines("GLM_ENABLE_EXPERIMENTAL=1")

project("Yojimbo")
    kind "StaticLib"

    location "."

    files {
        "yojimbo/include/**.h",
        "yojimbo/source/**.cpp",
        "yojimbo/netcode/**.h",
        "yojimbo/netcode/**.c",
        "yojimbo/reliable/**.h",
        "yojimbo/reliable/**.c",
        "yojimbo/sodium/**.h",
        "yojimbo/sodium/**.c",
        "yojimbo/tlsf/**.h",
        "yojimbo/tlsf/**.c"
    }

    includedirs {
        "yojimbo/include",
        "yojimbo/netcode",
        "yojimbo/reliable",
        "yojimbo/sodium",
        "yojimbo/tlsf"
    }

project("x-glm")
    kind "Utility"

    location "."

    files("glm/include/**.h*")
    files("glm/include/**.inl")

project("x-tinygltf")
    kind "StaticLib"

    location "."

    files("tinygltf/**.h*")

