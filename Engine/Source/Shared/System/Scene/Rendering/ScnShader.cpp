/**************************************************************************
*
* File:		ScnShader.cpp
* Author:	Neil Richardson 
* Ver/Date:	11/04/11	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Scene/Rendering/ScnShader.h"

#include "System/Content/CsCore.h"

#include "Base/BcMath.h"
#include "Base/BcFile.h"

#ifdef PSY_IMPORT_PIPELINE
#include "System/Scene/Import/ScnShaderImport.h"
#endif

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED( ScnShader );

void ScnShader::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "pHeader_", &ScnShader::pHeader_, bcRFF_SHALLOW_COPY | bcRFF_CHUNK_DATA ),
		new ReField( "ShaderMappings_", &ScnShader::ShaderMappings_, bcRFF_TRANSIENT | bcRFF_CONST ),
		new ReField( "ProgramMap_", &ScnShader::ProgramMap_, bcRFF_TRANSIENT | bcRFF_CONST ),
		new ReField( "TargetCodeType_", &ScnShader::TargetCodeType_, bcRFF_CONST ),
		new ReField( "TotalProgramsLoaded_", &ScnShader::TotalProgramsLoaded_, bcRFF_CONST ),
	};
		
	auto& Class = ReRegisterClass< ScnShader, Super >( Fields );
	BcUnusedVar( Class );

#ifdef PSY_IMPORT_PIPELINE
	// Add importer attribute to class for resource system to use.
	Class.addAttribute( new CsResourceImporterAttribute( 
		ScnShaderImport::StaticGetClass(), 0 ) );
#endif
}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnShader::ScnShader():
	pHeader_ ( nullptr ),
	TotalProgramsLoaded_( 0 ),
	TargetCodeType_( RsShaderCodeType::INVALID )
{

}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
ScnShader::~ScnShader()
{

}

//////////////////////////////////////////////////////////////////////////
// create
//virtual
void ScnShader::create()
{
	// TODO: Move some of the fileChunk(...) code into here. Will do for now.
	markReady();
}

//////////////////////////////////////////////////////////////////////////
// destroy
//virtual
void ScnShader::destroy()
{
	for( auto& ShaderMapping : ShaderMappings_ )
	{
		for( auto& ShaderEntry : ShaderMapping.Shaders_ )
		{
			RsCore::pImpl()->destroyResource( ShaderEntry.second );
		}

		ShaderMapping.Shaders_.clear();
	}

	for( auto& ProgramEntry : ProgramMap_ )
	{
		RsCore::pImpl()->destroyResource( ProgramEntry.second );
	}

	ProgramMap_.clear();
	pHeader_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// getProgram
RsProgram* ScnShader::getProgram( ScnShaderPermutationFlags PermutationFlags )
{
	// Find best matching permutation.
	TProgramMapIterator BestIter = ProgramMap_.find( PermutationFlags );
	
	if( BestIter == ProgramMap_.end() )
	{
		// Iterate over map and find best matching permutation (most matching bits).
		BcU32 BestFlagsSet = 0;
		for( TProgramMapIterator Iter = ProgramMap_.begin(); Iter != ProgramMap_.end(); ++Iter )
		{
			BcU32 FlagsSet = BcBitsSet( (BcU32)(*Iter).first & (BcU32)PermutationFlags );
			if( FlagsSet > BestFlagsSet )
			{
				BestIter = Iter;
				BestFlagsSet = FlagsSet;
			}
		}
	}
	
	// We should have found one.
	if( BestIter != ProgramMap_.end() )
	{
		return (*BestIter).second;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// getShader
RsShader* ScnShader::getShader( BcU32 Hash, ScnShader::TShaderMap& ShaderMap )
{
	// Find best matching permutation.
	TShaderMapIterator BestIter = ShaderMap.find( Hash );
		
	// We should have found one.
	if( BestIter != ShaderMap.end() )
	{
		return (*BestIter).second;
	}
	
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// fileReady
void ScnShader::fileReady()
{
	// File is ready, get the header chunk.
	requestChunk( 0 );
}

//////////////////////////////////////////////////////////////////////////
// fileChunkReady
void ScnShader::fileChunkReady( BcU32 ChunkIdx, BcU32 ChunkID, void* pData )
{
	// If we have no render core get chunk 0 so we keep getting entered into.
	if( RsCore::pImpl() == nullptr )
	{
		requestChunk( 0 );
		return;
	}

	if( ChunkID == BcHash( "header" ) )
	{
		pHeader_ = (ScnShaderHeader*)pData;

		// Find out what target code type we should load.
		// Always load in maximum supported.
		RsContext* pContext = RsCore::pImpl()->getContext( nullptr );
		RsShaderCodeType* pCodeTypes = (RsShaderCodeType*)( pHeader_ + 1 );
		for( BcU32 Idx = 0; Idx < pHeader_->NoofShaderCodeTypes_; ++Idx )
		{
			if( pContext->isShaderCodeTypeSupported( pCodeTypes[ Idx ] ) )
			{
				TargetCodeType_ = pCodeTypes[ Idx ];
			}
		}

#if PSY_DEBUG
		if( TargetCodeType_ == RsShaderCodeType::INVALID )
		{
			PSY_LOG( "Supported shader types in %s:", (*getName()).c_str() );
			const ReEnum* Enum = ReManager::GetEnum( "RsShaderCodeType" );
			for( BcU32 Idx = 0; Idx < pHeader_->NoofShaderCodeTypes_; ++Idx )
			{
				auto EnumConstant = Enum->getEnumConstant( static_cast< BcU32 >( pCodeTypes[ Idx ] ) );
				PSY_LOG( "- %s", (*EnumConstant->getName()).c_str() );
			}

		}

#endif
		BcAssertMsg( TargetCodeType_ != RsShaderCodeType::INVALID, "No valid code type built in shader. Please add to your package." );

		// Grab the rest of the chunks.
		const BcU32 TotalChunks = pHeader_->NoofProgramPermutations_ + pHeader_->NoofShaderPermutations_;
		for( BcU32 Idx = 0; Idx < TotalChunks; ++Idx )
		{
			requestChunk( ++ChunkIdx );
		}
	}
	else if( ChunkID == BcHash( "shader" ) )
	{
		ScnShaderUnitHeader* pShaderHeader = (ScnShaderUnitHeader*)pData;
		void* pShaderData = pShaderHeader + 1;
		BcU32 ShaderSize = getChunkSize( ChunkIdx ) - sizeof( ScnShaderUnitHeader );

		if( pShaderHeader->ShaderCodeType_ == TargetCodeType_ )
		{
			// Check for file loading.
			BcU32* pFileLoadTag = (BcU32*)pShaderData;
			BcBool FreeShaderData = BcFalse;
			BcUnusedVar( pFileLoadTag );
			BcUnusedVar( FreeShaderData );
#if !defined( PSY_PRODUCTION )
			if( *pFileLoadTag == ScnShader::LOAD_FROM_FILE_TAG )
			{
				BcFile InputFile;
				auto Filename = (const char*)( pFileLoadTag + 1 );
				InputFile.open( Filename );
				ShaderSize = static_cast< BcU32 >( InputFile.size() );
				pShaderData = InputFile.readAllBytes().release();
				FreeShaderData = BcTrue;
			}
#endif
			PSY_LOG( "Creating shader: %x", pShaderHeader->PermutationFlags_ );
			RsShader* pShader = RsCore::pImpl()->createShader(
				RsShaderDesc( pShaderHeader->ShaderType_, pShaderHeader->ShaderCodeType_ ), 
				pShaderData, ShaderSize,
				*getName() );
			ShaderMappings_[ (BcU32)pShaderHeader->ShaderType_ ].Shaders_[ pShaderHeader->ShaderHash_ ] = pShader;
		}
	}
	else if( ChunkID == BcHash( "program" ) )
	{
		std::vector< RsShader* > Shaders;
		Shaders.reserve( (size_t)RsShaderType::MAX );
		++TotalProgramsLoaded_;

		// Generate program.
		ScnShaderProgramHeader* pProgramHeader = (ScnShaderProgramHeader*)pData;
		
		// Only create target code type.
		if( pProgramHeader->ShaderCodeType_ == TargetCodeType_ )
		{
			for( BcU32 Idx = 0; Idx < (BcU32)RsShaderType::MAX; ++Idx )
			{
				if( pProgramHeader->ShaderHashes_[ Idx ] != 0 )
				{
					auto& ShaderMapping( ShaderMappings_[ Idx ] );
					RsShader* pShader = getShader( pProgramHeader->ShaderHashes_[ Idx ], ShaderMapping.Shaders_ );
					BcAssertMsg( pShader != nullptr, "Shader for permutation %x is invalid in ScnShader %s\n", pProgramHeader->ProgramPermutationFlags_, (*getName()).c_str() );
					Shaders.push_back( pShader );
				}
			}

			// Shrink to fit.
			Shaders.shrink_to_fit();

#if PSY_DEBUG
			if( Shaders.size() == 0)
			{
				PSY_LOG( "Unable to find shaders:" );
				for( BcU32 Idx = 0; Idx < (BcU32)RsShaderType::MAX; ++Idx )
				{
					PSY_LOG( " - Shader %u: %x", Idx, pProgramHeader->ShaderHashes_[ Idx ] != 0 );
				}
			}
#endif

			RsProgramVertexAttribute* VertexAttributes_ = (RsProgramVertexAttribute*)( pProgramHeader + 1 );
			RsProgramVertexAttributeList VertexAttributes;
			VertexAttributes.reserve( pProgramHeader->NoofVertexAttributes_ );
			for( BcU32 Idx = 0; Idx < pProgramHeader->NoofVertexAttributes_; ++Idx )
			{
				VertexAttributes.push_back( VertexAttributes_[ Idx ] );
			}

			RsProgramUniform* Uniforms_ = (RsProgramUniform*)( VertexAttributes_ + pProgramHeader->NoofVertexAttributes_ );
			RsProgramUniformList Uniforms;
			Uniforms.reserve( pProgramHeader->NoofUniforms_ );
			for( BcU32 Idx = 0; Idx < pProgramHeader->NoofUniforms_; ++Idx )
			{
				Uniforms.push_back( Uniforms_[ Idx ] );
			}

			RsProgramUniformBlock* UniformBlocks_ = (RsProgramUniformBlock*)( Uniforms_ + pProgramHeader->NoofUniforms_ );
			RsProgramUniformBlockList UniformBlocks;
			UniformBlocks.reserve( pProgramHeader->NoofUniformBlocks_ );
			for( BcU32 Idx = 0; Idx < pProgramHeader->NoofUniformBlocks_; ++Idx )
			{
				UniformBlocks.push_back( UniformBlocks_[ Idx ] );
			}
		
			// Create program.
			PSY_LOG( "Creating program: %x", pProgramHeader->ProgramPermutationFlags_ );
			RsProgram* pProgram = RsCore::pImpl()->createProgram( 
				std::move( Shaders ), 
				std::move( VertexAttributes ),
				std::move( Uniforms ),
				std::move( UniformBlocks ),
				*getName() );			
			ProgramMap_[ pProgramHeader->ProgramPermutationFlags_ ] = pProgram;
		}
	}

	// Mark ready if we've got all the programs we expect.
	if( TotalProgramsLoaded_ == pHeader_->NoofProgramPermutations_ )
	{
		markCreate();

		if( ProgramMap_.size() == 0 )
		{
			PSY_LOG( "WARNING: No programs created for $(ScnShader:%s.%s)",
				(*getPackage()->getName()).c_str(),
				(*getName()).c_str() );
		}
	}
}
