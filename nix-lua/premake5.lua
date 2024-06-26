---@diagnostic disable: undefined-global

function GetFromRoot(path)
    return string.format("%s/%s", _MAIN_SCRIPT_DIR, path)
end

BIN_DIR = GetFromRoot("bin/%{cfg.buildcfg}/%{cfg.platform}")
OBJ_DIR = GetFromRoot("obj/%{cfg.buildcfg}/%{cfg.platform}/%{prj.name}")

workspace("Nix.Lua")
    configurations({ "Debug", "Release" })
    platforms({ "Linux", "Windows", "Mac", "MacARM", "Mac64" })
    architecture("x86_64")

    filter({ "platforms:Linux" })
        system("linux")
        defines({"_UNIX"})

    filter({ "platforms:Windows" })
        system("windows")
        defines({"_WIN"})

    filter({ "platforms:Mac" })
        system("macosx")
        architecture("universal")
        defines({"_MAC"})

    filter({ "platforms:MacARM" })
        system("macosx")
        architecture("ARM64")
        defines({"_MAC", "_ARM64"})

    filter({ "platforms:Mac64" })
        system("macosx")
        defines({"_MAC", "_MAC_64"})

    project("Nix.Lua")
        kind("SharedLib")
        language("C++")
        cppdialect("C++20")
        targetdir(BIN_DIR)
        objdir(OBJ_DIR)

        
        files({
            "src/**.h", "src/**.hpp", "src/**.hh",
            "src/**.cpp", "src/**.cc"
        })
        includedirs({ "src" })

        libdirs({ "lib" })
        externalincludedirs({
            "external/lua/src",
            "external",
            "external/nix", -- Fix for issue with Nix headers
        })
        links({ "lua:shared" })


        filter("configurations:Debug")
            defines({"DEBUG"})
            symbols("On")

        filter("configurations:Release")
            defines({"NDEBUG"})
            optimize("On")