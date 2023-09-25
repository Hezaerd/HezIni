workspace "HezIni"
    architecture "x86_64"
    startproject "HezIni-Demo"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
    include "HezIni"
group ""

group "Demo"
    include "HezIni-Demo"
group ""