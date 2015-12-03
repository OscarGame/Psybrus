/**************************************************************************
*
* File:		RsBuffer.h
* Author:	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __RSBUFFER_H__
#define __RSBUFFER_H__

#include "System/Renderer/RsTypes.h"
#include "System/Renderer/RsResource.h"

//////////////////////////////////////////////////////////////////////////
// RsBufferDesc
struct RsBufferDesc
{
	RsBufferDesc();
	RsBufferDesc( RsResourceBindFlags BindFlags, RsResourceCreationFlags Flags, size_t SizeBytes, size_t StructureStride = 0 );

	PSY_DEPRECATED( "Please use constructor which takes RsResourceBindFlags." )
	RsBufferDesc( RsBufferType Type, RsResourceCreationFlags Flags, size_t SizeBytes );
	
	RsResourceBindFlags BindFlags_ = RsResourceBindFlags::NONE;
	RsResourceCreationFlags Flags_ = RsResourceCreationFlags::NONE;
	size_t SizeBytes_ = 0;
	size_t StructureStride_ = 0;
};

//////////////////////////////////////////////////////////////////////////
// RsBuffer
class RsBuffer:
	public RsResource
{
public:
	RsBuffer( class RsContext* pContext, const RsBufferDesc& BufferDesc );
	virtual ~RsBuffer();
	
	const RsBufferDesc& getDesc() const;

private:
	RsBufferDesc BufferDesc_;
};

#endif
