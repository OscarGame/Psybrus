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

#include "System/Scene/Rendering/ScnModel.h"
#include "System/Scene/Animation/ScnAnimation.h"
#include "System/Scene/Animation/ScnAnimationTreeNode.h"

//////////////////////////////////////////////////////////////////////////
// ScnAnimationComponentRef
typedef ReObjectRef< class ScnAnimationComponent > ScnAnimationComponentRef;

//////////////////////////////////////////////////////////////////////////
// ScnCanvasComponent
class ScnAnimationComponent:
	public ScnComponent
{
public:
	REFLECTION_DECLARE_DERIVED( ScnAnimationComponent, ScnComponent );

	ScnAnimationComponent();
	virtual ~ScnAnimationComponent();

	void destroy() override;

	void onAttach( ScnEntityWeakRef Parent ) override;
	void onDetach( ScnEntityWeakRef Parent ) override;

	/**
	 * Find a node by type.
	 */
	template< class _Ty >
	_Ty* findNodeByType( const BcName& Name )
	{
		return static_cast< _Ty* >( findNodeRecursively( Tree_, Name, _Ty::StaticGetClass() ) );
	}

private:
	void buildReferencePose();
	void applyPose();
	ScnAnimationTreeNode* findNodeRecursively( ScnAnimationTreeNode* pStartNode, const BcName& Name, const ReClass* Class );

	static void decode( const ScnComponentList& Components );
	static void pose( const ScnComponentList& Components );
	static void advance( const ScnComponentList& Components );

private:
	BcName Target_;
	ScnModelComponentRef Model_;

	/// Node file data for the model we are building a pose for.
	std::vector< ScnAnimationNodeFileData > ModelNodeFileData_;

	/// Root node in the animation tree.
	ScnAnimationTreeNode* Tree_;

	/// Reference pose.
	ScnAnimationPose* pReferencePose_;
};

#endif
