wwisesdkpath = os.getenv("WWISESDK")
wwiseversion = "Wwise v2012.1.4 build 4260"

if (wwisesdkpath == nil or string.find(wwisesdkpath, wwiseversion) == nil) then
	print ("Please install " .. wwiseversion)
	os.exit()
else
	print ("Got Wwise SDK Path: " .. wwisesdkpath)
end

wwisedebuglib = wwisesdkpath .. "/Win32_vc100/Debug(StaticCRT)/lib"
wwiseprofilelib = wwisesdkpath .. "/Win32_vc100/Profile(StaticCRT)/lib"
wwisereleaselib = wwisesdkpath .. "/Win32_vc100/Release(StaticCRT)/lib"

wwisedebuglibs = {
	"AkAudioInputSource",
	"AkCompressorFX",
	"AkConvolutionReverbFX",
	"AkDelayFX",
	"AkExpanderFX",
	"AkFlangerFX",
	"AkGainFX",
	"AkGuitarDistortionFX",
	"AkHarmonizerFX",
	"AkMatrixReverbFX",
	"AkMemoryMgr",
	"AkMeterFX",
	"AkMotionGenerator",
	"AkMP3Source",
	"AkMusicEngine",
	"AkParametricEQFX",
	"AkPeakLimiterFX",
	"AkPitchShifterFX",
	"AkRoomVerbFX",
	"AkRumble",
	"AkSilenceSource",
	"AkSineSource",
	"AkSoundEngine",
	"AkSoundSeedImpactFX",
	"AkSoundSeedWind",
	"AkSoundSeedWoosh",
	"AkStereoDelayFX",
	"AkStreamMgr",
	"AkTimeStretchFX",
	"AkToneSource",
	"AkTremoloFX",
	"AkVorbisDecoder",
	"CommunicationCentral",
	"iZHybridReverbFX",
	"iZTrashBoxModelerFX",
	"iZTrashDelayFX",
	"iZTrashDistortionFX",
	"iZTrashDynamicsFX",
	"iZTrashFiltersFX",
	"iZTrashMultibandDistortionFX",
	"McDSPFutzBoxFX",
	"McDSPLimiterFX",
	"SFlib"
}

wwiseprofilelibs = wwisedebuglibs

wwisereleaselibs = {
	"AkAudioInputSource",
	"AkCompressorFX",
	"AkConvolutionReverbFX",
	"AkDelayFX",
	"AkExpanderFX",
	"AkFlangerFX",
	"AkGainFX",
	"AkGuitarDistortionFX",
	"AkHarmonizerFX",
	"AkMatrixReverbFX",
	"AkMemoryMgr",
	"AkMeterFX",
	"AkMotionGenerator",
	"AkMP3Source",
	"AkMusicEngine",
	"AkParametricEQFX",
	"AkPeakLimiterFX",
	"AkPitchShifterFX",
	"AkRoomVerbFX",
	"AkRumble",
	"AkSilenceSource",
	"AkSineSource",
	"AkSoundEngine",
	"AkSoundSeedImpactFX",
	"AkSoundSeedWind",
	"AkSoundSeedWoosh",
	"AkStereoDelayFX",
	"AkStreamMgr",
	"AkTimeStretchFX",
	"AkToneSource",
	"AkTremoloFX",
	"AkVorbisDecoder",
	"iZHybridReverbFX",
	"iZTrashBoxModelerFX",
	"iZTrashDelayFX",
	"iZTrashDistortionFX",
	"iZTrashDynamicsFX",
	"iZTrashFiltersFX",
	"iZTrashMultibandDistortionFX",
	"McDSPFutzBoxFX",
	"McDSPLimiterFX",
	"SFlib"
}


project "Engine_IntegrationWwise"
	kind "StaticLib"
	language "C++"
	files { "./**.h", "./**.c", "./**.cpp" }
	includedirs { "./", "../Shared/" }
	includedirs { "../../../External/jsoncpp/include" }
	includedirs { wwisesdkpath .. "/include" }

	configuration "windows"
		includedirs { "../Platforms/Windows" }

	configuration "Debug"
		libdirs { wwisedebuglib }
	   	links (wwisedebuglibs)

	configuration "Release"
		libdirs { wwiseprofilelib }
	   	links (wwiseprofilelibs)

	configuration "Production"
		libdirs { wwisereleaselib }
	   	links (wwisereleaselibs)
