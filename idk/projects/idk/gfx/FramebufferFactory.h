#pragma once
#include <stdafx.h>
#include <gfx/Framebuffer.h>
#include <gfx/Texture.h>
namespace idk
{
	//using AttachmentInfo = int;
	struct AttachmentInfo
	{
		LoadOp  load_op = LoadOp::eClear;
		StoreOp store_op = StoreOp::eStore;
		ColorFormat internal_format = ColorFormat::RGBAF_32;
		InputChannels format = InputChannels::RGBA;
		FilterMode  filter_mode = FilterMode::Linear;
	};
	struct FrameBufferInfo
	{
		vector<AttachmentInfo> attachments;
		std::optional<AttachmentInfo> depth_attachment, stencil_attachment;
		ivec2 size;
	};
	struct FrameBufferBuilder
	{
		void Begin(ivec2 size)
		{
			info.size = size;
			info.attachments.clear();//Clear to be sure.
		}
		void AddAttachment(AttachmentInfo att_info)
		{
			info.attachments.emplace_back(att_info);
		}
		FrameBufferInfo End()
		{
			FrameBufferInfo tmp = std::move(info);
			info.attachments.clear();
			info.size = ivec2{};
			return std::move(tmp);
		}
	private:
		FrameBufferInfo info;
	};

	struct FrameBufferFactory : ResourceFactory<FrameBuffer>
	{
		std::unique_ptr<FrameBuffer> Create() override
		{
			return std::make_unique<FrameBuffer>();
		}
		RscHandle<FrameBuffer> Create(const FrameBufferInfo& info)
		{
			auto handle = Core::GetResourceManager().LoaderEmplaceResource<FrameBuffer>();
			Update(info, handle);
			return handle;
		}
		void Update(const FrameBufferInfo& info, RscHandle<FrameBuffer> h_fb)
		{
			auto& fb = *h_fb;
			Reset(fb);
			for (auto& attachment_info : info.attachments)
			{
				auto& attachment_ptr = fb.attachments.emplace_back();
				CreateAttachment(AttachmentType::eColor,attachment_info, info.size, attachment_ptr);
			}
			if (info.depth_attachment)
				CreateAttachment(AttachmentType::eDepth, *info.depth_attachment, info.size, h_fb->depth_attachment);
			if (info.stencil_attachment)
				CreateAttachment(AttachmentType::eStencil, *info.stencil_attachment, info.size, h_fb->stencil_attachment);
			Finalize(fb);
		}
	protected:
		//out must be assigned a make unique of the implementation version of attachment
		virtual void CreateAttachment(AttachmentType type,const AttachmentInfo& info, ivec2 size, unique_ptr<Attachment>& out) = 0;
		virtual void PreReset(FrameBuffer& framebuffer) = 0;//resets the framebuffer (queue resource for destruction)
		void Reset(FrameBuffer& framebuffer)
		{
			PreReset(framebuffer);
			framebuffer.attachments.clear();
			framebuffer.size = ivec2{};
		}
		virtual void Finalize(FrameBuffer& h_fb) = 0;
	};
}