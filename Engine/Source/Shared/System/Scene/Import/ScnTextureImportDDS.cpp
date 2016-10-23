#include "System/Scene/Import/ScnTextureImport.h"
#include "System/Scene/Rendering/ScnTextureFileData.h"

#if PSY_IMPORT_PIPELINE

#include "Base/BcStream.h"

namespace DDS
{
	#define MAKEFOURCC( A, B, C, D ) ( ( BcU32( D ) << 24 ) | ( BcU32( C ) << 16 ) | ( BcU32( B ) << 8 ) | ( BcU32( A ) ) )

	struct DDS_PIXELFORMAT
	{
		BcU32 dwSize;
		BcU32 dwFlags;
		BcU32 dwFourCC;
		BcU32 dwRGBBitCount;
		BcU32 dwRBitMask;
		BcU32 dwGBitMask;
		BcU32 dwBBitMask;
		BcU32 dwABitMask;
	};

	struct DDS_HEADER
	{
		BcU32 dwSize;
		BcU32 dwFlags;
		BcU32 dwHeight;
		BcU32 dwWidth;
		BcU32 dwPitchOrLinearSize;
		BcU32 dwDepth;
		BcU32 dwMipMapCount;
		BcU32 dwReserved1[11];
		DDS_PIXELFORMAT ddspf;
		BcU32 dwCaps;
		BcU32 dwCaps2;
		BcU32 dwCaps3;
		BcU32 dwCaps4;
		BcU32 dwReserved2;
	};

	enum DDSD_FLAGS
	{
		DDSD_CAPS = 0x1,
		DDSD_HEIGHT = 0x2,
		DDSD_WIDTH = 0x4,
		DDSD_PITCH = 0x8,
		DDSD_PIXELFORMAT = 0x1000,
		DDSD_MIPMAPCOUNT = 0x20000,
		DDSD_LINEARSIZE = 0x80000,
		DDSD_DEPTH = 0x800000,
	};

	enum DDPF_FLAGS
	{
		DDPF_ALPHAPIXELS = 0x1,
		DDPF_ALPHA = 0x2,
		DDPF_FOURCC = 0x4,
		DDPF_RGB = 0x40,
		DDPF_YUV = 0x200,
		DDPF_LUMINANCE = 0x20000,
	};

	enum DDSCAPS2
	{
		DDSCAPS2_CUBEMAP = 0x200,
		DDSCAPS2_CUBEMAP_POSITIVEX = 0x400,
		DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800,
		DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000,
		DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000,
		DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000,
		DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000,
		DDSCAPS2_VOLUME = 0x200000
	};

	enum D3D10_RESOURCE_DIMENSION
    {
        D3D10_RESOURCE_DIMENSION_TEXTURE1D	= 2,
        D3D10_RESOURCE_DIMENSION_TEXTURE2D	= 3,
        D3D10_RESOURCE_DIMENSION_TEXTURE3D	= 4
    };

	enum D3D10_RESOURCE_MISC_FLAG
	{ 
		D3D10_RESOURCE_MISC_GENERATE_MIPS      = 0x1L,
		D3D10_RESOURCE_MISC_SHARED             = 0x2L,
		D3D10_RESOURCE_MISC_TEXTURECUBE        = 0x4L,
		D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX  = 0x10L,
		D3D10_RESOURCE_MISC_GDI_COMPATIBLE     = 0x20L
	};

	enum DDS_ALPHA_MODE
	{
		DDS_ALPHA_MODE_UNKNOWN = 0x0,
		DDS_ALPHA_MODE_STRAIGHT = 0x1,
		DDS_ALPHA_MODE_PREMULTIPLIED = 0x2,
		DDS_ALPHA_MODE_OPAQUE = 0x3,
		DDS_ALPHA_MODE_CUSTOM = 0x4,
	};

