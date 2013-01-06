/**************************************************************************
*
* File:		ScnAnimationComponent.h
* Author:	Neil Richardson 
* Ver/Date:	05/01/13	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __SCNANIMATIONCOMPONENT_H__
#define __SCNANIMATIONCOMPONENT_H__

#include "System/Scene/ScnModel.h"
#include "System/Scene/Animation/ScnAnimation.h"
#include "System/Scene/Animation/ScnAnimationTreeNode.h"

//////////////////////////////////////////////////////////////////////////
// ScnAnimationComponentRef
typedef CsResourceRef< class ScnAnimationComponent > ScnAnimationComponentRef;

//////////////////////////////////////////////////////////////////////////
// ScnCanvasComponent
class ScnAnimationComponent:
	public ScnComponent
{
public:
	DECLARE_RESOURCE( ScnComponent, ScnAnimationComponent );
	
	virtual void						initialise( const Json::Value& Object );

	virtual void						preUpdate( BcF32 Tick );
	virtual void						update( BcF32 Tick );
	virtual void						postUpdate( BcF32 Tick );

	virtual void						onAttach( ScnEntityWeakRef Parent );
	virtual void						onDetach( ScnEntityWeakRef Parent );

	/**
	 * Find a node by type.
	 */
	template< class _Ty >
	_Ty*								findNodeByType( const BcName& Name )
	{
		return static_cast< _Ty* >( findNodeRecursively( pRootTreeNode_, Name, _Ty::StaticGetType() ) );
	}

private:
	void								buildReferencePose();
	void								applyPose();

	ScnAnimationTreeNode*				findNodeRecursively( ScnAnimationTreeNode* pStartNode, const BcName& Name, const BcName& Type );

private:
	BcName								TargetComponentName_;
	ScnModelComponentRef				Model_;

	ScnAnimationTreeNode*				pRootTreeNode_;
	ScnAnimationPose*					pReferencePose_;
};

#endif