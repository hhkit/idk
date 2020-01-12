#include "stdafx.h"
#include "FramebufferFactory.h"
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>

namespace idk
{
	RscHandle<FrameBuffer> FrameBufferFactory::Create(const FrameBufferInfo& info, SpecializedInfo* specialized_info)
	{
		auto handle = Core::GetResourceManager().Create<FrameBuffer>();
		Update(info, handle,specialized_info);
		return handle;
	}
	void FrameBufferFactory::Update(const FrameBufferInfo& info, RscHandle<FrameBuffer> h_fb, SpecializedInfo* specialized_info)
	{
		auto& fb = *h_fb;
		Reset(fb);
		fb.size = info.size;
		fb.num_layers = info.num_layers;
		for (auto& attachment_info : info.attachments)
		{
			auto& attachment_ptr = fb.attachments.emplace_back();
			CreateAttachment(AttachmentType::eColor, attachment_info, fb.size, attachment_ptr);
		}
		if (info.depth_attachment)
		{
			if(info.depth_attachment->isCubeMap)
				CreateAttachment(AttachmentType::eDepth3D, *info.depth_attachment, fb.size, h_fb->depth_attachment);
			else
				CreateAttachment(AttachmentType::eDepth, *info.depth_attachment, fb.size, h_fb->depth_attachment);
		}
		if (info.stencil_attachment)
			CreateAttachment(AttachmentType::eStencil, *info.stencil_attachment, fb.size, h_fb->stencil_attachment);
		Finalize(fb,specialized_info);
	}

	//resets the framebuffer (queue resource for destruction)

	void FrameBufferFactory::Reset(FrameBuffer& framebuffer)
	{
		PreReset(framebuffer);
		framebuffer.attachments.clear();
		framebuffer.size = ivec2{};
	}

	FrameBufferBuilder& FrameBufferBuilder::Begin(ivec2 size, size_t num_layers)
	{
		info.size = size;
		info.num_layers = num_layers;
		info.attachments.clear();//Clear to be sure.
		return *this;
	}

	FrameBufferBuilder& FrameBufferBuilder::AddAttachment(AttachmentInfo att_info)
	{
		info.attachments.emplace_back(att_info);
		return *this;
	}

	FrameBufferBuilder& FrameBufferBuilder::SetDepthAttachment(AttachmentInfo att_info)
	{
		info.depth_attachment = (att_info);
		return *this;
	}

	FrameBufferBuilder& FrameBufferBuilder::ClearDepthAttachment()
	{
		info.depth_attachment = {};
		return *this;
	}

	FrameBufferInfo FrameBufferBuilder::End()
	{
		FrameBufferInfo tmp{};
		std::swap(info, tmp);
		return std::move(tmp);
	}

}