	enum DXGI_FORMAT
	{
		DXGI_FORMAT_UNKNOWN	                    = 0,
		DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
		DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
		DXGI_FORMAT_R32G32B32A32_UINT           = 3,
		DXGI_FORMAT_R32G32B32A32_SINT           = 4,
		DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
		DXGI_FORMAT_R32G32B32_FLOAT             = 6,
		DXGI_FORMAT_R32G32B32_UINT              = 7,
		DXGI_FORMAT_R32G32B32_SINT              = 8,
		DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
		DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
		DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
		DXGI_FORMAT_R16G16B16A16_UINT           = 12,
		DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
		DXGI_FORMAT_R16G16B16A16_SINT           = 14,
		DXGI_FORMAT_R32G32_TYPELESS             = 15,
		DXGI_FORMAT_R32G32_FLOAT                = 16,
		DXGI_FORMAT_R32G32_UINT                 = 17,
		DXGI_FORMAT_R32G32_SINT                 = 18,
		DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
		DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
		DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
		DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
		DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
		DXGI_FORMAT_R10G10B10A2_UINT            = 25,
		DXGI_FORMAT_R11G11B10_FLOAT             = 26,
		DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
		DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
		DXGI_FORMAT_R8G8B8A8_UINT               = 30,
		DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
		DXGI_FORMAT_R8G8B8A8_SINT               = 32,
		DXGI_FORMAT_R16G16_TYPELESS             = 33,
		DXGI_FORMAT_R16G16_FLOAT                = 34,
		DXGI_FORMAT_R16G16_UNORM                = 35,
		DXGI_FORMAT_R16G16_UINT                 = 36,
		DXGI_FORMAT_R16G16_SNORM                = 37,
		DXGI_FORMAT_R16G16_SINT                 = 38,
		DXGI_FORMAT_R32_TYPELESS                = 39,
		DXGI_FORMAT_D32_FLOAT                   = 40,
		DXGI_FORMAT_R32_FLOAT                   = 41,
		DXGI_FORMAT_R32_UINT                    = 42,
		DXGI_FORMAT_R32_SINT                    = 43,
		DXGI_FORMAT_R24G8_TYPELESS              = 44,
		DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
		DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
		DXGI_FORMAT_R8G8_TYPELESS               = 48,
		DXGI_FORMAT_R8G8_UNORM                  = 49,
		DXGI_FORMAT_R8G8_UINT                   = 50,
		DXGI_FORMAT_R8G8_SNORM                  = 51,
		DXGI_FORMAT_R8G8_SINT                   = 52,
		DXGI_FORMAT_R16_TYPELESS                = 53,
		DXGI_FORMAT_R16_FLOAT                   = 54,
		DXGI_FORMAT_D16_UNORM                   = 55,
		DXGI_FORMAT_R16_UNORM                   = 56,
		DXGI_FORMAT_R16_UINT                    = 57,
		DXGI_FORMAT_R16_SNORM                   = 58,
		DXGI_FORMAT_R16_SINT                    = 59,
		DXGI_FORMAT_R8_TYPELESS                 = 60,
		DXGI_FORMAT_R8_UNORM                    = 61,
		DXGI_FORMAT_R8_UINT                     = 62,
		DXGI_FORMAT_R8_SNORM                    = 63,
		DXGI_FORMAT_R8_SINT                     = 64,
		DXGI_FORMAT_A8_UNORM                    = 65,
		DXGI_FORMAT_R1_UNORM                    = 66,
		DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
		DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
		DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
		DXGI_FORMAT_BC1_TYPELESS                = 70,
		DXGI_FORMAT_BC1_UNORM                   = 71,
		DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
		DXGI_FORMAT_BC2_TYPELESS                = 73,
		DXGI_FORMAT_BC2_UNORM                   = 74,
		DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
		DXGI_FORMAT_BC3_TYPELESS                = 76,
		DXGI_FORMAT_BC3_UNORM                   = 77,
		DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
		DXGI_FORMAT_BC4_TYPELESS                = 79,
		DXGI_FORMAT_BC4_UNORM                   = 80,
		DXGI_FORMAT_BC4_SNORM                   = 81,
		DXGI_FORMAT_BC5_TYPELESS                = 82,
		DXGI_FORMAT_BC5_UNORM                   = 83,
		DXGI_FORMAT_BC5_SNORM                   = 84,
		DXGI_FORMAT_B5G6R5_UNORM                = 85,
		DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
		DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
		DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
		DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
		DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
		DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
		DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
		DXGI_FORMAT_BC6H_TYPELESS               = 94,
		DXGI_FORMAT_BC6H_UF16                   = 95,
		DXGI_FORMAT_BC6H_SF16                   = 96,
		DXGI_FORMAT_BC7_TYPELESS                = 97,
		DXGI_FORMAT_BC7_UNORM                   = 98,
		DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
		DXGI_FORMAT_AYUV                        = 100,
		DXGI_FORMAT_Y410                        = 101,
		DXGI_FORMAT_Y416                        = 102,
		DXGI_FORMAT_NV12                        = 103,
		DXGI_FORMAT_P010                        = 104,
		DXGI_FORMAT_P016                        = 105,
		DXGI_FORMAT_420_OPAQUE                  = 106,
		DXGI_FORMAT_YUY2                        = 107,
		DXGI_FORMAT_Y210                        = 108,
		DXGI_FORMAT_Y216                        = 109,
		DXGI_FORMAT_NV11                        = 110,
		DXGI_FORMAT_AI44                        = 111,
		DXGI_FORMAT_IA44                        = 112,
		DXGI_FORMAT_P8                          = 113,
		DXGI_FORMAT_A8P8                        = 114,
		DXGI_FORMAT_B4G4R4A4_UNORM              = 115,
		DXGI_FORMAT_FORCE_UINT                  = 0xffffffff
	};

