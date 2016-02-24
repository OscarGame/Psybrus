-- Write out revision file.
--os.execute( "cd assimp && git log -1 --format=%h > git.version" )
--os.execute( "cd assimp && git rev-parse --abbrev-ref HEAD > git.branch" )

--local inVersion = assert( io.open( "assimp/git.version", "r" ) )
local version = "00" -- ( inVersion:read() )
--inVersion:close()

--local inBranch = assert( io.open( "assimp/git.branch", "r" ) )
local branch = "local" -- ( inBranch:read() )
--inBranch:close()

local revisionFile = assert( io.open( "./assimp/revision.h", "w+" ) )
revisionFile:write( "#ifndef ASSIMP_REVISION_H_INC\n" )
revisionFile:write( "#define ASSIMP_REVISION_H_INC\n" )
revisionFile:write( string.format( "#define GitVersion 0x%s\n", version ) )
revisionFile:write( string.format( "#define GitBranch \"%s\"\n", branch ) )
revisionFile:write( "#endif // ASSIMP_REVISION_H_INC\n" )
revisionFile:close()

if PsyProjectExternalLib( "assimp", "C++" ) then
	kind ( EXTERNAL_PROJECT_KIND )
	configuration "windows-* or linux-* or osx-*"
		files { 
			"./assimp/include/**.h", 
			"./assimp/code/**.h",
			"./assimp/code/**.cpp",
		}

		includedirs { 
			"./assimp",
			"./assimp/include",
			"./assimp/code",
			"./assimp/code/BoostWorkaround",
			"./zlib"
		}

		defines { 
			"ASSIMP_BUILD_NO_OWN_ZLIB=1",
			"_SCL_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE",

			-- Remove importers that we don't want yet to
			-- keep the build lean.
			"ASSIMP_BUILD_NO_X_IMPORTER",
			"ASSIMP_BUILD_NO_MD3_IMPORTER",
			"ASSIMP_BUILD_NO_MDL_IMPORTER",
			"ASSIMP_BUILD_NO_MD2_IMPORTER",
			"ASSIMP_BUILD_NO_HMP_IMPORTER",
			"ASSIMP_BUILD_NO_MDC_IMPORTER",
			"ASSIMP_BUILD_NO_LWO_IMPORTER",
			"ASSIMP_BUILD_NO_NFF_IMPORTER",
			"ASSIMP_BUILD_NO_RAW_IMPORTER",
			"ASSIMP_BUILD_NO_OFF_IMPORTER",
			"ASSIMP_BUILD_NO_AC_IMPORTER",
			"ASSIMP_BUILD_NO_BVH_IMPORTER",
			"ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
			"ASSIMP_BUILD_NO_IRR_IMPORTER",
			"ASSIMP_BUILD_NO_Q3D_IMPORTER",
			"ASSIMP_BUILD_NO_B3D_IMPORTER",
			"ASSIMP_BUILD_NO_COLLADA_IMPORTER",
			"ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
			"ASSIMP_BUILD_NO_CSM_IMPORTER",
			"ASSIMP_BUILD_NO_3D_IMPORTER",
			"ASSIMP_BUILD_NO_LWS_IMPORTER",
			"ASSIMP_BUILD_NO_OGRE_IMPORTER",
			"ASSIMP_BUILD_NO_MS3D_IMPORTER",
			"ASSIMP_BUILD_NO_COB_IMPORTER",
			"ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
			"ASSIMP_BUILD_NO_NDO_IMPORTER",
			"ASSIMP_BUILD_NO_IFC_IMPORTER",
			"ASSIMP_BUILD_NO_XGL_IMPORTER"
		}

		excludes { 
			-- Remove ogre, has compile issues.
			"./assimp/code/Ogre**.*",
		}

	configuration "vs*"
		pchheader "AssimpPCH.h"
		pchsource "./assimp/code/AssimpPCH.cpp"

	configuration "windows-* or linux-* osx-*"
		links {
			-- External libs.
			"External_assimp_contrib",
		}
end


if PsyProjectExternalLib( "assimp_contrib", "C++" ) then
	kind ( EXTERNAL_PROJECT_KIND )
	configuration "windows-* or linux-* or osx-*"
		files { 
			"./assimp/contrib/clipper/**.cpp",
			"./assimp/contrib/clipper/**.hpp",
			"./assimp/contrib/ConvertUTF/**.c",
			"./assimp/contrib/ConvertUTF/**.h",
			"./assimp/contrib/poly2tri/**.cc",
			"./assimp/contrib/poly2tri/**.h"
		}

		includedirs { 
			"./zlib"
		}

		defines { 
			"ASSIMP_BUILD_NO_OWN_ZLIB=1",
			"_SCL_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE"
		}
end
