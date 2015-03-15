/**************************************************************************
*
* File:		ScnPhysicsFileData.h
* Author:	Neil Richardson 
* Ver/Date:	
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
#include "Math/MaAABB.h"

//////////////////////////////////////////////////////////////////////////
// ScnPhysicsMeshShapeType
enum class ScnPhysicsMeshShapeType : BcU32
{
	BVH,
	GIMPACT,

	STATIC = BVH,
	DYNAMIC = GIMPACT
};	

//////////////////////////////////////////////////////////////////////////
// ScnPhysicsMeshHeader
struct ScnPhysicsMeshHeader
{
	ScnPhysicsMeshShapeType ShapeType_;
	BcU32 NoofTriangles_;
	BcU32 NoofVertices_;
	BcU32 NoofMaterials_;
	MaAABB AABB_;
};

//////////////////////////////////////////////////////////////////////////
// ScnPhysicsTriangle
struct ScnPhysicsTriangle
{
	BcU32 Indices_[ 3 ];
	BcU32 Material_;		/// TODO: Consider per face properties for E and F?
};

//////////////////////////////////////////////////////////////////////////
// ScnPhysicsVertex
struct ScnPhysicsVertex
{
	MaVec4d Position_;
};

//////////////////////////////////////////////////////////////////////////
// ScnPhysicsMaterial
// TODO: Resource?
struct ScnPhysicsMaterial
{
	BcF32 Friction_;
	BcF32 Restitution_;
	BcU32 CollisionFlags_;
};

#endif
