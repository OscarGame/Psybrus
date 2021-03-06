if PsyProjectExternalLib( "freetype", "C" ) then
	configuration "osx-*"
		kind ( EXTERNAL_PROJECT_KIND )
		includedirs { 
			"./freetype/devel", 
			"./freetype/include", 
			"./freetype/src/base" 
		}
		files {
			"./freetype/src/base/ftmac.c",
		}

	configuration "*"
		kind ( EXTERNAL_PROJECT_KIND )
		defines { "FT2_BUILD_LIBRARY" }
		files {
			"./freetype/src/autofit/autofit.c",
			"./freetype/src/base/basepic.c",
			"./freetype/src/base/ftadvanc.c",
			"./freetype/src/base/ftapi.c",
			"./freetype/src/base/ftbbox.c",
			"./freetype/src/base/ftadvanc.c",
			"./freetype/src/base/ftbdf.c",
			"./freetype/src/base/ftbitmap.c",
			"./freetype/src/base/ftcalc.c",
			"./freetype/src/base/ftcid.c",
			"./freetype/src/base/ftdbgmem.c",
			"./freetype/src/base/ftdebug.c",
			"./freetype/src/base/ftfstype.c",
			"./freetype/src/base/ftgasp.c",
			"./freetype/src/base/ftgloadr.c",
			"./freetype/src/base/ftglyph.c",
			"./freetype/src/base/ftgxval.c",
			"./freetype/src/base/ftinit.c",
			"./freetype/src/base/ftlcdfil.c",
			"./freetype/src/base/ftmm.c",
			"./freetype/src/base/ftobjs.c",
			"./freetype/src/base/ftotval.c",
			"./freetype/src/base/ftoutln.c",
			"./freetype/src/base/ftpatent.c",
			"./freetype/src/base/ftpfr.c",
			"./freetype/src/base/ftpic.c",
			"./freetype/src/base/ftrfork.c",
			"./freetype/src/base/ftsnames.c",
			"./freetype/src/base/ftstream.c",
			"./freetype/src/base/ftstroke.c",
			"./freetype/src/base/ftsynth.c",
			"./freetype/src/base/ftsystem.c",
			"./freetype/src/base/fttrigon.c",
			"./freetype/src/base/fttype1.c",
			"./freetype/src/base/ftutil.c",
			"./freetype/src/base/ftwinfnt.c",
			"./freetype/src/base/ftxf86.c",
			"./freetype/src/bdf/bdf.c",
			"./freetype/src/cache/ftcache.c",
			"./freetype/src/cff/cff.c",
			"./freetype/src/cid/type1cid.c",
			"./freetype/src/gxvalid/gxvalid.c",
			"./freetype/src/gzip/ftgzip.c",
			"./freetype/src/lzw/ftlzw.c",
			"./freetype/src/otvalid/otvalid.c",
			"./freetype/src/pcf/pcf.c",
			"./freetype/src/pfr/pfr.c",
			"./freetype/src/psaux/psaux.c",
			"./freetype/src/pshinter/pshinter.c",
			"./freetype/src/psnames/psnames.c",
			"./freetype/src/raster/raster.c",
			"./freetype/src/sfnt/sfnt.c",
			"./freetype/src/smooth/smooth.c",
			"./freetype/src/truetype/truetype.c",
			"./freetype/src/type1/type1.c",
			"./freetype/src/type42/type42.c",
			"./freetype/src/winfonts/winfnt.c"
		}
		includedirs { "./freetype/include" }

		links {
			EXTERNAL_PROJECT_PREFIX .. "zlib"
		}
end
