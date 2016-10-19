/**************************************************************************
*
* File:		RsContextNull.h
* Author: 	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __RSCONTEXTNULL_H__
#define __RSCONTEXTNULL_H__

#include "System/Renderer/RsContext.h"
#include "System/Renderer/Null/RsNull.h"

#include "Base/BcMisc.h"

//////////////////////////////////////////////////////////////////////////
// RsContextNull
class RsContextNull:
	public RsContext
{
public:
	RsContextNull( OsClient* pClient, RsContextNull* pParent );
	virtual ~RsContextNull();
	
	virtual OsClient* getClient() const override;
	virtual const RsFeatures& getFeatures() const override;

	virtual BcBool isShaderCodeTypeSupported( RsShaderCodeType CodeType ) const override;
	virtual RsShaderCodeType maxShaderCodeType( RsShaderCodeType CodeType ) const override;

	BcU32 getWidth() const override;
	BcU32 getHeight() const override;
	class RsFrameBuffer* getBackBuffer() const override;

	void resizeBackBuffer( BcU32 Width, BcU32 Height ) override;
	void beginFrame() override;
	void endFrame() override;
	void present() override;
	void takeScreenshot( RsScreenshotFunc ScreenshotFunc ) override;

	void clear( 
		const RsFrameBuffer* FrameBuffer,
		const RsColour& Colour,
		BcBool EnableClearColour,
		BcBool EnableClearDepth,
		BcBool EnableClearStencil ) override;
	void drawPrimitives( 
		const RsGeometryBinding* GeometryBinding, 
		const RsProgramBinding* ProgramBinding, 
		const RsRenderState* RenderState,
		const RsFrameBuffer* FrameBuffer, 
		const RsViewport* Viewport,
		const RsScissorRect* ScissorRect,
		RsTopologyType TopologyType, 
		BcU32 IndexOffset, BcU32 NoofIndices,
		BcU32 FirstInstance, BcU32 NoofInstances ) override;
	void drawIndexedPrimitives( 
		const RsGeometryBinding* GeometryBinding, 
		const RsProgramBinding* ProgramBinding, 
		const RsRenderState* RenderState,
		const RsFrameBuffer* FrameBuffer,
		const RsViewport* Viewport,
		const RsScissorRect* ScissorRect,
		RsTopologyType TopologyType, 
		BcU32 IndexOffset, BcU32 NoofIndices, BcU32 VertexOffset,
		BcU32 FirstInstance, BcU32 NoofInstances ) override;

	void copyTexture( RsTexture* SourceTexture, RsTexture* DestTexture ) override;
	void dispatchCompute( class RsProgramBinding* ProgramBinding, BcU32 XGroups, BcU32 YGroups, BcU32 ZGroups ) override;
	void beginQuery( class RsQueryHeap* QueryHeap, size_t Idx ) override;
	void endQuery( class RsQueryHeap* QueryHeap, size_t Idx ) override;
	bool isQueryResultAvailible( class RsQueryHeap* QueryHeap, size_t Idx ) override;
	void resolveQueries( class RsQueryHeap* QueryHeap, size_t Offset, size_t NoofQueries, BcU64* OutData ) override;

	bool createRenderState(
		RsRenderState* RenderState ) override;
	bool destroyRenderState(
		RsRenderState* RenderState ) override;
	bool createSamplerState(
		RsSamplerState* SamplerState ) override;
	bool destroySamplerState(
		RsSamplerState* SamplerState ) override;

	bool createFrameBuffer( 
		class RsFrameBuffer* FrameBuffer ) override;
	bool destroyFrameBuffer( 
		class RsFrameBuffer* FrameBuffer ) override;

	bool createBuffer( 
		RsBuffer* Buffer ) override;
	bool destroyBuffer( 
		RsBuffer* Buffer ) override;
	bool updateBuffer( 
		RsBuffer* Buffer,
		BcSize Offset,
		BcSize Size,
		RsResourceUpdateFlags Flags,
		RsBufferUpdateFunc UpdateFunc ) override;

	bool createTexture( 
		class RsTexture* Texture ) override;
	bool destroyTexture( 
		class RsTexture* Texture ) override;
	bool updateTexture( 
		class RsTexture* Texture,
		const struct RsTextureSlice& Slice,
		RsResourceUpdateFlags Flags,
		RsTextureUpdateFunc UpdateFunc ) override;

	bool createShader( class RsShader* Shader ) override;
	bool destroyShader( class RsShader* Shader ) override;

	bool createProgram( class RsProgram* Program ) override;
	bool destroyProgram( class RsProgram* Program ) override;
	bool createProgramBinding( class RsProgramBinding* ProgramBinding ) override;
	bool destroyProgramBinding( class RsProgramBinding* ProgramBinding ) override;
	bool createGeometryBinding( class RsGeometryBinding* GeometryBinding ) override;
	bool destroyGeometryBinding( class RsGeometryBinding* GeometryBinding ) override;
	bool createVertexDeclaration( class RsVertexDeclaration* VertexDeclaration ) override;
	bool destroyVertexDeclaration( class RsVertexDeclaration* VertexDeclaration  ) override;
	bool createQueryHeap( class RsQueryHeap* QueryHeap ) override;
	bool destroyQueryHeap( class RsQueryHeap* QueryHeap ) override;

	void flushState();


protected:
	void create() override;
	void update() override;
	void destroy() override;

private:
	RsContextNull* pParent_;
	OsClient* pClient_;
	BcU32 Width_;
	BcU32 Height_;
	BcThreadId OwningThread_;
	RsFeatures Features_;
};

#endif