	enum D3DFORMAT
	{
		D3DFMT_UNKNOWN              =  0,

		D3DFMT_R8G8B8               = 20,
		D3DFMT_A8R8G8B8             = 21,
		D3DFMT_X8R8G8B8             = 22,
		D3DFMT_R5G6B5               = 23,
		D3DFMT_X1R5G5B5             = 24,
		D3DFMT_A1R5G5B5             = 25,
		D3DFMT_A4R4G4B4             = 26,
		D3DFMT_R3G3B2               = 27,
		D3DFMT_A8                   = 28,
		D3DFMT_A8R3G3B2             = 29,
		D3DFMT_X4R4G4B4             = 30,
		D3DFMT_A2B10G10R10          = 31,
		D3DFMT_A8B8G8R8             = 32,
		D3DFMT_X8B8G8R8             = 33,
		D3DFMT_G16R16               = 34,
		D3DFMT_A2R10G10B10          = 35,
		D3DFMT_A16B16G16R16         = 36,

		D3DFMT_A8P8                 = 40,
		D3DFMT_P8                   = 41,

		D3DFMT_L8                   = 50,
		D3DFMT_A8L8                 = 51,
		D3DFMT_A4L4                 = 52,

		D3DFMT_V8U8                 = 60,
		D3DFMT_L6V5U5               = 61,
		D3DFMT_X8L8V8U8             = 62,
		D3DFMT_Q8W8V8U8             = 63,
		D3DFMT_V16U16               = 64,
		D3DFMT_A2W10V10U10          = 67,

		D3DFMT_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
		D3DFMT_R8G8_B8G8            = MAKEFOURCC('R', 'G', 'B', 'G'),
		D3DFMT_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
		D3DFMT_G8R8_G8B8            = MAKEFOURCC('G', 'R', 'G', 'B'),
		D3DFMT_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
		D3DFMT_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
		D3DFMT_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
		D3DFMT_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
		D3DFMT_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

		D3DFMT_ATI1                 = MAKEFOURCC('A', 'T', 'I', '1'),
		D3DFMT_ATI2                 = MAKEFOURCC('A', 'T', 'I', '2'),

		D3DFMT_BC4U                 = MAKEFOURCC('B', 'C', '4', 'U'),
		D3DFMT_BC4S                 = MAKEFOURCC('B', 'C', '4', 'S'),

		D3DFMT_BC5U                 = MAKEFOURCC('B', 'C', '5', 'U'),
		D3DFMT_BC5S                 = MAKEFOURCC('B', 'C', '5', 'S'),

		D3DFMT_D16_LOCKABLE         = 70,
		D3DFMT_D32                  = 71,
		D3DFMT_D15S1                = 73,
		D3DFMT_D24S8                = 75,
		D3DFMT_D24X8                = 77,
		D3DFMT_D24X4S4              = 79,
		D3DFMT_D16                  = 80,

		D3DFMT_D32F_LOCKABLE        = 82,
		D3DFMT_D24FS8               = 83,

