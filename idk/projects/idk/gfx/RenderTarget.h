#pragma once
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <gfx/Texture.h>

namespace idk
{
	struct RenderTargetMeta
	{
		ivec2 size {1024, 1024};
		RscHandle<Texture> textures[2];
		bool is_world_renderer = true;
		bool render_debug = true;
		//RscHandle<Texture> depth_buffer;
	};

	class RenderTarget
		: public Resource<RenderTarget>
		, public MetaTag<RenderTargetMeta>
	{
	public:
		static constexpr uint32_t kColorIndex = 0;
		static constexpr uint32_t kDepthIndex = 1;
		float AspectRatio() const;
		RscHandle<Texture> GetColorBuffer();
		RscHandle<Texture> GetDepthBuffer();

		virtual void Finalize() {}; //Finalizes the framebuffer
		


		virtual ~RenderTarget() = default;
	protected:
		//virtual void  AddAttachmentImpl([[maybe_unused]]AttachmentType type, [[maybe_unused]] RscHandle<Texture> texture) {};
		//virtual size_t  AddAttachmentImpl(AttachmentType type, uint32_t size_x, uint32_t size_y) = 0;
		//vector<size_t> attachments[AttachmentType::eSizeAT];
	};

}