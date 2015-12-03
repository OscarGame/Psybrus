/**************************************************************************
*
* File:		ScnComponent.h
* Author:	Neil Richardson 
* Ver/Date:	26/11/11	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __ScnComponent_H__
#define __ScnComponent_H__

#include "System/Renderer/RsCore.h"
#include "System/Content/CsResource.h"

#include "System/Scene/ScnTypes.h"
#include "System/Scene/ScnVisitor.h"
#include "System/Scene/ScnSpatialTree.h"

#include <json/json.h>

//////////////////////////////////////////////////////////////////////////
// ScnComponentFlags
enum ScnComponentFlags
{
	scnCF_ATTACHED =					0x00000001,
	scnCF_PENDING_ATTACH =				0x00000002,
	scnCF_PENDING_DETACH =				0x00000004,
	scnCF_PENDING_DESTROY =				0x00000008,
	scnCF_DIRTY_ATTACHMENTS =			0x00000010,

	scnCF_WANTS_PREUPDATE =				0x00000100,
	scnCF_WANTS_UPDATE =				0x00000200,
	scnCF_WANTS_POSTUPDATE =			0x00000400,

	scnCF_ANY = 						0xffffffff,
};

//////////////////////////////////////////////////////////////////////////
// ScnComponentVisitType
enum class ScnComponentVisitType
{
	TOP_DOWN,
	BOTTOM_UP
};

//////////////////////////////////////////////////////////////////////////
// ScnComponentVisitFunc
typedef std::function< void( class ScnComponent*, class ScnEntity* ) > ScnComponentVisitFunc;
	
//////////////////////////////////////////////////////////////////////////
// ScnComponent
class ScnComponent:
	public CsResource
{
public:
	REFLECTION_DECLARE_DERIVED_MANUAL_NOINIT( ScnComponent, CsResource );

	ScnComponent();
	ScnComponent( ReNoInit );
	virtual ~ScnComponent();

public:
	virtual void initialise() override;
	void postInitialise();


	virtual void visitHierarchy( 
		ScnComponentVisitType VisitType, 
		ScnEntity* Parent,
		const ScnComponentVisitFunc& Func );

	virtual void onAttach( ScnEntityWeakRef Parent );
	virtual void onDetach( ScnEntityWeakRef Parent );

	void setFlag( ScnComponentFlags Flag );
	void clearFlag( ScnComponentFlags Flag );
	BcBool isFlagSet( ScnComponentFlags Flag ) const;

	BcBool isAttached() const;
	BcBool isAttached( ScnEntityWeakRef Parent ) const;
	void setParentEntity( ScnEntityWeakRef Entity );
	ScnEntity* getParentEntity();
	const ScnEntity* getParentEntity() const;

	/**
	 * Get full name inc. parents.
	 */
	std::string getFullName();

	// temp.
	const BcChar* getJsonObject() const{ return pJsonObject_; }

public:
	// ScnEntity utilities.

	/**
	 * Get component.
	 */
	virtual ScnComponent* getComponent( size_t Idx = 0, const ReClass* Class = nullptr );

	/**
	 * Get component.
	 */
	virtual ScnComponent* getComponent( BcName Name, const ReClass* Class = nullptr );

	/**
	 * Get component by type.
	 */
	template< typename _Ty >
	_Ty* getComponentByType( size_t Idx = 0 )
	{
		return static_cast< _Ty* >( getComponent( Idx, _Ty::StaticGetClass() ) );
	}

	/**
	 * Get component by type.
	 */
	template< typename _Ty >
	_Ty* getComponentByType( BcName Name )
	{
		return static_cast< _Ty* >( getComponent( Name, _Ty::StaticGetClass() ) );
	}

	/**
	 * Get component on any parent or self.
	 */
	virtual ScnComponent* getComponentAnyParent( size_t Idx = 0, const ReClass* Class = nullptr );

	/**
	 * Get component on any parent or self.
	 */
	virtual ScnComponent* getComponentAnyParent( BcName Name, const ReClass* Class = nullptr );

	/**
	 * Get component on any parent or self by type.
	 */
	template< typename _Ty >
	_Ty* getComponentAnyParentByType( BcU32 Idx = 0 )
	{
		return static_cast< _Ty* >( getComponentAnyParent( Idx, _Ty::StaticGetClass() ) );
	}

	/**
	 * Get component on any parent or self by type.
	 */
	template< typename _Ty >
	_Ty* getComponentAnyParentByType( BcName Name )
	{
		return static_cast< _Ty* >( getComponentAnyParent( Name, _Ty::StaticGetClass() ) );
	}


private:
	void create() override;
	void destroy() override;

	void fileReady() override;
	void fileChunkReady( BcU32 ChunkIdx, BcU32 ChunkID, void* pData ) override;

protected:
	BcU32 ComponentFlags_;
	ScnEntityWeakRef ParentEntity_;
	const BcChar* pJsonObject_;

};

#endif