		/* Z-Stencil formats valid for CPU access */
		D3DFMT_D32_LOCKABLE         = 84,
		D3DFMT_S8_LOCKABLE          = 85,

		D3DFMT_L16                  = 81,

		D3DFMT_VERTEXDATA           =100,
		D3DFMT_INDEX16              =101,
		D3DFMT_INDEX32              =102,

		D3DFMT_Q16W16V16U16         =110,

		D3DFMT_MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),

		// Floating point surface formats

		// s10e5 formats (16-bits per channel)
		D3DFMT_R16F                 = 111,
		D3DFMT_G16R16F              = 112,
		D3DFMT_A16B16G16R16F        = 113,

		// IEEE s23e8 formats (32-bits per channel)
		D3DFMT_R32F                 = 114,
		D3DFMT_G32R32F              = 115,
		D3DFMT_A32B32G32R32F        = 116,

		D3DFMT_CxV8U8               = 117,

		// Monochrome 1 bit per pixel format
		D3DFMT_A1                   = 118,

		// 2.8 biased fixed point
		D3DFMT_A2B10G10R10_XR_BIAS  = 119,

		// Binary format indicating that the data has no inherent type
		D3DFMT_BINARYBUFFER         = 199,

		D3DFMT_FORCE_DWORD          = 0x7fffffff
	};

	struct DDS_HEADER_DXT10
	{
		DXGI_FORMAT dxgiFormat;
		D3D10_RESOURCE_DIMENSION resourceDimension;
		BcU32 miscFlag;
		BcU32 arraySize;
		BcU32 miscFlags2;
	};

	RsResourceFormat GetResourceFormat( DXGI_FORMAT Format )
	{
		switch( Format )
		{
		case DXGI_FORMAT_BC1_UNORM:
			return RsResourceFormat::BC1_UNORM;
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			return RsResourceFormat::BC1_UNORM_SRGB;

		case DXGI_FORMAT_BC2_UNORM:
			return RsResourceFormat::BC2_UNORM;
		case DXGI_FORMAT_BC2_UNORM_SRGB:
			return RsResourceFormat::BC2_UNORM_SRGB;

		case DXGI_FORMAT_BC3_UNORM:
			return RsResourceFormat::BC3_UNORM;
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			return RsResourceFormat::BC3_UNORM_SRGB;

		case DXGI_FORMAT_BC4_UNORM:
			return RsResourceFormat::BC4_UNORM;
		case DXGI_FORMAT_BC4_SNORM:
			return RsResourceFormat::BC4_SNORM;

		case DXGI_FORMAT_BC5_UNORM:
			return RsResourceFormat::BC5_UNORM;
		case DXGI_FORMAT_BC5_SNORM:
			return RsResourceFormat::BC5_SNORM;

		case DXGI_FORMAT_BC6H_UF16:
			return RsResourceFormat::BC6H_UF16;
		case DXGI_FORMAT_BC6H_SF16:
			return RsResourceFormat::BC6H_SF16;

		case DXGI_FORMAT_BC7_UNORM:
			return RsResourceFormat::BC7_UNORM;
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return RsResourceFormat::BC7_UNORM_SRGB;
		}
		return RsResourceFormat::UNKNOWN;
	}

	RsResourceFormat GetResourceFormat( D3DFORMAT Format )
	{
		switch( Format )
		{
		case D3DFMT_DXT1:
			return RsResourceFormat::BC1_UNORM;

		case D3DFMT_DXT2:
		case D3DFMT_DXT3:
			return RsResourceFormat::BC2_UNORM;

		case D3DFMT_DXT4:
		case D3DFMT_DXT5:
			return RsResourceFormat::BC3_UNORM;

		case D3DFMT_ATI1:
			return RsResourceFormat::BC4_UNORM;

		case D3DFMT_ATI2:
			return RsResourceFormat::BC5_UNORM;

		case D3DFMT_BC4U:
			return RsResourceFormat::BC4_UNORM;
		case D3DFMT_BC4S:
			return RsResourceFormat::BC4_SNORM;

		case D3DFMT_BC5U:
			return RsResourceFormat::BC5_UNORM;
		case D3DFMT_BC5S:
			return RsResourceFormat::BC5_SNORM;
		}

		return RsResourceFormat::UNKNOWN;
	}
}

