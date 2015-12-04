#pragma once

#include "System/Renderer/VK/RsVK.h"

//////////////////////////////////////////////////////////////////////////
// RsFrameBufferVK
class RsFrameBufferVK
{
public:
	RsFrameBufferVK( class RsFrameBuffer* Parent, VkDevice Device );
	~RsFrameBufferVK();

	VkFramebuffer getFrameBuffer() const { return FrameBuffer_; }
	VkRenderPass getRenderPass() const { return RenderPass_; }

private:
	void createFrameBuffer();
	void createRenderPass();

private:
	class RsFrameBuffer* Parent_ = nullptr;
	VkDevice Device_ = 0;
	VkRenderPass RenderPass_ = 0;
	VkFramebuffer FrameBuffer_ = 0;
};
