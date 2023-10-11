workspace "Pearl-Intern"  
    architecture "x64"
    configurations { "Debug", "Release" } 
    startproject "Pearl-Intern"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}

project "Pearl-Intern"
    location "Pearl-Intern"
    kind "ConsoleApp"
    language "C++"   
    cppdialect "C++20" 
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "pch.cpp"

    files {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
        "%{prj.name}/**.hlsl",
    }
    includedirs {
        "%{prj.name}/src/",
    }

    links {
        "d3d11",
        "dxgi",
        "dxguid",
        "D3DCompiler",
    }

    optimize "Speed"

    floatingpoint "Fast"

    characterset "MBCS"

    filter "files:**.hlsl"
        shaderentry "main"
        shadermodel "5.0"
        shaderobjectfileoutput "%{wks.location}/%{file.basename}.cso"

    filter "files:**PS.hlsl"
        shadertype "Pixel"

    filter "files:**VS.hlsl"
        shadertype "Vertex"