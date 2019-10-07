#pragma once
#include <stdafx.h>
namespace idk
{
	ENUM(ColorFormat, char,
		RG_8,
		RGF_16,
		RGB_8,
		RGBA_8,
		RGBF_16,
		RGBF_32,
		RGBAF_16,
		RGBAF_32,
		SRGB,
		DEPTH_COMPONENT,
		DXT1,
		DXT3,
		DXT5

	)

		ENUM(UVMode, char,
			Repeat,
			MirrorRepeat,
			Clamp,
			ClampToBorder
		);

	ENUM(FilterMode, char,
		Linear,
		Nearest
	);

	ENUM(InputChannels, char
		, RED
		, RG
		, RGB
		, RGBA
		, DEPTH_COMPONENT
	);
	enum AttachmentType
	{
		eColor,
		eDepth,
		eSizeAT
	};
	//using AttachmentInfo = int;
	struct AttachmentInfo
	{
		AttachmentType type;
		ColorFormat internal_format = ColorFormat::RGBF_32;
		InputChannels format = InputChannels::RGBA;
		FilterMode  filter_mode = FilterMode::Linear;
	};

	inline AttachmentType& operator++(AttachmentType& type)
	{
		type = s_cast<AttachmentType>(s_cast<int>(type) + 1);
		return type;
	}
	struct FrameBufferInfo
	{
		vector<AttachmentInfo> attachments;
		ivec2 size;
	};
	struct Attachment
	{
		virtual ~Attachment() = default;
	};

	struct FrameBuffer
	{
		vector<unique_ptr<Attachment>> attachments[AttachmentType::eSizeAT];
		FrameBuffer()
		{
		}
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

	struct FrameBufferLoader : ResourceFactory<FrameBuffer>
	{
		std::unique_ptr<FrameBuffer> Create() override
		{
			return std::make_unique<FrameBuffer>();
		}
		RscHandle<FrameBuffer> Create(const FrameBufferInfo& info) 
		{
			auto handle = Core::GetResourceManager().LoaderEmplaceResource<FrameBuffer>();
			Update(info,handle);
			return handle;
		}
		void Update(const FrameBufferInfo& info, RscHandle<FrameBuffer> h_fb)
		{
			auto& fb = *h_fb;
			for (auto& attachment_info : info.attachments)
			{
				auto& attachment_ptr = fb.attachments[attachment_info.type].emplace_back();
				CreateAttachment(attachment_info, info.size, attachment_ptr);
			}
			Finalize(fb);
		}
	protected:
		//out must be assigned a make unique of the implementation version of attachment
		virtual void CreateAttachment(const AttachmentInfo& info, ivec2 size, unique_ptr<Attachment>& out) = 0;
		virtual void Finalize(FrameBuffer& h_fb) =0;
	};
}