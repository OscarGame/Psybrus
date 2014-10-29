PsyProjectExternalLib( EXTERNAL_PROJECT_PREFIX .. "HLSLCrossCompiler" )
	configuration "*"
		kind ( EXTERNAL_PROJECT_KIND )
		language "C"
		files { 
			"./HLSLCrossCompiler/include/**.h", 
			"./HLSLCrossCompiler/src/**.h", 
			"./HLSLCrossCompiler/src/**.c",
		}

		includedirs { 
			"./HLSLCrossCompiler/include", 
			"./HLSLCrossCompiler/src", 
			"./HLSLCrossCompiler/src/cbstring" ,
		}
