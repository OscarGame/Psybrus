/**************************************************************************
*
* File:		ScnMaterial.cpp
* Author:	Neil Richardson 
* Ver/Date:	5/03/11	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Scene/Rendering/ScnMaterial.h"
#include "System/Scene/ScnEntity.h"
#include "System/Content/CsCore.h"
#include "System/Os/OsCore.h"

#include "System/Renderer/RsRenderNode.h"

#ifdef PSY_IMPORT_PIPELINE
#include "System/Scene/Import/ScnMaterialImport.h"
#endif

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED( ScnMaterial );

void ScnMaterial::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "pHeader_", &ScnMaterial::pHeader_, bcRFF_SHALLOW_COPY | bcRFF_CHUNK_DATA ),
		new ReField( "Shader_", &ScnMaterial::Shader_, bcRFF_SHALLOW_COPY ),
		new ReField( "TextureMap_", &ScnMaterial::TextureMap_ ),
		new ReField( "RenderStateDesc_", &ScnMaterial::RenderStateDesc_, bcRFF_SHALLOW_COPY | bcRFF_CHUNK_DATA ),
	};
		
	auto& Class = ReRegisterClass< ScnMaterial, Super >( Fields );
	BcUnusedVar( Class );

#ifdef PSY_IMPORT_PIPELINE
	// Add importer attribute to class for resource system to use.
	Class.addAttribute( new CsResourceImporterAttribute( 
		ScnMaterialImport::StaticGetClass(), 0 ) );
#endif
}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnMaterial::ScnMaterial():
	pHeader_( nullptr ),
	RenderStateDesc_( nullptr )
{

}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
ScnMaterial::~ScnMaterial()
{

}

//////////////////////////////////////////////////////////////////////////
// create
//virtual
void ScnMaterial::create()
{
	ScnMaterialTextureHeader* pTextureHeaders = (ScnMaterialTextureHeader*)( pHeader_ + 1 );
		
	// Get resources.
	Shader_ = getPackage()->getCrossRefResource( pHeader_->ShaderRef_ );
	for( BcU32 Idx = 0; Idx < pHeader_->NoofTextures_; ++Idx )
	{
		ScnMaterialTextureHeader* pTextureHeader = &pTextureHeaders[ Idx ];
		TextureMap_[ pTextureHeader->SamplerName_ ] = getPackage()->getCrossRefResource( pTextureHeader->TextureRef_ );
	}
	
	// Create render state.
	RenderState_ = RsCore::pImpl()->createRenderState( *RenderStateDesc_, getFullName().c_str() );

	// Create sampler states.
	for( BcU32 Idx = 0; Idx < pHeader_->NoofTextures_; ++Idx )
	{
		ScnMaterialTextureHeader* pTextureHeader = &pTextureHeaders[ Idx ];
		auto SamplerState = RsCore::pImpl()->createSamplerState( pTextureHeader->SamplerStateDesc_, getFullName().c_str() );
		SamplerStateMap_[ pTextureHeader->SamplerName_ ] = std::move( SamplerState );
	}

	// Mark as ready.
	markReady();
}

//////////////////////////////////////////////////////////////////////////
// destroy
//virtual
void ScnMaterial::destroy()
{
	RenderState_.reset();
	SamplerStateMap_.clear();
}

//////////////////////////////////////////////////////////////////////////
// getTexture
ScnTextureRef ScnMaterial::getTexture( BcName Name )
{
	return TextureMap_[ Name ];
}

//////////////////////////////////////////////////////////////////////////
// getProgram
RsProgram* ScnMaterial::getProgram( ScnShaderPermutationFlags PermutationFlags )
{
	return Shader_->getProgram( PermutationFlags );
}

//////////////////////////////////////////////////////////////////////////
// getProgramBinding
RsProgramBindingDesc ScnMaterial::getProgramBinding( ScnShaderPermutationFlags PermutationFlags )
{
	auto* Program = Shader_->getProgram( PermutationFlags );
	RsProgramBindingDesc ProgramBindingDesc;

	for( auto Iter( TextureMap_.begin() ); Iter != TextureMap_.end(); ++Iter )
	{
		const BcName& SamplerName = (*Iter).first;
		ScnTextureRef Texture = (*Iter).second;

		BcU32 Slot = Program->findShaderResourceSlot( (*SamplerName).c_str() );
		if( Slot != BcErrorCode )
		{
			ProgramBindingDesc.setShaderResourceView( Slot, Texture->getTexture() );
		}
	}

	for( auto Iter( SamplerStateMap_.begin() ); Iter != SamplerStateMap_.end(); ++Iter )
	{
		const BcName& SamplerName = (*Iter).first;
		RsSamplerState* Sampler = (*Iter).second.get();

		BcU32 Slot = Program->findSamplerSlot( (*SamplerName).c_str() );
		if( Slot != BcErrorCode )
		{
			ProgramBindingDesc.setSamplerState( Slot, Sampler );
		}
	}

	return ProgramBindingDesc;
}

//////////////////////////////////////////////////////////////////////////
// getRenderState
RsRenderState* ScnMaterial::getRenderState()
{
	return RenderState_.get();
}

//////////////////////////////////////////////////////////////////////////
// fileReady
void ScnMaterial::fileReady()
{
	// File is ready, get the header chunk.
	requestChunk( 0 );
}

//////////////////////////////////////////////////////////////////////////
// fileChunkReady
void ScnMaterial::fileChunkReady( BcU32 ChunkIdx, BcU32 ChunkID, void* pData )
{
	// If we have no render core get chunk 0 so we keep getting entered into.
	if( RsCore::pImpl() == NULL )
	{
		requestChunk( 0 );
		return;
	}
	
	if( ChunkID == BcHash( "header" ) )
	{
		pHeader_ = (ScnMaterialHeader*)pData;
		ScnMaterialTextureHeader* pTextureHeaders = (ScnMaterialTextureHeader*)( pHeader_ + 1 );

		// Markup names now.
		// TODO: Automate this process with reflection!
		for( BcU32 Idx = 0; Idx < pHeader_->NoofTextures_; ++Idx )
		{
			ScnMaterialTextureHeader* pTextureHeader = &pTextureHeaders[ Idx ];
			markupName( pTextureHeader->SamplerName_ );
		}

		requestChunk( ++ChunkIdx );
	}
	else if( ChunkID == BcHash( "renderstate" ) )
	{
		RenderStateDesc_ = (const RsRenderStateDesc*)pData;

		markCreate(); // All data loaded, time to create.
	}
}

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED( ScnMaterialComponent );

void ScnMaterialComponent::StaticRegisterClass()
{
	{
		ReField* Fields[] = 
		{
			new ReField( "Material_", &ScnMaterialComponent::Material_, bcRFF_SHALLOW_COPY | bcRFF_IMPORTER ),
			new ReField( "PermutationFlags_", &ScnMaterialComponent::PermutationFlags_, bcRFF_IMPORTER ),

			new ReField( "pProgram_", &ScnMaterialComponent::pProgram_, bcRFF_SHALLOW_COPY ),
			new ReField( "ViewUniformBlockIndex_", &ScnMaterialComponent::ViewUniformBlockIndex_, bcRFF_CONST ),
			new ReField( "BoneUniformBlockIndex_", &ScnMaterialComponent::BoneUniformBlockIndex_, bcRFF_CONST ),
			new ReField( "ObjectUniformBlockIndex_", &ScnMaterialComponent::ObjectUniformBlockIndex_, bcRFF_CONST ),
		};
		ReRegisterClass< ScnMaterialComponent, Super >( Fields );
	}

	{
		ReField* Fields[] = 
		{
			new ReField( "Material_", &TTextureBinding::Handle_ ),
			new ReField( "pProgram_", &TTextureBinding::Texture_, bcRFF_SHALLOW_COPY ),
		};
		ReRegisterClass< TTextureBinding >( Fields );
	}

	{
		ReField* Fields[] = 
		{
			new ReField( "Index_", &TUniformBlockBinding::Index_ ),
			new ReField( "UniformBuffer_", &TUniformBlockBinding::UniformBuffer_, bcRFF_SHALLOW_COPY ),
		};
		ReRegisterClass< TUniformBlockBinding >( Fields );
	}
}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnMaterialComponent::ScnMaterialComponent():
	ScnMaterialComponent( nullptr, ScnShaderPermutationFlags::NONE )
{

}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnMaterialComponent::ScnMaterialComponent( ScnMaterialRef Material, ScnShaderPermutationFlags PermutationFlags ):
	Material_( Material ),
	PermutationFlags_( PermutationFlags ),
	pProgram_( nullptr ),
	ViewUniformBlockIndex_(),
	BoneUniformBlockIndex_(),
	ObjectUniformBlockIndex_()
{
}

//////////////////////////////////////////////////////////////////////////
// Ctor
ScnMaterialComponent::ScnMaterialComponent( ScnMaterialComponent* Parent ):
	Material_( Parent->Material_ ),
	PermutationFlags_( Parent->PermutationFlags_ ),
	pProgram_( Parent->pProgram_ ),
	ViewUniformBlockIndex_( Parent->ViewUniformBlockIndex_ ),
	BoneUniformBlockIndex_( Parent->BoneUniformBlockIndex_ ),
	ObjectUniformBlockIndex_( Parent->ObjectUniformBlockIndex_ )
{
}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
ScnMaterialComponent::~ScnMaterialComponent()
{

}

//////////////////////////////////////////////////////////////////////////
// initialise
void ScnMaterialComponent::initialise()
{
	PermutationFlags_ = PermutationFlags_ | ScnShaderPermutationFlags::RENDER_FORWARD | ScnShaderPermutationFlags::PASS_MAIN;
	if( Material_ && pProgram_ == nullptr )
	{
		BcAssertMsg( Material_->isReady(), 
			"Material is not ready for use. Possible cause is trying to use a material with a component from within the same package. "
			"Known issue, can be worked around by moving the material $(ScnMaterial:%s.%s) into another package to allow "
			"it to be fully loaded. This will hopefully be fixed by #118.  "
			"Another possible cause is due to not using the bcRFF_SHALLOW_COPY flag on an imported member field. ",
			(*Material_->getPackageName()).c_str(), (*Material_->getName()).c_str() );
		pProgram_ = Material_->Shader_->getProgram( PermutationFlags_ );
		BcAssert( pProgram_ != nullptr );
		
		// Build a binding list for textures.
		auto& TextureMap( Material_->TextureMap_ );
		for( auto Iter( TextureMap.begin() ); Iter != TextureMap.end(); ++Iter )
		{
			const BcName& SamplerName = (*Iter).first;
			ScnTextureRef Texture = (*Iter).second;

			BcU32 SamplerIdx = findTextureSlot( SamplerName );
			if( SamplerIdx != BcErrorCode )
			{
				setTexture( SamplerIdx, Texture );
			}
		}

		// Build a binding list for samplers.
		auto& SamplerMap( Material_->SamplerStateMap_ );
		for( auto Iter( SamplerMap.begin() ); Iter != SamplerMap.end(); ++Iter )
		{
			const BcName& SamplerName = (*Iter).first;
			RsSamplerState* Sampler = (*Iter).second.get();

			BcU32 SamplerIdx = findTextureSlot( SamplerName );
			if( SamplerIdx != BcErrorCode )
			{
				setSamplerState( SamplerIdx, Sampler );
			}
		}

		// Grab uniform blocks.
		ViewUniformBlockIndex_ = findUniformBlock( "ScnShaderViewUniformBlockData" );
		BoneUniformBlockIndex_ = findUniformBlock( "ScnShaderBoneUniformBlockData" );
		ObjectUniformBlockIndex_ = findUniformBlock( "ScnShaderObjectUniformBlockData" );
	}
}

//////////////////////////////////////////////////////////////////////////
// destroy
void ScnMaterialComponent::destroy()
{
	Material_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// findTextureSlot
BcU32 ScnMaterialComponent::findTextureSlot( const BcName& TextureName )
{
	return pProgram_->findShaderResourceSlot( (*TextureName).c_str() );
}

//////////////////////////////////////////////////////////////////////////
// setTexture
void ScnMaterialComponent::setTexture( BcU32 Slot, ScnTextureRef Texture )
{
	// Find the texture slot to put this in.
	if( Slot != BcErrorCode )
	{
		if( ProgramBindingDesc_.setShaderResourceView( Slot, Texture ? Texture->getTexture() : nullptr ) )
		{
			TextureMap_[ Slot ] = Texture;
			ProgramBinding_.reset();
		}
	}
	else
	{
		PSY_LOG( "ERROR: Unable to set texture in \"%s\"\n", getFullName().c_str() );
	}
}

//////////////////////////////////////////////////////////////////////////
// setTexture
void ScnMaterialComponent::setTexture( const BcName& TextureName, ScnTextureRef Texture )
{
	setTexture( findTextureSlot( TextureName ), Texture );
}

//////////////////////////////////////////////////////////////////////////
// setSamplerState
void ScnMaterialComponent::setSamplerState( BcU32 Slot, RsSamplerState* Sampler )
{
	if( Slot != BcErrorCode )
	{
		if( ProgramBindingDesc_.setSamplerState( Slot, Sampler ) )
		{
			ProgramBinding_.reset();
		}
	}
	else
	{
		PSY_LOG( "ERROR: Unable to set sampler state in \"%s\"\n", getFullName().c_str() );
	}
}

//////////////////////////////////////////////////////////////////////////
// setSamplerState
void ScnMaterialComponent::setSamplerState( const BcName& TextureName, RsSamplerState* Sampler )
{
	setSamplerState( findTextureSlot( TextureName ), Sampler );
}

//////////////////////////////////////////////////////////////////////////
// findUniformBlock
BcU32 ScnMaterialComponent::findUniformBlock( const BcName& UniformBlockName )
{
	return pProgram_->findUniformBufferSlot( (*UniformBlockName).c_str() );
}

//////////////////////////////////////////////////////////////////////////
// setUniformBlock
void ScnMaterialComponent::setUniformBlock( BcU32 Slot, RsBuffer* UniformBuffer )
{
	if( Slot != BcErrorCode )
	{
		if( ProgramBindingDesc_.setUniformBuffer( Slot, UniformBuffer ) )
		{
			ProgramBinding_.reset();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// setUniformBlock
void ScnMaterialComponent::setUniformBlock( const BcName& UniformBlockName, RsBuffer* UniformBuffer )
{
	setUniformBlock( findUniformBlock( UniformBlockName ), UniformBuffer );
}

//////////////////////////////////////////////////////////////////////////
// setViewUniformBlock
void ScnMaterialComponent::setViewUniformBlock( RsBuffer* UniformBuffer )
{
	setUniformBlock( ViewUniformBlockIndex_, UniformBuffer );
}

//////////////////////////////////////////////////////////////////////////
// setBoneUniformBlock
void ScnMaterialComponent::setBoneUniformBlock( RsBuffer* UniformBuffer )
{
	setUniformBlock( BoneUniformBlockIndex_, UniformBuffer );
}

//////////////////////////////////////////////////////////////////////////
// setObjectUniformBlock
void ScnMaterialComponent::setObjectUniformBlock( RsBuffer* UniformBuffer )
{
	setUniformBlock( ObjectUniformBlockIndex_, UniformBuffer );
}

//////////////////////////////////////////////////////////////////////////
// getProgramBinding
RsProgramBinding* ScnMaterialComponent::getProgramBinding()
{
	if( ProgramBinding_ == nullptr )
	{
		OsCore::pImpl()->unsubscribeAll( this );

		// If the texture is client dependent, resubscribe for the binding to be recreated.
		for( auto TextureEntry : TextureMap_ )
		{
			if( TextureEntry.second && TextureEntry.second->isClientDependent() )
			{
				OsCore::pImpl()->subscribe( osEVT_CLIENT_RESIZE, this,
					[ this ]( EvtID, const EvtBaseEvent& )->eEvtReturn
					{
						ProgramBinding_.reset();
						return evtRET_PASS;
					} );
				break;
			}
		}

		ProgramBinding_ = RsCore::pImpl()->createProgramBinding( pProgram_, ProgramBindingDesc_, getFullName().c_str() );
	}
	return ProgramBinding_.get();
}

//////////////////////////////////////////////////////////////////////////
// getRenderState
RsRenderState* ScnMaterialComponent::getRenderState()
{
	return Material_->RenderState_.get();
}

//////////////////////////////////////////////////////////////////////////
// getTexture
ScnTextureRef ScnMaterialComponent::getTexture( BcU32 Slot )
{
	if( TextureMap_.find( Slot ) != TextureMap_.end() )
	{
		return TextureMap_[ Slot ];
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// getTexture
ScnTextureRef ScnMaterialComponent::getTexture( const BcName& TextureName )
{
	return getTexture( findTextureSlot( TextureName ) );
}

//////////////////////////////////////////////////////////////////////////
// getMaterial
ScnMaterialRef ScnMaterialComponent::getMaterial()
{
	return Material_;
}

//////////////////////////////////////////////////////////////////////////
// bind
struct ScnMaterialComponentRenderData
{
	// Texture binding block.
	BcU32 NoofTextures_;
	BcU32* TextureHandles_;
	RsTexture** ppTextures_;
	RsSamplerState** ppSamplerStates_;

	// Uniform blocks.
	BcU32 NoofUniformBlocks_;
	BcU32* pUniformBlockIndices_;
	RsBuffer** ppUniformBuffers_;

	// State.
	RsRenderState* RenderState_;

	// Program.
	RsProgram* pProgram_;
	
	// For debugging.
	ScnMaterialComponent* pMaterial_;
};

void ScnMaterialComponent::bind( RsFrame* pFrame, RsRenderSort& Sort, BcBool Stateless )
{
	BcAssertMsg( isAttached(), "Material \"%s\" needs to be attached to an entity!", (*getName()).c_str() );
}

//////////////////////////////////////////////////////////////////////////
// onAttach
//virtual
void ScnMaterialComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );
}

//////////////////////////////////////////////////////////////////////////
// onDetach
//virtual
void ScnMaterialComponent::onDetach( ScnEntityWeakRef Parent )
{
	ProgramBinding_.reset();

	Super::onDetach( Parent );
}
