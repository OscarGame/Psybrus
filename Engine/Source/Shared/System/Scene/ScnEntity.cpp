/**************************************************************************
*
* File:		ScnEntity.cpp
* Author:	Neil Richardson 
* Ver/Date:	26/11/11
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Scene/ScnEntity.h"
#include "System/Scene/ScnCore.h"
#include "System/Content/CsCore.h"

#ifdef PSY_SERVER
#include "Base/BcStream.h"

//////////////////////////////////////////////////////////////////////////
// import
//virtual
BcBool ScnEntity::import( class CsPackageImporter& Importer, const Json::Value& Object )
{
	// Write out object to be used later.
	Json::FastWriter Writer;
	std::string JsonData = Writer.write( Object );
	
	//
	Importer.addChunk( BcHash( "object" ), JsonData.c_str(), JsonData.size() + 1 );
	
	//
	return Super::import( Importer, Object );
}
#endif

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
DEFINE_RESOURCE( ScnEntity );

//////////////////////////////////////////////////////////////////////////
// initialise
void ScnEntity::initialise()
{
	// NULL internals.
	IsAttached_ = BcFalse;
	pJsonObject_ = NULL;
}

//////////////////////////////////////////////////////////////////////////
// initialise
void ScnEntity::initialise( ScnEntityRef Basis )
{
	// Grab our basis.
	Basis_ = Basis->getBasisEntity();

	// Copy over internals.
	IsAttached_ = BcFalse;
	pJsonObject_ = Basis_->pJsonObject_;
	Transform_ = Basis_->Transform_;

	// Create components from Json.
	// TEMP.
	Json::Value Root;
	Json::Reader Reader;
	if( Reader.parse( pJsonObject_, Root ) )
	{
		//BcPrintf( "** ScnEntity::initialise:\n" );

		//
		const Json::Value& Components = Root[ "components" ];

		for( BcU32 Idx = 0; Idx < Components.size(); ++Idx )
		{
			const Json::Value& Component( Components[ Idx ] );
			CsResourceRef<> ResourceRef;
			if( CsCore::pImpl()->internalCreateResource( BcName::INVALID, Component[ "type" ].asCString(), BcErrorCode, NULL, ResourceRef ) )
			{
				ScnComponentRef ComponentRef( ResourceRef );
				BcAssert( ComponentRef.isValid() );

				//BcPrintf( "** - %s:%s\n", (*ComponentRef->getName()).c_str(), (*ComponentRef->getType()).c_str() );

				// Initialise has already been called...need to change this later.
				ComponentRef->initialise( Component );

				// Attach.
				attach( ComponentRef );
			}			
		}
	}

	Super::initialise();
}

//////////////////////////////////////////////////////////////////////////
// create
void ScnEntity::create()
{

}

//////////////////////////////////////////////////////////////////////////
// destroy
void ScnEntity::destroy()
{

}

//////////////////////////////////////////////////////////////////////////
// isReady
BcBool ScnEntity::isReady()
{
	// TODO: Set a flag internally once stuff has loaded. Will I ever fucking get round to this!?
	for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
	{
		ScnComponentRef& Component( *It );

		if( Component.isReady() == BcFalse )
		{
			return BcFalse;
		}
	}

	return pJsonObject_ != NULL;
}

//////////////////////////////////////////////////////////////////////////
// update
void ScnEntity::update( BcReal Tick )
{
	// Update as component first.
	Super::update( Tick );

	// Process attach/detach.
	processAttachDetach();

	// Update components.
	for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
	{
		ScnComponentRef& Component( *It );

		Component->update( Tick );
	}
}

//////////////////////////////////////////////////////////////////////////
// render
void ScnEntity::render( class ScnViewComponent* pViewComponent, RsFrame* pFrame, RsRenderSort Sort )
{
	// Set all material parameters that the view has info on. (HACK).
	for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
	{
		ScnMaterialComponentRef MaterialComponent( *It );
		if( MaterialComponent.isValid() )
		{
			pViewComponent->setMaterialParameters( MaterialComponent );
		}
	}

	// Render all renderable components.
	for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
	{
		ScnRenderableComponentWeakRef RenderableComponent( *It );
		if( RenderableComponent.isValid() )
		{
			RenderableComponent->render( pViewComponent, pFrame, Sort );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// attach
void ScnEntity::attach( ScnComponent* Component )
{
	BcAssertMsg( Component != NULL, "Trying to attach a null component!" );

	AttachComponents_.push_back( Component );
}

//////////////////////////////////////////////////////////////////////////
// detach
void ScnEntity::detach( ScnComponent* Component )
{
	BcAssertMsg( Component != NULL, "Trying to detach a null component!" );

	DetachComponents_.push_back( Component );
}

//////////////////////////////////////////////////////////////////////////
// onAttachScene
void ScnEntity::onAttach( ScnEntityWeakRef Parent )
{
	// Process attach/detach.
	processAttachDetach();

	BcAssert( IsAttached_ == BcFalse );
	IsAttached_ = BcTrue;

	// Do onAttachComponent for all entities current components.
	for( BcU32 Idx = 0; Idx < getNoofComponents(); ++Idx )
	{
		ScnCore::pImpl()->onAttachComponent( ScnEntityWeakRef( this ), getComponent( Idx ) );
	}

	Super::onAttach( Parent );
}

//////////////////////////////////////////////////////////////////////////
// onAttachScene
void ScnEntity::onDetach( ScnEntityWeakRef Parent )
{
	// All components, get rid of NOW.
	// TODO: See about removing stuff from ScnCore for this.
	Super::onDetach( Parent );

	BcAssert( IsAttached_ == BcTrue );
	IsAttached_ = BcFalse;

	// Do onDetachComponent for all entities current components.
	for( BcU32 Idx = 0; Idx < getNoofComponents(); ++Idx )
	{
		ScnComponentRef Component( getComponent( Idx ) );

		// Can be unattached if the entity has been immediately removed from the scene.
		if( Component->isAttached() == BcTrue )
		{
			ScnCore::pImpl()->onDetachComponent( ScnEntityWeakRef( this ), Component );
			Component->onDetach( ScnEntityWeakRef( this ) );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// isAttached
BcBool ScnEntity::isAttached() const
{
	return IsAttached_ || Super::isAttached();
}

//////////////////////////////////////////////////////////////////////////
// getBasisEntity
ScnEntityRef ScnEntity::getBasisEntity()
{
	// If we have a basis, ask it for it's basis.
	if( Basis_.isValid() )
	{
		return Basis_->getBasisEntity();
	}

	// We have no basis, therefore we are it.
	return this;
}

//////////////////////////////////////////////////////////////////////////
// getNoofComponents
BcU32 ScnEntity::getNoofComponents() const
{
	return Components_.size() + AttachComponents_.size();
}
	
//////////////////////////////////////////////////////////////////////////
// getComponent
ScnComponentRef ScnEntity::getComponent( BcU32 Idx, const BcName& Type )
{
	if( Type == BcName::INVALID )
	{
		BcU32 CurrIdx = 0;
		for( BcU32 ComponentIdx = 0; ComponentIdx < Components_.size(); ++ComponentIdx )
		{
			if( CurrIdx++ == Idx )
			{
				return Components_[ ComponentIdx ];
			}
		}
		for( BcU32 ComponentIdx = 0; ComponentIdx < AttachComponents_.size(); ++ComponentIdx )
		{
			if( CurrIdx++ == Idx )
			{
				return AttachComponents_[ ComponentIdx ];
			}
		}
	}
	else
	{
		// HACK: Seperate into seperate function.
		BcU32 NoofComponents = getNoofComponents();
		BcU32 SearchIdx = 0;
		for( BcU32 ComponentIdx = 0; ComponentIdx < NoofComponents; ++ComponentIdx )
		{
			ScnComponentRef Component = getComponent( ComponentIdx );
			if( Component->getType() == Type )
			{
				if( SearchIdx == Idx )
				{
					return Component;
				}

				++SearchIdx;
			}
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// getComponentAnyParent
ScnComponentRef ScnEntity::getComponentAnyParent( BcU32 Idx, const BcName& Type )
{
	ScnComponentRef Component = getComponent( Idx, Type );

	if( Component.isValid() == BcFalse && getParentEntity().isValid() )
	{
		Component = getParentEntity()->getComponentAnyParent( Idx, Type );
	}

	return Component;
}

//////////////////////////////////////////////////////////////////////////
// setPosition
void ScnEntity::setPosition( const BcVec3d& Position )
{
	Transform_.translation( Position );
}

//////////////////////////////////////////////////////////////////////////
// setMatrix
void ScnEntity::setMatrix( const BcMat4d& Matrix )
{
	Transform_ = Matrix;
}

//////////////////////////////////////////////////////////////////////////
// getPosition
BcVec3d ScnEntity::getPosition() const
{
	return Transform_.translation();
}

//////////////////////////////////////////////////////////////////////////
// getMatrix
const BcMat4d& ScnEntity::getMatrix() const
{
	return Transform_;
}

//////////////////////////////////////////////////////////////////////////
// getAABB
BcAABB ScnEntity::getAABB()
{
	BcAABB AABB;
	
	for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
	{
		AABB.expandBy( (*It)->getAABB() );
	}
	
	return AABB;
}

//////////////////////////////////////////////////////////////////////////
// fileReady
void ScnEntity::fileReady()
{
	// File is ready, get the header chunk.
	requestChunk( 0 );
}

//////////////////////////////////////////////////////////////////////////
// fileChunkReady
void ScnEntity::fileChunkReady( BcU32 ChunkIdx, BcU32 ChunkID, void* pData )
{
	if( ChunkID == BcHash( "object" ) )
	{
		pJsonObject_ = reinterpret_cast< const BcChar* >( pData );
	}
}

//////////////////////////////////////////////////////////////////////////
// processAttachDetach
void ScnEntity::processAttachDetach()
{
	// Detach first.
	while( DetachComponents_.size() > 0 )
	{
		for( ScnComponentListIterator It( DetachComponents_.begin() ); It != DetachComponents_.end(); )
		{
			ScnComponentRef Component( (*It) );
			DetachComponents_.erase( It );
			internalDetach( Component );
			It = DetachComponents_.begin();
		}
	}

	// Attach second.
	while( AttachComponents_.size() > 0 )
	{
		for( ScnComponentListIterator It( AttachComponents_.begin() ); It != AttachComponents_.end(); )
		{
			ScnComponentRef Component( (*It) );
			AttachComponents_.erase( It );
			internalAttach( Component );
			It = AttachComponents_.begin();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// internalAttach
void ScnEntity::internalAttach( ScnComponent* Component )
{
	BcAssertMsg( Component != NULL, "Trying to attach a null component!" );

	// If we're not attached to ourself, bail.
	if( Component->isAttached( this ) == BcFalse )
	{
		BcAssertMsg( Component->isAttached() == BcFalse, "Component is attached to another entity!" );

		// Call the on detach.
		Component->onAttach( ScnEntityWeakRef( this ) );

		// Put into component list.
		Components_.push_back( Component );

		// Tell the scene about it.
		if( isAttached() == BcTrue )
		{
			ScnCore::pImpl()->onAttachComponent( ScnEntityWeakRef( this ), Component );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// internalDetach
void ScnEntity::internalDetach( ScnComponent* Component )
{
	BcAssertMsg( Component != NULL, "Trying to detach a null component!" );

	// If component isn't attached, don't worry. Only a warning?
	if( Component->isAttached() == BcTrue )
	{
		BcAssertMsg( Component->isAttached( this ) == BcTrue, "Component isn't attached to this entity!" );
		// Call the on detach.
		Component->onDetach( ScnEntityWeakRef( this ) );

		// Remove from the list.
		for( ScnComponentListIterator It( Components_.begin() ); It != Components_.end(); ++It )
		{
			ScnComponentRef& ListComponent( *It );

			if( ListComponent == Component )
			{
				// Remove from component list.
				Components_.erase( It );
				break;
			}
		}

		// Tell the scene about it.
		if( isAttached() )
		{
			ScnCore::pImpl()->onDetachComponent( ScnEntityWeakRef( this ), Component );
		}
	}
}
