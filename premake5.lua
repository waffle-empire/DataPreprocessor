workspace "DataPreprocessor"
	architecture "x64"
	startproject "DataPreprocessor"

	configurations
	{
		"Debug",
		"Release"
	}

	outputdir = "%{cfg.buildcfg}"

	IncludeDir = {}
    IncludeDir["csv_parser"] = "vendor/csv-parser"
	IncludeDir["json"] = "vendor/json/single_include"
	IncludeDir["ta_lib"] = "/usr/include/ta-lib"

	CppVersion = "C++17"

	project "DataPreprocessor"
		location "src/"
		kind "ConsoleApp"
		language "C++"

		targetdir ("bin/" .. outputdir)
		objdir ("bin/int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"src/**.hpp",
			"src/**.h",
			"src/**.cpp",
			"src/**.asm"
		}

		includedirs
		{
            "%{IncludeDir.csv_parser}",
			"%{IncludeDir.json}",
			"src/"
		}

		libdirs
		{
			"bin/lib"
		}

		links
		{
			"pthread",
			"ta_lib"
		}

		flags { "NoImportLib", "Maps" }

		filter "configurations:Debug"
			flags { "LinkTimeOptimization", "MultiProcessorCompile" }
			editandcontinue "Off"
			defines { "DEBUG" }

		filter "configurations:Release"
			flags { "LinkTimeOptimization", "NoManifest", "MultiProcessorCompile" }
			defines { "RELEASE" }
			optimize "speed"
