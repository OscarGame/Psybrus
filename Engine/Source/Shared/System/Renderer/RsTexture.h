/**************************************************************************
*
* File:		RsTexture.h
* Author: 	Neil Richardson 
* Ver/Date:	
* Description:
*		Simple Texture handling class
*		
*
*
* 
**************************************************************************/

#ifndef __RSTEXTURE_H__
#define __RSTEXTURE_H__

#include "System/Renderer/RsTypes.h"
#include "System/Renderer/RsResource.h"

//////////////////////////////////////////////////////////////////////////
// RsTextureDesc
struct RsTextureDesc
{
	RsTextureDesc();
	RsTextureDesc( 
		RsTextureType Type,
		RsResourceCreationFlags CreationFlags,
		RsBindFlags BindFlags,
		RsResourceFormat Format,
		BcU32 Levels,
		BcU32 Width, 
		BcU32 Height = 1,
		BcU32 Depth = 1 );

	RsTextureType Type_;
	RsResourceCreationFlags CreationFlags_;
	RsBindFlags BindFlags_;
	RsResourceFormat Format_;
	BcU32 Levels_;
	BcU32 Width_;
	BcU32 Height_;
	BcU32 Depth_;	
};

//////////////////////////////////////////////////////////////////////////
// RsTexture
class RsTexture:
	public RsResource
{
public:
	RsTexture( RsContext* pContext, const RsTextureDesc& Desc );
	virtual ~RsTexture(){};

	/**
	 * Get descriptor.
	 */
	const RsTextureDesc& getDesc() const;

	/**
	 * Get slice.
	 * Validity checking is performed by this method.
	 */
	RsTextureSlice getSlice( BcU32 Level = 0, RsTextureFace Face = RsTextureFace::NONE ) const;


protected:
	RsTextureDesc Desc_;
};

#endif
