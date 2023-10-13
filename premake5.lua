workspace "Grid"  
    architecture "x64"
    configurations { "Debug", "Release" } 
    toolset "v142"
    startproject "Grid"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}

project "Grid"
    location "Grid"
    kind "ConsoleApp"
    language "C++"   
    cppdialect "C++20" 
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/pch.cpp"

    files {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
        "%{prj.name}/**.hlsl",
    }

    includedirs {
        "%{prj.name}/",
        "3rd_party/",
        "3rd_party/ImGui/",
        "3rd_party/ImGui/backends/"
    }

    links {
        "d3d11",
        "dxgi",
        "dxguid",
        "D3DCompiler",
        "DirectXTK",
        "ImGui",
    }

    floatingpoint "Fast"

    characterset "MBCS"

    filter "files:**.hlsl"
        shaderentry "main"
        shadermodel "5.0"
        shaderobjectfileoutput "%{wks.location}/%{prj.name}/%{file.basename}.cso"

    filter "files:**PS.hlsl"
        shadertype "Pixel"

    filter "files:**VS.hlsl"
        shadertype "Vertex"

    filter "files:**GS.hlsl"
        shadertype "Geometry"

    filter "files:**CS.hlsl"
        shadertype "Compute"

    vpaths {
        ["HLSL"] = "**.hlsl",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }  
        runtime "Debug"
        symbols "on" 
        libdirs {
            "3rd_party/DirectXTK-main/Bin/Desktop_2019/x64/Debug",
            "bin/Debug-windows-x86_64/ImGui",
        }
        buildoptions "/MDd"
        optimize "Debug"

    filter "configurations:Release"  
        defines { "RELEASE" }    
        runtime "Release"
        libdirs {
            "3rd_party/DirectXTK-main/Bin/Desktop_2019/x64/Release",
            "bin/Release-windows-x86_64/ImGui",
        }
        buildoptions "/MD"
        optimize "Speed"



project "ImGui"
    location "3rd_party/ImGui"
    kind "StaticLib"
    language "C++"   
    cppdialect "C++17" 
    staticruntime "Off"


    includedirs {
        "3rd_party/%{prj.name}/",
        "3rd_party/libfreetype2/freetype2/include",
    }

    libdirs {
        "3rd_party/libfreetype2/freetype2/objs/vc2010/Win32"
    }

    links {
        "freetype265d.lib"
    }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "3rd_party/%{prj.name}/**.h",
        "3rd_party/%{prj.name}/**.cpp",
    }

    filter "configurations:Debug"
        runtime "Debug"
        buildoptions "/MDd"
        optimize "Debug"

    filter "configurations:Release"     
        runtime "Release"
        buildoptions "/MD"
        optimize "Speed"