bool ScnTextureImport::loadDDS( const char* FileName )
{
	using namespace DDS;

	ScnTextureHeader OutHeader;
	BcStream OutStream;
	auto ResolvedPath = CsPaths::resolveContent( FileName );
	CsResourceImporter::addDependency( ResolvedPath.c_str() );

	BcFile File;
	if( !File.open( ResolvedPath.c_str() ) )
	{
		return false;
	}

	BcU32 Magic = 0;
	DDS_HEADER DDSHeader = {};
	DDS_HEADER_DXT10 DDSHeaderDXT10 = {};

	// Read magic.
	File.read( &Magic, sizeof( Magic ) );
	if( Magic != 0x20534444 )
	{
		return false;
	}

	// Read header.
	File.read( &DDSHeader, sizeof( DDSHeader ) );

	OutHeader.Width_ = DDSHeader.dwWidth;
	OutHeader.Height_ = DDSHeader.dwHeight;
	OutHeader.Depth_ = DDSHeader.dwDepth;
	OutHeader.Levels_ = DDSHeader.dwMipMapCount;

	const BcU32 Flags1D = DDSD_WIDTH;
	const BcU32 Flags2D = DDSD_WIDTH | DDSD_HEIGHT;
	const BcU32 Flags3D = DDSD_WIDTH | DDSD_HEIGHT | DDSD_DEPTH;
	const BcU32 FlagsCube = DDSCAPS2_CUBEMAP;
	if( BcContainsAllFlags( DDSHeader.dwFlags, Flags1D ) )
	{
		OutHeader.Type_ = RsTextureType::TEX1D;
	}
	if( BcContainsAllFlags( DDSHeader.dwFlags, Flags2D ) )
	{
		OutHeader.Type_ = RsTextureType::TEX2D;
	}
	if( BcContainsAllFlags( DDSHeader.dwFlags, Flags3D ) )
	{
		OutHeader.Type_ = RsTextureType::TEX3D;
	}
	if( BcContainsAllFlags( DDSHeader.dwCaps2, FlagsCube ) )
	{
		OutHeader.Type_ = RsTextureType::TEXCUBE;
	}

	OutHeader.Format_ = DDS::GetResourceFormat( DDS::D3DFORMAT( DDSHeader.ddspf.dwFourCC ) );
	OutHeader.Editable_ = BcFalse;
	OutHeader.BindFlags_ = RsBindFlags::SHADER_RESOURCE;

	// Check for DX10 format.
	if( BcContainsAllFlags( DDSHeader.ddspf.dwFlags, BcU32( DDPF_FOURCC ) ) &&
		DDSHeader.ddspf.dwFourCC == MAKEFOURCC( 'D', 'X', '1', '0' ) )
	{
		File.read( &DDSHeaderDXT10, sizeof( DDSHeaderDXT10 ) );
		OutHeader.Format_ = DDS::GetResourceFormat( DDSHeaderDXT10.dxgiFormat );
	}

	// No format determined, log error and fail.
	if( OutHeader.Format_ == RsResourceFormat::UNKNOWN )
	{
		std::array< char, 4096 > Error = { 0 };
		BcSPrintf( Error.data(), Error.size(), "Unable to load texture \"%s\", unsupported format.", FileName );
		CsResourceImporter::addMessage( CsMessageCategory::ERROR, Error.data() );
		return false;
	}

	// Calculate size.
	auto FormatSize = RsResourceFormatSize( OutHeader.Format_, OutHeader.Width_, OutHeader.Height_, OutHeader.Depth_, OutHeader.Levels_ );
	std::unique_ptr< BcU8[] > Data( new BcU8[ FormatSize ] );
	File.read( Data.get(), FormatSize );
	OutStream.push( Data.get(), FormatSize );

	// Add chunks.
	CsResourceImporter::addChunk( BcHash( "header" ), &OutHeader, sizeof( OutHeader ), 16, csPCF_IN_PLACE );
	CsResourceImporter::addChunk( BcHash( "body" ), OutStream.pData(), OutStream.dataSize() );

	return true;
}

#endif // PSY_IMPORT_PIPELINE
