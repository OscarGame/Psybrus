PsyProjectEngineLib( "System_Network" )
  configuration "*"
  	files {
      "./Shared/System/Network/**.h", 
      "./Shared/System/Network/**.inl", 
      "./Shared/System/Network/**.cpp", 
      "./Platforms/Windows/System/Network/**.h", 
      "./Platforms/Windows/System/Network/**.inl", 
      "./Platforms/Windows/System/Network/**.cpp", 
    }
  	includedirs {
      "./Shared/",
      "./Platforms/Windows/",
      "../../External/jsoncpp/include/",
      "../../External/libb64/include/",
      "../../External/RakNet/Source",
      "../../External/webby/"
    }

		PsyAddEngineLinks {
      "System",
    }

    PsyAddExternalLinks {
      "webby",
		}
