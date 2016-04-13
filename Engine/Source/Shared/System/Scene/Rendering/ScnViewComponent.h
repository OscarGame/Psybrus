/**************************************************************************
*
* File:		Rendering/ScnViewComponent.h
* Author:	Neil Richardson 
* Ver/Date:	26/11/11	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __ScnViewComponent_H__
#define __ScnViewComponent_H__

#include "System/Renderer/RsCore.h"
#include "System/Renderer/RsBuffer.h"
#include "System/Renderer/RsRenderNode.h"
#include "System/Scene/ScnComponent.h"
#include "System/Scene/ScnComponentProcessor.h"
#include "System/Scene/ScnCoreCallback.h"
#include "System/Scene/Rendering/ScnTexture.h"
#include "System/Scene/Rendering/ScnShaderFileData.h"
#include "System/Scene/Rendering/ScnViewRenderInterface.h"

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////
// ScnViewProcessor
class ScnViewProcessor : 
	public BcGlobal< ScnViewProcessor >,
	public ScnComponentProcessor,
	public ScnCoreCallback
{
public:
	ScnViewProcessor();
	virtual ~ScnViewProcessor();

	/**
	 * Register render interface.
	 */
	void registerRenderInterface( const ReClass* Class, ScnViewRenderInterface* Interface );

	/**
	 * Deregister render interface.
	 */
	void deregisterRenderInterface( const ReClass* Class, ScnViewRenderInterface* Interface );

	/**
	 * Reset view render data.
	 */
	void resetViewRenderData( class ScnComponent* Component );

	/**
	 * Will render everything visible to all views.
	 */
	void renderViews( const ScnComponentList& Components );	

protected:
	void initialise() override;
	void shutdown() override;

	void onAttach( ScnComponent* Component ) override;
	void onDetach( ScnComponent* Component ) override;

	void onAttachComponent( ScnComponent* Component ) override;
	void onDetachComponent( ScnComponent* Component ) override;

private:
	ScnViewRenderInterface* getRenderInterface( const ReClass* Class );

	std::unique_ptr< class ScnViewVisibilityTree > SpatialTree_;
	std::vector< ScnViewVisibilityLeaf* > GatheredVisibleLeaves_;
	std::vector< ScnViewComponentRenderData > ViewComponentRenderDatas_;

	std::set< ScnComponent* > PendingViewDataReset_;

	struct ProcessingGroup
	{
		class ScnViewRenderInterface* RenderInterface_ = nullptr;
		BcU32 Base_ = 0;
		BcU32 Noof_ = 0;
	};

	std::vector< ProcessingGroup > ProcessingGroups_;

	std::unordered_map< const ReClass*, ScnViewRenderInterface* > RenderInterfaces_;
	std::unordered_map< ScnComponent*, ScnViewVisibilityLeaf* > VisibilityLeaves_;

	struct ViewData
	{
		/// View.
		class ScnViewComponent* View_ = nullptr;
		std::unordered_map< ScnComponent*, ScnViewRenderData* > ViewRenderData_;

#if 0
		/// Data to be stored per component.
		struct ComponentData
		{
			class ScnComponent* Component_;
			class ScnViewRenderData* ViewRenderData_;
			MaAABB CullingAABB_;
		};

		/// Data to be stored per class.
		struct ClassData
		{
			const ReClass* Class_;
			std::vector< ComponentData > ComponentData_;
		};

		std::vector< ClassData > ClassData_;
#endif
	};

	std::vector< std::unique_ptr< ViewData > > ViewData_;
	std::vector< ScnComponent* > RenderableComponents_;
};

//////////////////////////////////////////////////////////////////////////
// ScnViewComponentRef
typedef ReObjectRef< class ScnViewComponent > ScnViewComponentRef;
typedef std::list< ScnViewComponentRef > ScnViewComponentList;
typedef ScnViewComponentList::iterator ScnViewComponentListIterator;
typedef ScnViewComponentList::const_iterator ScnViewComponentListConstIterator;
typedef std::map< std::string, ScnViewComponentRef > ScnViewComponentMap;
typedef ScnViewComponentMap::iterator ScnViewComponentMapIterator;
typedef ScnViewComponentMap::const_iterator ScnViewComponentMapConstIterator;

//////////////////////////////////////////////////////////////////////////
// ScnViewComponent
class ScnViewComponent:
	public ScnComponent
{
public:
	REFLECTION_DECLARE_DERIVED( ScnViewComponent, ScnComponent );

	ScnViewComponent();
	virtual ~ScnViewComponent();
	
	void onAttach( ScnEntityWeakRef Parent ) override;
	void onDetach( ScnEntityWeakRef Parent ) override;

	/**
	 * Get view uniform buffer.
	 */
	class RsBuffer* getViewUniformBuffer(); 

	void setMaterialParameters( class ScnMaterialComponent* MaterialComponent ) const;
	void getWorldPosition( const MaVec2d& ScreenPosition, MaVec3d& Near, MaVec3d& Far ) const;
	MaVec2d getScreenPosition( const MaVec3d& WorldPosition ) const;
	BcU32 getDepth( const MaVec3d& WorldPos ) const;

	BcBool intersect( const MaAABB& AABB ) const;
	BcBool hasRenderTarget() const;

	RsFrameBuffer* getFrameBuffer() const;
	const RsViewport& getViewport() const;
	bool compare( const ScnViewComponent* Other ) const;

	/**
	 * Determine the sort pass type for a given set of @a SortPassFlags & @a PermutationFlags.
	 */
	RsRenderSortPassType getSortPassType( RsRenderSortPassFlags SortPassFlags, ScnShaderPermutationFlags PermutationFlags ) const;

virtual void bind( class RsFrame* pFrame, RsRenderSort Sort );
	
	void setRenderMask( BcU32 RenderMask ) { RenderMask_ = RenderMask; }
	const BcU32 getRenderMask() const { return RenderMask_; }
	const ScnShaderPermutationFlags getRenderPermutation() const { return RenderPermutation_ & ScnShaderPermutationFlags::RENDER_ALL; }
	const RsRenderSortPassFlags getPasses() const { return Passes_; }

private:
	void recreateFrameBuffer();

private:
	// Viewport. Values relative to the size of the client being rendered into.
	BcF32 X_;
	BcF32 Y_;
	BcF32 Width_;
	BcF32 Height_;

	// Perspective projection.
	BcF32 Near_;
	BcF32 Far_;
	BcF32 HorizontalFOV_;		// Used by default.
	BcF32 VerticalFOV_;		// Used if HorizontalFOV_ is 0.0.

	RsColour ClearColour_;
	bool EnableClearColour_;
	bool EnableClearDepth_;
	bool EnableClearStencil_;	
	
	BcU32 RenderMask_;		// Used to determine what objects should be rendered for this view.
	
	// Permutation types.
	ScnShaderPermutationFlags RenderPermutation_;
	RsRenderSortPassFlags Passes_;

	// TODO: Remove this dependency, not really needed.
	RsViewport Viewport_;

	// Uniform block data.
	ScnShaderViewUniformBlockData ViewUniformBlock_;
	RsBufferUPtr ViewUniformBuffer_;

	// Used for culling.
	// TODO: Move into BcFrustum, or perhaps a BcConvexHull?
	MaPlane FrustumPlanes_[ 6 ];

	// Frame buffer + render target.
	std::vector< ScnTextureRef > RenderTarget_;
	ScnTextureRef DepthStencilTarget_;
	RsFrameBufferUPtr FrameBuffer_;
};

#endif
