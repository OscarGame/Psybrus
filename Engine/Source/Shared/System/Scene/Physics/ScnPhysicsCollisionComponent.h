/**************************************************************************
*
* File:		ScnPhysicsCollisionShape.h
* Author:	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __ScnPhysicsCollisionShape_H__
#define __ScnPhysicsCollisionShape_H__

#include "System/Scene/ScnComponent.h"

//////////////////////////////////////////////////////////////////////////
// ScnPhysicsCollisionComponent
class ScnPhysicsCollisionComponent:
	public ScnComponent
{
public:
	REFLECTION_DECLARE_DERIVED( ScnPhysicsCollisionComponent, ScnComponent );
	
	ScnPhysicsCollisionComponent();
	virtual ~ScnPhysicsCollisionComponent();

	void setLocalScaling( const MaVec3d& LocalScaling );

	class btCollisionShape* getCollisionShape();


protected:
	MaVec3d LocalScaling_;

	class btCollisionShape* CollisionShape_;
};

#endif
