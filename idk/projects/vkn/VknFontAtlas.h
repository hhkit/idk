#pragma once
#include "gfx/FontAtlas.h"
#include<vulkan/vulkan.hpp>
#include "idk.h"
//#include "gfx/Texture.h"
#include <vkn/MemoryAllocator.h>
#include <vkn/VknTexture.h>
#include <gfx/CompiledTexture.h>
namespace idk::vkn {
	class VknFontAtlas
		:public FontAtlas
	{
	public:

		RscHandle<VknTexture>   texture;

		//Required if you want the image to be able to be used in imgui (Cast to ImTextureID)
		
		RscHandle<Texture> Tex()const noexcept;
		opt<vk::DescriptorSet>	descriptorSet{};
		vk::Sampler Sampler()const { return texture->Sampler(); }
		vk::Image Image()const { return texture->Image(); }
		vk::ImageView ImageView()const { return texture->ImageView(); }
		vk::ImageAspectFlags ImageAspects();
		VknFontAtlas() = default;
		~VknFontAtlas();
		VknFontAtlas(VknFontAtlas&& rhs) noexcept;
		VknFontAtlas(const CompiledFontAtlas&);

		VknFontAtlas& operator=(VknFontAtlas&&) noexcept;
		using FontAtlas::Size;
		void Size(uvec2 new_size) override;
		virtual void* ID() const;

	private:
		void OnMetaUpdate(const FontAtlasMeta&);
		void UpdateUV(UVMode);
	};
}