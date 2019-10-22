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
		FilterMode  filter_mode = FilterMode::Linear;
		AttachmentInfo() = default;
		AttachmentInfo(
			LoadOp  load_op_,
			StoreOp store_op_,
			ColorFormat internal_format_,
			FilterMode  filter_mode_
		) :
			load_op{ load_op_ },
			store_op{ store_op_ },
			internal_format{ internal_format_ },
			filter_mode{ filter_mode_ }
		{};
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
		void SetDepthAttachment(AttachmentInfo att_info)
		{
			info.depth_attachment = (att_info);

		}
		void ClearDepthAttachment()
		{
			info.depth_attachment = {};
		}
		FrameBufferInfo End()
		{
			FrameBufferInfo tmp = std::move(info);
			info.attachments.clear();
			info.depth_attachment = {};
			info.size = ivec2{};
			return std::move(tmp);
		}
	private:
		FrameBufferInfo info;
	};

	struct FrameBufferFactory : ResourceFactory<FrameBuffer>
	{
		RscHandle<FrameBuffer> Create(const FrameBufferInfo& info)
		{
			auto handle = Core::GetResourceManager().Create<FrameBuffer>();
			Update(info, handle);
			return handle;
		}
		void Update(const FrameBufferInfo& info, RscHandle<FrameBuffer> h_fb)
		{
			auto& fb = *h_fb;
			Reset(fb);
			fb.size = info.size;
			for (auto& attachment_info : info.attachments)
			{
				auto& attachment_ptr = fb.attachments.emplace_back();
				CreateAttachment(AttachmentType::eColor,attachment_info, fb.size , attachment_ptr);
			}
			if (info.depth_attachment)
				CreateAttachment(AttachmentType::eDepth, *info.depth_attachment, fb.size, h_fb->depth_attachment);
			if (info.stencil_attachment)
				CreateAttachment(AttachmentType::eStencil, *info.stencil_attachment, fb.size, h_fb->stencil_attachment);
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