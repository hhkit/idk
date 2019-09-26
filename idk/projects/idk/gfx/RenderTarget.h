#pragma once
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <gfx/Texture.h>

namespace idk
{
	enum AttachmentType
	{
		eColor,
		eDepth,
		eSizeAT
	};
	inline AttachmentType& operator++(AttachmentType& type)
	{
		type = s_cast<AttachmentType>(s_cast<int>(type) + 1);
		return type;
	}
	struct RenderTargetMeta
	{
		ivec2 size {1024, 1024};
		vector<RscHandle<Texture>> textures;
		bool is_world_renderer = true;
		bool render_debug = true;
		//RscHandle<Texture> depth_buffer;
	};

	class RenderTarget
		: public Resource<RenderTarget>
		, public MetaTag<RenderTargetMeta>
	{
	public:
		float AspectRatio() const;

		RscHandle<Texture> GetAttachment(AttachmentType type,size_t index)const;
		size_t NumAttachments(AttachmentType type)const;


		size_t  AddAttachment(AttachmentType type, uint32_t size_x, uint32_t size_y);
		//virtual size_t  AddAttachment(AttachmentType type,uint32_t size_x, uint32_t size_y)=0;

		virtual void Finalize() {}; //Finalizes the framebuffer
		


		virtual ~RenderTarget() = default;
	protected:
		virtual void  AddAttachmentImpl(AttachmentType type, RscHandle<Texture> texture) {};
		//virtual size_t  AddAttachmentImpl(AttachmentType type, uint32_t size_x, uint32_t size_y) = 0;
		vector<size_t> attachments[AttachmentType::eSizeAT];
	};

}