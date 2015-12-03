/**************************************************************************
*
* File:		RsTexture.cpp
* Author: 	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Renderer/RsTexture.h"

#include "Base/BcMath.h"

//////////////////////////////////////////////////////////////////////////
// RsTextureDesc
RsTextureDesc::RsTextureDesc():
	Type_( RsTextureType::UNKNOWN ),
	CreationFlags_( RsResourceCreationFlags::NONE ),
	BindFlags_( RsResourceBindFlags::NONE ),
	Format_( RsTextureFormat::INVALID ),
	Width_( 0 ),
	Height_( 0 ),
	Depth_( 0 )
{

}

//////////////////////////////////////////////////////////////////////////
// RsTextureDesc
RsTextureDesc::RsTextureDesc( 
		RsTextureType Type, 
		RsResourceCreationFlags CreationFlags,
		RsResourceBindFlags BindFlags,
		RsTextureFormat Format,
		BcU32 Levels, 
		BcU32 Width, 
		BcU32 Height,
		BcU32 Depth ):
	Type_( Type ),
	CreationFlags_( CreationFlags ),
	BindFlags_( BindFlags ),
	Format_( Format ),
	Levels_( Levels ),
	Width_( Width ),
	Height_( Height ),
	Depth_( Depth )
{
#ifdef PSY_DEBUG
	// Check levels is valid.
	BcAssert( Levels_ > 0 );
	BcAssert( Width_ >= 1 );
	BcAssert( Height_ >= 1 );
 	BcAssert( Depth_ >= 1 );

	// Max num of mips...math fail...must be a simpler way to calculate it.
	BcU32 MinWidth = BcMax( 1, Width_ );
	BcU32 MinHeight = BcMax( 1, Height_ );
	BcU32 MinDepth = BcMax( 1, Depth_ );
	BcU32 MaxLevels = 0;
	while( MinWidth >= 1 || MinHeight >= 1 || MinDepth >= 1 )
	{
		++MaxLevels;
		MinWidth >>= 1;
		MinHeight >>= 1;
		MinDepth >>= 1;
	}
	BcAssertMsg( Levels_ <= MaxLevels, "Levels_ (%u) <= MaxLevels (%u). Width (%u), Height (%u), Depth (%u)", Levels_, MaxLevels, Width_, Height_, Depth_ );

	// Calculate minimum dimension.
	BcU32 MinimumDimension = 1;
	switch( Format_ )
	{
	case RsTextureFormat::DXT1:
	case RsTextureFormat::DXT3:
	case RsTextureFormat::DXT5:
		MinimumDimension = 4;
		break;
	default:
		break;
	}

	// Check vs texture type.
	switch( Type_ )
	{
	case RsTextureType::TEX1D:
		BcAssert( Width_ >= MinimumDimension && Height_ == 1 && Depth_ == 1 );
		break;
	case RsTextureType::TEX2D:
		BcAssert( Width_ >= MinimumDimension && Height_ >= MinimumDimension && Depth_ == 1 );
		break;
	case RsTextureType::TEX3D:
		BcAssert( Width_ >= MinimumDimension && Height_ >= MinimumDimension && Depth_ >= 1 );
		break;
	case RsTextureType::TEXCUBE:
		BcAssert( Width_ >= MinimumDimension && Height_ >= MinimumDimension && Depth_ == 1 );
		break;
	default:
		BcBreakpoint;
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
// Ctor
RsTexture::RsTexture( RsContext* pContext, const RsTextureDesc& Desc ):
	RsResource( RsResourceType::TEXTURE, pContext ),
	Desc_( Desc )
{

}

//////////////////////////////////////////////////////////////////////////
// getDesc
const RsTextureDesc& RsTexture::getDesc() const
{
	return Desc_;
}

//////////////////////////////////////////////////////////////////////////
// getSlice
RsTextureSlice RsTexture::getSlice( BcU32 Level, RsTextureFace Face ) const
{
#ifdef PSY_DEBUG
	// Check level validity.
	BcAssert( Level >= 0 && Level < Desc_.Levels_ );

	// Check face validity.
	if( Desc_.Type_ == RsTextureType::TEXCUBE )
	{
		BcAssert( Face != RsTextureFace::NONE );
	}
#endif

	RsTextureSlice Slice = 
	{
		Level,
		Face
	};
	
	return Slice;
}
