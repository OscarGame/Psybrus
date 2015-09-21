dofile( "NDK/ndk.lua" )
dofile( "NDK/common.lua" )
dofile( "NDK/makefile.lua" )
dofile( "NDK/manifest.lua" )

ANDROID_TOOL="$(ANDROID_SDK)/tools/android"	
if IsHostOS("windows") then
	ANDROID_TOOL="\"$(ANDROID_SDK)/tools/android\""
end


ANDROID_NDK_PATH = os.getenv("ANDROID_NDK")

function mkdirCommand( path )
	mkdirCommandString = "mkdir -p " .. path
	if IsHostOS("windows") then
		mkdirCommandString = "mkdir " .. path
		mkdirCommandString = string.gsub( mkdirCommandString, "/", "\\" )
		print( mkdirCommandString )
	end
	print( "mkdirCommand: " .. mkdirCommandString )
	return mkdirCommandString
end

function copyCommand( src, dest )
	copyCommandString = "cp " .. src .. " " .. dest 
	if IsHostOS("windows") then
		copyCommandString = "copy " .. src .. " " .. dest 
		copyCommandString = string.gsub( copyCommandString, "/", "\\" )
	end
	print( "copyComamndString: " .. copyCommandString )
	return copyCommandString
end

function SetupAndroidProject()
	local suffix = _OPTIONS["toolchain"]

	if _OPTIONS["toolchain"] == "android-gcc-arm" or 
	   _OPTIONS["toolchain"] == "android-gcc-x86" then
		kind "SharedLib"
		flags { "NoImportLib" }
		--kind "ConsoleApp"

		-- TODO: Other abis.
		local gdbserver = ANDROID_NDK_PATH .. "/prebuilt/android-arm/gdbserver/gdbserver"

		if _OPTIONS["toolchain"] == "android-gcc-arm" then
			abi = "armeabi-v7a"
		elseif _OPTIONS["toolchain"] == "android-gcc-x86" then
			abi = "x86"
		end

		local libName = solution().name
		buildPath = "../Build/" .. _ACTION .. "-" .. suffix

		projectPath = buildPath .. "/project"

		postBuildPath = "../" .. buildPath
		postBuildProjectPath = "../" .. projectPath

		-- Create AndroidManifest.xml
		manifestFile = assert( io.open( projectPath .. "/AndroidManifest.xml", "w+" ) )
		manifestFile:write( "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" )
		manifestFile:write( "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n" )
		manifestFile:write( "          package=\"" .. GAME.android.package .. "\"\n" )
		manifestFile:write( "          android:versionCode=\"" .. GAME.android.version_code .. "\"\n" )
		manifestFile:write( "          android:versionName=\"" .. GAME.android.version_name .. "\">\n" )
		manifestFile:write( "  <uses-sdk android:minSdkVersion=\"" .. GAME.android.sdk_version .. "\" />\n" )

		for i, permission in ipairs(GAME.android.permissions) do
			manifestFile:write( "  <uses-permission android:name=\"" .. permission .. "\" />\n" )
		end

		manifestFile:write( "  <uses-feature android:glEsVersion=\"" .. GAME.android.es_version .. "\" />\n" )
		manifestFile:write( "  <application android:label=\"@string/app_name\"\n" )
		manifestFile:write( "               android:hasCode=\"false\" android:debuggable=\"true\">\n" )
		manifestFile:write( "    <activity android:name=\"android.app.NativeActivity\"\n" )
		manifestFile:write( "              android:screenOrientation=\"" .. GAME.android.orientation .. "\"\n" )
		manifestFile:write( "              android:label=\"@string/app_name\">\n" )
		manifestFile:write( "      <meta-data android:name=\"android.app.lib_name\"\n" )
		manifestFile:write( "                 android:value=\"" .. libName .. "\" />\n" )
		manifestFile:write( "      <intent-filter>\n" )
		manifestFile:write( "        <action android:name=\"android.intent.action.MAIN\" />\n" )
		manifestFile:write( "        <category android:name=\"android.intent.category.LAUNCHER\" />\n" )
		manifestFile:write( "      </intent-filter>\n" )
		manifestFile:write( "    </activity>\n" )
		manifestFile:write( "  </application>\n" )
		manifestFile:write( "</manifest>\n" )
		manifestFile:close()

		-- Create strings xml.
		stringsFile = assert( io.open( projectPath .. "/res/values/strings.xml", "w+" ) )
		stringsFile:write( "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" )
		stringsFile:write( "<resources>\n" )
		stringsFile:write( "  <string name=\"app_name\">" .. libName .. "</string>\n" )
		stringsFile:write( "</resources>\n" )
		stringsFile:close()

		-- Create gdb config.
		gdbFile = assert( io.open( projectPath .. "/libs/" .. abi .. "/gdb.setup", "w+" ) )
		gdbFile:write( "set solib-search-path ../obj\n" )
		gdbFile:write( "source " .. ANDROID_NDK_PATH .. "/prebuilt/common/gdb/common.setup\n" )
		gdbFile:write( "directory ../../../Source ../../../Psybrus/Engine/Source\n" )

		-- Setup post build setp.
		configuration { "android-*" }
			PsyAddExternalLinks( "llvmcxxabi" )

			links {
				"GLESv1_CM",
				"GLESv2",
				"EGL",
				"OpenSLES",
				"llvmcxxabi"
			}

			androidTarget = "android-" .. GAME.android.sdk_version

		libPrefixName = "lib" .. libName
		libExt = ".so"

	end

end

function LLVMcxxabiProject()
	if _OPTIONS["toolchain"] == "android-gcc-arm" or 
	   _OPTIONS["toolchain"] == "android-gcc-x86" then
		project( "llvmcxxabi" )
			language( "C++" )
			configuration "*"
				kind ( EXTERNAL_PROJECT_KIND )
				buildoptions( "-std=c++11" )
				files { 
					"../Psybrus/External/libcxxabi/src/*",
					"../Psybrus/External/libcxxabi/include/*"
				}
				includedirs {
					"../Psybrus/External/libcxxabi/include"
				}
	end
end
