/**************************************************************************
*
* File:		ScnPhysicsMesh.cpp
* Author:	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/
#include "System/Scene/Physics/ScnPhysicsMesh.h"
#include "System/Scene/Physics/ScnPhysics.h"

#include "System/SysKernel.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#ifdef PSY_IMPORT_PIPELINE
#include "System/Scene/Physics/ScnPhysicsMeshImport.h"
#endif

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED( ScnPhysicsMesh );

void ScnPhysicsMesh::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "Header_", &ScnPhysicsMesh::Header_, bcRFF_CHUNK_DATA ),
	};
		
	auto& Class = ReRegisterClass< ScnPhysicsMesh, Super >( Fields );
	BcUnusedVar( Class );

#ifdef PSY_IMPORT_PIPELINE
	// Add importer attribute to class for resource system to use.
	Class.addAttribute( new CsResourceImporterAttribute( 
		ScnPhysicsMeshImport::StaticGetClass(), 1 ) );
#endif
}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnPhysicsMesh::ScnPhysicsMesh():
	Header_(),
	Triangles_( nullptr ),
	Vertices_( nullptr ),
	MeshInterface_( nullptr ),
	OptimizedBvh_( nullptr )
{
}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
ScnPhysicsMesh::~ScnPhysicsMesh()
{
}

//////////////////////////////////////////////////////////////////////////
// create
//virtual
void ScnPhysicsMesh::create()
{	
	// Setup indexed mesh structure.
	btIndexedMesh IndexedMesh;
	IndexedMesh.m_numTriangles = Header_.NoofTriangles_;
	IndexedMesh.m_triangleIndexBase = reinterpret_cast< const unsigned char* >( Triangles_ );
	IndexedMesh.m_triangleIndexStride = sizeof( ScnPhysicsTriangle );
	IndexedMesh.m_numVertices = Header_.NoofVertices_;
	IndexedMesh.m_vertexBase = reinterpret_cast< const unsigned char* >( Vertices_ );
	IndexedMesh.m_vertexStride = sizeof( ScnPhysicsVertex );
	IndexedMesh.m_indexType = PHY_INTEGER;
	IndexedMesh.m_vertexType = PHY_FLOAT;

	// Create mesh interface.
	auto TriangleIndexVertexArray = new btTriangleIndexVertexArray();
	TriangleIndexVertexArray->addIndexedMesh( IndexedMesh );
	MeshInterface_ = TriangleIndexVertexArray;

	// Create optimised bvh.
	OptimizedBvh_ = new btOptimizedBvh();

	// Build optimised bvh on a worker, mark as ready immediately
	// to maximise concurrency, just wait on access.
	BuildingBvhFence_.increment();
	SysKernel::pImpl()->pushFunctionJob(
		BcErrorCode,
		[ this ]
		{
			;
			OptimizedBvh_->build( 
				MeshInterface_, true, 
				ScnPhysicsToBullet( Header_.AABB_.min() ), ScnPhysicsToBullet( Header_.AABB_.max() ) );
			BuildingBvhFence_.decrement();
		} );


	markReady();
}

//////////////////////////////////////////////////////////////////////////
// destroy
//virtual
void ScnPhysicsMesh::destroy()
{
	BuildingBvhFence_.wait();

	delete MeshInterface_;
	delete OptimizedBvh_;
}

//////////////////////////////////////////////////////////////////////////
// createTriangleMeshShape
btTriangleMeshShape* ScnPhysicsMesh::createTriangleMeshShape()
{
	// Wait for optimised bvh to complete building.
	BuildingBvhFence_.wait();

	// Create bvh triangle mesh shape using our optimised bvh.
	auto MeshShape = new btBvhTriangleMeshShape( MeshInterface_, true, false );
	MeshShape->setOptimizedBvh( OptimizedBvh_ );
	return MeshShape;
}

//////////////////////////////////////////////////////////////////////////
// fileReady
void ScnPhysicsMesh::fileReady()
{
	// File is ready, get the header chunk.
	requestChunk( 0 );
}

//////////////////////////////////////////////////////////////////////////
// fileChunkReady
void ScnPhysicsMesh::fileChunkReady( BcU32 ChunkIdx, BcU32 ChunkID, void* pData )
{
	if( ChunkID == BcHash( "header" ) )
	{
		requestChunk( 1 );
		requestChunk( 2 );
	}
	else if( ChunkID == BcHash( "triangles" ) )
	{
		Triangles_ = reinterpret_cast< const ScnPhysicsTriangle* >( pData );
	}
	else if( ChunkID == BcHash( "vertices" ) )
	{
		Vertices_ = reinterpret_cast< const ScnPhysicsVertex* >( pData );
	}
}