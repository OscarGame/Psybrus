/**************************************************************************
*
* File:		RsVertexDeclaration.cpp
* Author:	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Renderer/RsVertexDeclaration.h"

//////////////////////////////////////////////////////////////////////////
// RsVertexDeclarationDesc
RsVertexDeclarationDesc::RsVertexDeclarationDesc( BcU32 NoofElements )
{
	Elements_.reserve( NoofElements );
}

//////////////////////////////////////////////////////////////////////////
// addElement
RsVertexDeclarationDesc& RsVertexDeclarationDesc::addElement( const RsVertexElement& Element )
{
	BcAssertMsg( Elements_.size() < Elements_.max_size(), "Adding too many elements." );
	Elements_.push_back( Element );
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// Ctor
RsVertexDeclaration::RsVertexDeclaration( class RsContext* pContext, const RsVertexDeclarationDesc& Desc ):
	RsResource( pContext ),
	Desc_( Desc )
{

}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual 
RsVertexDeclaration::~RsVertexDeclaration()
{

}

//////////////////////////////////////////////////////////////////////////
// getDesc
const RsVertexDeclarationDesc& RsVertexDeclaration::getDesc() const
{
	return Desc_;
}