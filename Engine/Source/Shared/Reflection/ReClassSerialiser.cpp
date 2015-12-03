#include "Reflection/ReReflection.h"

//////////////////////////////////////////////////////////////////////////
// Definitions
REFLECTION_DEFINE_DERIVED( ReClassSerialiser );

void ReClassSerialiser::StaticRegisterClass()
{
	ReRegisterAbstractClass< ReClassSerialiser, Super >();
}

//////////////////////////////////////////////////////////////////////////
// Ctor
ReClassSerialiser::ReClassSerialiser( BcName Name )
{
	Class_ = ReManager::GetClass( Name );
}
	
//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
ReClassSerialiser::~ReClassSerialiser()
{
			
}

//////////////////////////////////////////////////////////////////////////
// getBinaryDataSize
size_t ReClassSerialiser::getBinaryDataSize( const void* ) const
{
	return Class_->getSize();
}

//////////////////////////////////////////////////////////////////////////
// serialiseToBinary
BcBool ReClassSerialiser::serialiseToBinary( const void*, BcBinaryData::Stream& ) const
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
// serialiseFromBinary
BcBool ReClassSerialiser::serialiseFromBinary( void*, const BcBinaryData::Stream& ) const 
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
// serialiseToString
BcBool ReClassSerialiser::serialiseToString( const void*, std::string& ) const
{
	return false;
}
	
//////////////////////////////////////////////////////////////////////////
// serialiseFromString
BcBool ReClassSerialiser::serialiseFromString( void*, const std::string& ) const
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
// copy
BcBool ReClassSerialiser::copy( void* pDst, void* pSrc ) const 
{
	return false;
}
