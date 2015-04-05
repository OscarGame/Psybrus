#include "System/Renderer/D3D12/RsResourceD3D12.h"
#include "System/Renderer/D3D12/RsUtilsD3D12.h"

#include "System/Renderer/RsBuffer.h"
#include "System/Renderer/RsTexture.h"

#include "Base/BcMath.h"

//////////////////////////////////////////////////////////////////////////
// Ctor
RsResourceD3D12::RsResourceD3D12( 
		ID3D12Resource* Resource, 
		RsResourceBindFlags BindFlags, 
		RsResourceBindFlags InitialBindType ):
	Resource_( Resource ),
	BindFlags_( BindFlags ),
	CurrentBindType_( InitialBindType )
{
	BcAssert( ( InitialBindType & BindFlags_ ) != RsResourceBindFlags::NONE );
	BcAssert( BcBitsSet( (BcU32)InitialBindType ) == 1 );
}

//////////////////////////////////////////////////////////////////////////
// Dtor
RsResourceD3D12::~RsResourceD3D12()
{
}

//////////////////////////////////////////////////////////////////////////
// getInternalResource
ComPtr< ID3D12Resource >& RsResourceD3D12::getInternalResource()
{
	return Resource_;
}
//////////////////////////////////////////////////////////////////////////
// getGPUVirtualAddress
D3D12_GPU_VIRTUAL_ADDRESS RsResourceD3D12::getGPUVirtualAddress()
{
	return Resource_->GetGPUVirtualAddress();
}

//////////////////////////////////////////////////////////////////////////
// resourceBarrierTransition
void RsResourceD3D12::resourceBarrierTransition( ID3D12GraphicsCommandList* CommandList, RsResourceBindFlags BindType )
{
	BcAssert( ( BindType & BindFlags_ ) != RsResourceBindFlags::NONE );
	BcAssert( BcBitsSet( (BcU32)BindType ) == 1 );

	if( CurrentBindType_ != BindType )
	{
		D3D12_RESOURCE_BARRIER_DESC descBarrier = {};
		descBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		descBarrier.Transition.pResource = Resource_.Get();
		descBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		descBarrier.Transition.StateBefore = RsUtilsD3D12::GetResourceUsage( CurrentBindType_ );
		descBarrier.Transition.StateAfter = RsUtilsD3D12::GetResourceUsage( BindType );

		CommandList->ResourceBarrier( 1, &descBarrier );
		CurrentBindType_ = BindType;
	}
}

