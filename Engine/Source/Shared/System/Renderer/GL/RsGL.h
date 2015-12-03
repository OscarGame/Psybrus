#pragma once

#include "Base/BcTypes.h"
#include "Base/BcDebug.h"
#include "System/Renderer/RsFeatures.h"

#include <tuple>

////////////////////////////////////////////////////////////////////////////////
// HTML5
#if PLATFORM_HTML5
#  include "GL/glew.h"

#  define GL_ETC1_RGB8_OES 0x8D64

#  define RENDER_USE_GLES

////////////////////////////////////////////////////////////////////////////////
// Android
#elif PLATFORM_ANDROID
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#  include "GLES3/gl3ext.h"
#  include "GLES3/gl31.h"

#  include <EGL/egl.h>

#define GL_SAMPLER_1D 0x8B5D
#define GL_SAMPLER_1D_SHADOW 0x8B61

#define GL_IMAGE_1D 0x904C
#define GL_IMAGE_2D 0x904D
#define GL_IMAGE_3D 0x904E
#define GL_IMAGE_2D_RECT 0x904F
#define GL_IMAGE_CUBE 0x9050
#define GL_IMAGE_BUFFER 0x9051
#define GL_IMAGE_1D_ARRAY 0x9052
#define GL_IMAGE_2D_ARRAY 0x9053
#define GL_IMAGE_CUBE_MAP_ARRAY 0x9054
#define GL_IMAGE_2D_MULTISAMPLE 0x9055
#define GL_IMAGE_2D_MULTISAMPLE_ARRAY 0x9056

#  define RENDER_USE_GLES

#if ANDROID_NDK_VERSION >= 20
#  define RENDER_USE_GLES3
#endif

////////////////////////////////////////////////////////////////////////////////
// Linux
#elif PLATFORM_LINUX
#  include "GL/glew.h"

////////////////////////////////////////////////////////////////////////////////
// Mac
#elif PLATFORM_OSX
#  include "GL/glew.h"

////////////////////////////////////////////////////////////////////////////////
// Win32
#elif PLATFORM_WINDOWS
#  define WIN32_LEAN_AND_MEAN
#  define NOGDICAPMASKS    
#  define NOMENUS         
#  define NORASTEROPS     
#  define OEMRESOURCE     
#  define NOATOM          
#  define NOCLIPBOARD     
#  define NODRAWTEXT      
#  define NONLS           
#  define NOMEMMGR        
#  define NOMETAFILE      
#  define NOMINMAX        
#  define NOOPENFILE      
#  define NOSCROLL        
#  define NOSERVICE       
#  define NOSOUND         
#  define NOTEXTMETRIC    
#  define NOWINOFFSETS    
#  define NOKANJI         
#  define NOHELP          
#  define NOPROFILER      
#  define NODEFERWINDOWPOS
#  define NOMCX
#  include "GL/glew.h"
#  include "GL/wglew.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// GLES defines.
#if !defined( RENDER_USE_GLES )
#define GL_ETC1_RGB8_OES           0x8D64
#endif

////////////////////////////////////////////////////////////////////////////////
// RsGLCatchError
#define PSY_GL_CATCH_ERRORS ( 1 && !PSY_PRODUCTION && !PSY_USE_PROFILER && !PLATFORM_HTML5 )

GLuint RsReportGLErrors( const char* File, int Line, const char* CallString );

#if PSY_GL_CATCH_ERRORS
#  define GL( _call ) \
	gl##_call; RsReportGLErrors( __FILE__, __LINE__, #_call  )

#else
#  define GL( _call ) \
	gl##_call
#endif

////////////////////////////////////////////////////////////////////////////////
// RsOpenGLType
enum class RsOpenGLType
{
	CORE = 0,
	COMPATIBILITY,
	ES
};

////////////////////////////////////////////////////////////////////////////////
// RsOpenGLVersion
struct RsOpenGLVersion
{
	RsOpenGLVersion();
	RsOpenGLVersion( BcS32 Major, BcS32 Minor, RsOpenGLType Type, RsShaderCodeType MaxCodeType );

	/**
	 * Will setup feature support + query extensions for active context and setup all the features supported.
	 */
	void setupFeatureSupport();

	/**
	 * Is shader code type supported?
	 */
	BcBool isShaderCodeTypeSupported( RsShaderCodeType CodeType ) const;

	bool operator < ( const RsOpenGLVersion& Other ) const 
	{
		return std::make_tuple( Major_, Minor_, Type_, MaxCodeType_ ) < std::make_tuple( Other.Major_, Minor_, Type_, MaxCodeType_ );
	}

	// Overall information.
	BcS32 Major_;
	BcS32 Minor_;
	RsOpenGLType Type_;
	RsShaderCodeType MaxCodeType_;

	// Features.
	RsFeatures Features_;

	// GL specific features.
	bool SupportPolygonMode_;
	bool SupportVAOs_;
	bool SupportSamplerStates_;
	bool SupportUniformBuffers_;
	bool SupportImageLoadStore_;
	bool SupportShaderStorageBufferObjects_;
	bool SupportProgramInterfaceQuery_;
	bool SupportGeometryShaders_;
	bool SupportTesselationShaders_;
	bool SupportComputeShaders_;
	bool SupportDrawElementsBaseVertex_;
	bool SupportBlitFrameBuffer_;
	bool SupportCopyImageSubData_;
	GLint MaxTextureSlots_;

};

