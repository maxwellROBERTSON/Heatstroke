-- Third party libraries
-- Adapted from COMP5892M (Advanced Rendering)

includedirs("yojimbo")

print("Including third_party")

project "Yojimbo"
    kind "StaticLib"

    location "."

    -- Include all necessary files
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

project()