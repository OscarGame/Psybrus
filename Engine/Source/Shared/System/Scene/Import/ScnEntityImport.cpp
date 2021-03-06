/**************************************************************************
*
* File:		ScnEntityImport.cpp
* Author:	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Scene/Import/ScnEntityImport.h"
#include "System/Scene/Import/ScnComponentImport.h"
#include "System/Scene/Import/ScnEntityObjectCodec.h"
#include "System/Scene/ScnEntity.h"


#include "Serialisation/SeJsonReader.h"
#include "Serialisation/SeJsonWriter.h"

#include "Base/BcFile.h"
#include "Base/BcStream.h"

//////////////////////////////////////////////////////////////////////////
// Reflection
REFLECTION_DEFINE_DERIVED( ScnEntityImport )
	
void ScnEntityImport::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "Entity_", &ScnEntityImport::Entity_, bcRFF_IMPORTER ),
		new ReField( "LocalTransform_", &ScnEntityImport::LocalTransform_, bcRFF_IMPORTER ),
		new ReField( "Components_", &ScnEntityImport::Components_, bcRFF_IMPORTER )
	};
	
	ReRegisterClass< ScnEntityImport, Super >( Fields );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnEntityImport::ScnEntityImport()
{

}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnEntityImport::ScnEntityImport( ScnEntity* Entity ):
	CsResourceImporter( *Entity->getName(), *Entity->getTypeName() )
{
	// Copy in all serialised data.
	Components_.reserve( Entity->getNoofComponents() );
	for( BcU32 Idx = 0; Idx < Entity->getNoofComponents(); ++Idx )
	{
		Components_.push_back( Entity->getComponent( Idx ) );
	}
	LocalTransform_ = Entity->getLocalMatrix();

	CsCore::pImpl()->internalForceDestroy( Entity );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnEntityImport::ScnEntityImport( ReNoInit )
{

}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
ScnEntityImport::~ScnEntityImport()
{
}

//////////////////////////////////////////////////////////////////////////
// import
BcBool ScnEntityImport::import()
{
#if PSY_IMPORT_PIPELINE
	
	if( Entity_.size() != 0 )
	{
		// Perform load.
		ScnEntityObjectCodec ObjectCodec( nullptr );
		SeJsonReader Reader( &ObjectCodec );

		auto ResolvedPath = CsPaths::resolveContent( Entity_.c_str() );
		if( !Reader.load( ResolvedPath.c_str() ) )
		{
			addMessage( CsMessageCategory::ERROR, "Unable to load entity from \"%s\"", ResolvedPath.c_str() );
			return BcFalse;
		}

		addDependency( ResolvedPath.c_str() );

		Json::Value RootValue = Reader.getRootValue();
		CsResourceImporter::addAllPackageCrossRefs( RootValue );
		Reader.setRootValue( RootValue );

		ScnEntity* Entity = nullptr;
		Reader << Entity;

		if( Entity == nullptr )
		{
			addMessage( CsMessageCategory::ERROR, "Unable to load entity from \"%s\"", ResolvedPath.c_str() );
			return BcFalse;
		}

		// Copy in all serialised data.
		Components_.reserve( Entity->getNoofComponents() );
		for( BcU32 Idx = 0; Idx < Entity->getNoofComponents(); ++Idx )
		{
			Components_.push_back( Entity->getComponent( Idx ) );
		}
		LocalTransform_ = Entity->getLocalMatrix();

		CsCore::pImpl()->internalForceDestroy( Entity );
	}

	BcStream Stream;
	ScnEntityHeader Header;
	Header.LocalTransform_ = LocalTransform_;
	Header.NoofComponents_ = (BcU32)Components_.size();
	Stream << Header;
	for( auto* Component : Components_ )
	{
		// Visit and assign names to any objects without.
		ReVisitRecursively( Component, Component->getClass(),
			[]( void* InData, const ReClass* InClass )
			{
				if( InClass->hasBaseClass( ReObject::StaticGetClass() ) )
				{
					ReObject* Object = static_cast< ReObject* >( InData );
					if( Object->getName() == BcName::INVALID )
					{
						Object->setName( Object->getClass()->getName().getUnique() );
					}
				}
			} );

		// If component is an entity, create entity importer.
		if( Component->isTypeOf< ScnEntity >() )
		{
			// Create entity importer for component.
			CsResourceImporterUPtr ResourceImporter( new ScnEntityImport( static_cast< ScnEntity* >( Component ) ) );
			BcU32 CrossRef = CsResourceImporter::addImport( std::move( ResourceImporter ), BcTrue );
			Stream << CrossRef;
		}
		else
		{
			// Create component importer for component.
			CsResourceImporterUPtr ResourceImporter( new ScnComponentImport( Component ) );
			BcU32 CrossRef = CsResourceImporter::addImport( std::move( ResourceImporter ), BcTrue );
			Stream << CrossRef;
		}
	}
	Components_.clear();
	CsResourceImporter::addChunk( BcHash( "header" ), Stream.pData(), Stream.dataSize() );

	return BcTrue;
#else
	return BcFalse;
#endif // PSY_IMPORT_PIPELINE
}

