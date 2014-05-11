/**************************************************************************
*
* File:		ScnPhysicsFileData.h
* Author:	Neil Richardson 
* Ver/Date:	25/02/13
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __SCNPHYSICSFILEDATA__
#define __SCNPHYSICSFILEDATA__

#include "Base/BcTypes.h"
#include "Math/MaVec3d.h"

//////////////////////////////////////////////////////////////////////////
// ScnPhysicsCollisionShapeHeader
struct ScnPhysicsCollisionShapeHeader
{

};

//////////////////////////////////////////////////////////////////////////
// ScnPhysicsBoxCollisionShapeHeader
struct ScnPhysicsBoxCollisionShapeHeader:
	public ScnPhysicsCollisionShapeHeader
{
	MaVec3d HalfExtents_;
};

//////////////////////////////////////////////////////////////////////////
// ScnPhysicsSphereCollisionShapeHeader
struct ScnPhysicsSphereCollisionShapeHeader:
	public ScnPhysicsCollisionShapeHeader
{
	BcF32 Radius_;
};

#endif
