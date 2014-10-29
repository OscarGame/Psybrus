PsyProjectExternalLib( EXTERNAL_PROJECT_PREFIX .. "glew" )
	configuration "*"
		kind ( EXTERNAL_PROJECT_KIND ) 
		language "C++"
		files { "./glew/include/**.h", "./glew/src/glew.c" }
		includedirs { "./glew/include" }
		defines { "GLEW_BUILD=1" }
		if ( EXTERNAL_PROJECT_KIND == "StaticLib" ) then
			defines { "GLEW_STATIC=1" }
		end
		links {
			"opengl32"
		}
