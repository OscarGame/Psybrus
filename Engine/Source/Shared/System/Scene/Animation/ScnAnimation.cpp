/**************************************************************************
*
* File:		ScnAnimation.cpp
* Author:	Neil Richardson 
* Ver/Date:	05/01/13	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Scene/Animation/ScnAnimation.h"

#include "System/Content/CsCore.h"
#include "Base/BcProfiler.h"

#ifdef PSY_IMPORT_PIPELINE
#include "System/Scene/Import/ScnAnimationImport.h"
#endif

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED( ScnAnimation );

void ScnAnimation::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "Header_",			&ScnAnimation::Header_ ),
	};
		
	auto& Class = ReRegisterClass< ScnAnimation, Super >( Fields );
	BcUnusedVar( Class );
	
#ifdef PSY_IMPORT_PIPELINE
	// Add importer attribute to class for resource system to use.
	Class.addAttribute( new CsResourceImporterAttribute( 
		ScnAnimationImport::StaticGetClass(), 0 ) );
#endif
}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnAnimation::ScnAnimation()
{
}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
ScnAnimation::~ScnAnimation()
{
}

//////////////////////////////////////////////////////////////////////////
// getNodeByIndex
const ScnAnimationNodeFileData* ScnAnimation::getNodeByIndex( BcU32 Idx ) const
{
	if( Idx < Header_.NoofNodes_ )
	{
		return &pNodeData_[ Idx ];
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// getNodeIndexByName
BcU32 ScnAnimation::getNodeIndexByName( BcName Name ) const
{
	for( BcU32 Idx = 0; Idx < Header_.NoofNodes_; ++Idx )
	{
		if( pNodeData_[ Idx ].Name_ == Name )
		{
			return Idx;
		}
	}
	
	return BcErrorCode;
}

//////////////////////////////////////////////////////////////////////////
// findPoseIndexAtTime
BcU32 ScnAnimation::findPoseIndexAtTime( BcF32 Time ) const 
{
	PSY_PROFILER_SECTION( "ScnAnimation::findPoseIndexAtTime" );

	// Setup indices.
	BcU32 MinIdx = 0;
	BcU32 MidIdx = 0;
	BcU32 MaxIdx = Header_.NoofPoses_ - 2;

	// Binary search.
	BcBool KeepSearching = BcTrue;
	do
	{
		MidIdx = ( MinIdx + MaxIdx ) / 2;
		const ScnAnimationPoseFileData* pPoseA = &pPoseData_[ MidIdx ];
		const ScnAnimationPoseFileData* pPoseB = &pPoseData_[ MidIdx + 1 ];

		if( Time >= pPoseA->Time_ )
		{
			if( Time <= pPoseB->Time_ )
			{
				KeepSearching = BcFalse;
			}
			else
			{
				KeepSearching = MinIdx != MidIdx;
				MinIdx = MidIdx;
			}
		}
		else
		{
			KeepSearching = MaxIdx != MidIdx;
			MaxIdx = MidIdx;
		}
	}
	while( KeepSearching );

	return MidIdx;
}

//////////////////////////////////////////////////////////////////////////
// findPoseAtIndex
const ScnAnimationPoseFileData*	ScnAnimation::findPoseAtIndex( BcU32 Idx ) const 
{
	if( Idx < Header_.NoofPoses_ )
	{
		return &pPoseData_[ Idx ];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// findKeyDataStartForPose
const BcU8* ScnAnimation::findKeyDataStartForPose( const ScnAnimationPoseFileData* pPose ) const
{
	return &pKeyData_[ pPose->KeyDataOffset_ ];
}

//////////////////////////////////////////////////////////////////////////
// getLength
BcF32 ScnAnimation::getLength() const
{
	const ScnAnimationPoseFileData* pPose = &pPoseData_[ Header_.NoofPoses_ - 1 ];
	return pPose->Time_;
}

//////////////////////////////////////////////////////////////////////////
// decodePoseAtIndexTyped
template< typename _Ty >
void ScnAnimation::decodePoseAtIndexTyped( 
	BcU32 Idx, 
	ScnAnimationPose* pOutputPose, 
	ScnAnimationNodeFileData* TargetNodesFileData ) const
{
	const ScnAnimationPoseFileData* pPoseFileData = findPoseAtIndex( Idx );
	if( pPoseFileData != nullptr )
	{
		const BcU8* pKeyData = findKeyDataStartForPose( pPoseFileData );
		const _Ty* pTransformKeys = reinterpret_cast< const _Ty* >( pKeyData );

		ScnAnimationTransform Transform;
		for( BcU32 NodeIdx = 0; NodeIdx < Header_.NoofNodes_; ++NodeIdx )
		{
			pTransformKeys[ NodeIdx ].unpack( Transform.R_, Transform.S_, Transform.T_ );

			// Find matching output node.
			// TODO: Have a mapping object rather than doing this each decode step.
			for( BcU32 TargetIdx = 0; TargetIdx < pOutputPose->getNoofNodes(); ++TargetIdx )
			{
				if( TargetNodesFileData[ TargetIdx ].Name_ == pNodeData_[ NodeIdx ].Name_ )
				{
					pOutputPose->setTransform( TargetIdx, Transform );
					break;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// decodePoseAtIndex
void ScnAnimation::decodePoseAtIndex( 
	BcU32 Idx,
	ScnAnimationPose* pOutputPose,
	ScnAnimationNodeFileData* TargetNodesFileData ) const
{
	PSY_PROFILER_SECTION( "ScnAnimation::decodePoseAtIndex" );

	switch( Header_.Packing_ )
	{
	case scnAP_R16S32T32:
		decodePoseAtIndexTyped< ScnAnimationTransformKey_R16S32T32 >( Idx, pOutputPose, TargetNodesFileData );
		break;
	case scnAP_R16S16T16:
		decodePoseAtIndexTyped< ScnAnimationTransformKey_R16S16T16 >( Idx, pOutputPose, TargetNodesFileData );
		break;
	case scnAP_R16T32:
		decodePoseAtIndexTyped< ScnAnimationTransformKey_R16T32 >( Idx, pOutputPose, TargetNodesFileData );
		break;
	case scnAP_R16T16:
		decodePoseAtIndexTyped< ScnAnimationTransformKey_R16T16 >( Idx, pOutputPose, TargetNodesFileData );
		break;
	default:
		BcBreakpoint;
	}
}

//////////////////////////////////////////////////////////////////////////
// fileReady
void ScnAnimation::fileReady()
{
	// File is ready, get the header chunk.
	requestChunk( 0, &Header_ );
}

//////////////////////////////////////////////////////////////////////////
// fileChunkReady
void ScnAnimation::fileChunkReady( BcU32 ChunkIdx, BcU32 ChunkID, void* pData )
{
	if( ChunkID == BcHash( "header" ) )
	{		
		// Grab pose and key chunks.
		requestChunk( 1 );
		requestChunk( 2 );
		requestChunk( 3 );
	}
	else if( ChunkID == BcHash( "nodes" ) )
	{
		pNodeData_ = reinterpret_cast< ScnAnimationNodeFileData* >( pData );

		for( BcU32 Idx = 0; Idx < Header_.NoofNodes_; ++Idx )
		{
			getPackage()->markupName( pNodeData_[ Idx ].Name_ );
		}
	}
	else if( ChunkID == BcHash( "poses" ) )
	{
		pPoseData_ = reinterpret_cast< const ScnAnimationPoseFileData* >( pData ); 
	}
	else if( ChunkID == BcHash( "keys" ) )
	{
		pKeyData_ = reinterpret_cast< const BcU8* >( pData );
		
		markCreate();
		markReady();
	}

}
