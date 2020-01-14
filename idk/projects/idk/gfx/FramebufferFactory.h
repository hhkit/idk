#pragma once
#include <stdafx.h>
#include <gfx/Framebuffer.h>
#include <gfx/Texture.h>
#include <gfx/TextureInternalFormat.h>
#include <res/ResourceFactory.h>

namespace idk
{
	//using AttachmentInfo = int;
	struct AttachmentInfo
	{
		LoadOp  load_op = LoadOp::eClear;
		StoreOp store_op = StoreOp::eStore;

		TextureInternalFormat internal_format = TextureInternalFormat::RGBA_32_F;
		FilterMode  filter_mode = FilterMode::Linear;
		std::optional<RscHandle<Texture>> buffer;
		bool isCubeMap = false;
		bool override_as_depth = false;
		bool is_input_att = false;

		AttachmentInfo() = default;

		AttachmentInfo(
			LoadOp  load_op_,
			StoreOp store_op_,
			TextureInternalFormat int_format,
			FilterMode  filter_mode_,
			bool isCubeMap_ = false,
			std::optional<RscHandle<Texture>> buffer_ = std::nullopt
		) :
			load_op{ load_op_ },
			store_op{ store_op_ },
			internal_format{ int_format },
			filter_mode{ filter_mode_ },
			isCubeMap{ isCubeMap_ },
			buffer{ buffer_ }
		{};

		AttachmentInfo(
			LoadOp  load_op_,
			StoreOp store_op_,
			ColorFormat color_format,
			FilterMode  filter_mode_,
			bool isCubeMap_ = false,
			std::optional<RscHandle<Texture>> buffer_= std::nullopt
		) :
			load_op{ load_op_ },
			store_op{ store_op_ },
			internal_format{ ToInternalFormat(color_format, false) },
			filter_mode{ filter_mode_ },
			isCubeMap{ isCubeMap_},
			buffer{ buffer_ }
		{};

		AttachmentInfo(
			LoadOp  load_op_,
			StoreOp store_op_,
			DepthBufferMode depth_format,
			bool enable_stencil,
			FilterMode  filter_mode_,
			bool isCubeMap_ = false,
			std::optional<RscHandle<Texture>> buffer_ = std::nullopt
		) :
			load_op{ load_op_ },
			store_op{ store_op_ },
			internal_format{ ToInternalFormat(depth_format, enable_stencil) },
			filter_mode{ filter_mode_ },
			isCubeMap{ isCubeMap_ },
			buffer{ buffer_ }
		{};

		AttachmentInfo(const Attachment& attachment)
			: load_op{ attachment.load_op }
			, store_op{ attachment.store_op }
			, internal_format{ attachment.buffer->InternalFormat()}
			, filter_mode{ attachment.buffer->Filter() }
			, buffer{attachment.buffer}
		{
		}
	};

	struct SpecializedInfo {};

	struct FrameBufferInfo
	{
		vector<AttachmentInfo> attachments;
		std::optional<AttachmentInfo> depth_attachment, stencil_attachment;
		ivec2 size{};
		size_t num_layers{};
		string name;
	};

	class FrameBufferBuilder
	{
	public:
		FrameBufferBuilder& Begin(const string& name, ivec2 size, size_t num_layers = 1);
		FrameBufferBuilder& AddAttachment(AttachmentInfo att_info);
		FrameBufferBuilder& SetDepthAttachment(AttachmentInfo att_info);
		FrameBufferBuilder& ClearDepthAttachment();
		FrameBufferInfo End();
	private:
		FrameBufferInfo info;
	};

	class FrameBufferFactory 
		: public ResourceFactory<FrameBuffer>
	{
	public:
		RscHandle<FrameBuffer> Create(const FrameBufferInfo& info, SpecializedInfo* specialized_info = nullptr);
		void Update(const FrameBufferInfo& info, RscHandle<FrameBuffer> h_fb, SpecializedInfo* specialized_info);
	protected:
		//out must be assigned a make unique of the implementation version of attachment
		virtual void CreateAttachment(AttachmentType type,const AttachmentInfo& info, ivec2 size, unique_ptr<Attachment>& out) = 0;
		virtual void PreReset(FrameBuffer& framebuffer) = 0;//resets the framebuffer (queue resource for destruction)
		void Reset(FrameBuffer& framebuffer);
		virtual void Finalize(FrameBuffer& h_fb, SpecializedInfo* specialized_info) = 0;
	};
}