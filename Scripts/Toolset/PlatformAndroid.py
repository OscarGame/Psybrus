from Platform import *
from BuildGmake import *

androidPlaform = Platform( 
	"android-gcc-arm", "Android ARM (GCC 4.9)", "android",
	"x32", "linux", "gmake", BuildGmake,
	[] )

PLATFORMS.append( androidPlaform )

