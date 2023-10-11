workspace "Pearl-Intern"  
    architecture "x64"
    configurations { "Debug", "Release" } 
    toolset "v142"
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
    pchsource "%{prj.name}/pch.cpp"

    files {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
        "%{prj.name}/**.hlsl",
    }

    includedirs {
        "%{prj.name}/",
        "3rd_party/",
        "%{prj.name}/3rd_party/imgui-docking/",
        "%{prj.name}/3rd_party/imgui-docking/backends"
    }
    
    libdirs {
        "3rd_party/DirectXTK-main/Bin/Desktop_2019/x64/Release"
    }

    links {
        "d3d11",
        "dxgi",
        "dxguid",
        "D3DCompiler",
        "DirectXTK",
    }

    optimize "Speed"

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
